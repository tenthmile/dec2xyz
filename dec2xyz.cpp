/*
Copyright (c) 2018 tenthmile

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#include <iostream>
#include <sstream>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <unistd.h>

std::string letters = "0123456789abcdef"; // default output is number to lowercase hexadecimal

void print_help(char** argv, std::ostream &output=std::cerr, bool extended=false) {
  output << "Usage: " << argv[0] << " [-e expression] [-s] [letters]\n";
  if (extended) {
    output << std::endl
           << "Parameters:" << std::endl
           << "\t-s Spreadsheet-Column-Counting style (e.g. ...x,y,z,aa,ab,... not like ...x,y,z,ba,bb,bc...)." << std::endl
           << "\t-1 start counting from 1." << std::endl
           << "\t-h Will print this help."
           << std::endl << std::endl
           << "Information:" << std::endl
           << "\t-e and letters can't be combined" << std::endl
           << std::endl << std::endl
           << "Examples:" << std::endl
           << "$ seq 0 16 | " << argv[0] << "" << std::endl << "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\na\nb\nc\nd\ne\nf\n10\n" << std::endl
           << "$ seq 0 8 | " << argv[0] << " 01" << std::endl << "0\n1\n10\n11\n100\n101\n110\n111\n1000\n" << std::endl
           << "$ seq 8 11 | " << argv[0] << " -e \"[0-9][A-F]\"" << std::endl << "8\n9\nA\nB\n" << std::endl
           << "$ seq 1 3 | "  << argv[0] << " -1 -e \"[a-z]\"" << std::endl << "a\nb\nc\n" << std::endl
           << "$ seq 23 27 | "  << argv[0] << " -s -e \"[A-Z]\"" << std::endl << "X\nY\nZ\nAA\nAB\n" << std::endl
           << "Exotic Examples:" << std::endl
           << "$ echo 0 | "  << argv[0] << " -e \"[[-[]\"" << std::endl << "[\n" << std::endl
           << "$ seq 0 3 | "  << argv[0] << " -1 -s \"|\"" << std::endl << "\n|\n||\n|||\n" << std::endl
           << std::endl;
  }
}

int main(int argc, char** argv) {
  bool spreadSheetCounting = false;
  bool notZeroIndexed = false;
  bool expressionUsed = false;
  
  int opt;
  while ((opt = getopt(argc, argv, "e:s1h")) != -1) {
    switch (opt) {
    case 's':
      spreadSheetCounting = true;
      break;
    case '1':
      notZeroIndexed = true;
      break;
    case 'e':
      {
        expressionUsed = true;
        
        std::stringstream ss;
        bool withinBracket = false;
        char scopeStartLetter = '\0';
        char scopeEndLetter = '\0';
        int posWithinBracket = 0;
    
        char* currLetter = optarg;
        do {
          if (!withinBracket && *currLetter == '[') {
            withinBracket = true;
            posWithinBracket = 0;
          }

          if (withinBracket) {
            assert(posWithinBracket >= 0);
            if (posWithinBracket == 1)
              scopeStartLetter = *currLetter;
            else if (posWithinBracket == 2 && *currLetter != '-') {
              print_help(argv);
              exit(EXIT_FAILURE);
            } else if (posWithinBracket == 3) {
              scopeEndLetter = *currLetter;
            } else if (posWithinBracket == 4) {
              if (*currLetter != ']' || scopeEndLetter < scopeStartLetter) {
                print_help(argv);
                exit(EXIT_FAILURE);
              }
          
              do {
                ss << scopeStartLetter;
                scopeStartLetter++;
              } while (scopeStartLetter <= scopeEndLetter);
              scopeStartLetter = '\0';
              scopeEndLetter = '\0';
              withinBracket = false;
            }
            assert(posWithinBracket <= 4);

            posWithinBracket++;
          } else {
            ss << *currLetter;
          }
        } while (*(++currLetter));
    
        letters = ss.str();
        break;
      }
    case 'h':
      print_help(argv, std::cout, true);
      return 0;
      break;
    default: // '?'
      std::cerr << "You entered the invalid char: " << (char)optopt << std::endl;
      std::cerr << "Write \"" << argv[0] << " -h\" for help." << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  if (argc - optind >= 2 || (expressionUsed && argc - optind == 1)) {
    std::cerr << "Too many arguments" << std::endl;
    std::cerr << "Write \"" << argv[0] << " -h\" for help." << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!expressionUsed && optind < argc)
    letters = std::string(argv[optind]);
  
  bool gotInput = false;
  int letterCount = letters.size();
  int number;
  while (std::cin >> number) {
    if (notZeroIndexed)
      number--;
    
    gotInput = true;
    std::stringstream ss;
    if (number == 0) {
      ss << letters[0];
    } else {
      while ((number > 0 && !spreadSheetCounting) || (number >= 0 && spreadSheetCounting)) {
        int unit = number % letterCount;
        ss << letters[unit];
        number = number / letterCount;
        if (spreadSheetCounting)
          number--;
      }
    }
    std::string output = ss.str();
    std::reverse(output.begin(), output.end());
    std::cout << output << std::endl;
  }
  return !gotInput;
}
