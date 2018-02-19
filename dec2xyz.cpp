#include <iostream>
#include <sstream>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <unistd.h>
#include "big_integer.hpp"

std::string letters = "0123456789abcdef"; // default output is number to lowercase hexadecimal

void print_help(char** argv, std::ostream &output=std::cerr, bool extended=false) {
  output << "Usage: " << argv[0] << " [-e expression] [-s] [-n] [letters]\n";
  if (extended) {
    output << std::endl
           << "Parameters:" << std::endl
           << "\t-s Spreadsheet-Column-Counting style (e.g. ...x,y,z,aa,ab,... not like ...x,y,z,ba,bb,bc...)." << std::endl
           << "\t-1 start counting from 1." << std::endl
           << "\t-n to number from input." << std::endl
           << "\t-h Will print this help."
           << std::endl << std::endl
           << "Information:" << std::endl
           << "\t-e and letters can't be combined" << std::endl
           << std::endl << std::endl
           << "Examples:" << std::endl
           << "$ seq 0 16 | " << argv[0] << "" << std::endl << "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\na\nb\nc\nd\ne\nf\n10\n" << std::endl
           << "$ echo f | " << argv[0] << " -n" << std::endl << "15\n" << std::endl
           << "$ seq 0 8 | " << argv[0] << " 01" << std::endl << "0\n1\n10\n11\n100\n101\n110\n111\n1000\n" << std::endl
           << "$ echo 111 | " << argv[0] << " -n 01" << std::endl << "7\n" << std::endl
           << "$ seq 8 11 | " << argv[0] << " -e \"[0-9][A-F]\"" << std::endl << "8\n9\nA\nB\n" << std::endl
           << "$ seq 1 3 | "  << argv[0] << " -1 -e \"[a-z]\"" << std::endl << "a\nb\nc\n" << std::endl
           << "$ seq 23 27 | "  << argv[0] << " -s -e \"[A-Z]\"" << std::endl << "X\nY\nZ\nAA\nAB\n" << std::endl
           << "$ echo AAAA | "  << argv[0] << " -n -s -e \"[A-Z]\"" << std::endl << "18278\n" << std::endl
           << "Negative Number Examples:" << std::endl
           << "$ printf \"%4s\\n\" $(seq 1 8 | ./dec2xyz -1 10) | tr \" \" \"1\"" << std::endl << "1111\n1110\n1101\n1100\n1011\n1010\n1001\n1000\n" << std::endl
           << "$ printf \"%4s\\n\" $(seq 1 128 | ./dec2xyz -1 -e \"[f-a][9-0]\") | tr \" \" \"f\"" << std::endl << "ffff\nfffe\nfffd\n...\nff82\nff81\nff80\n" << std::endl << std::endl
           << "Exotic Examples:" << std::endl
           << "$ echo 0 | "  << argv[0] << " -e \"[[-[]\"" << std::endl << "[\n" << std::endl
           << "$ seq 0 3 | "  << argv[0] << " -1 -s \"|\"" << std::endl << "\n|\n||\n|||\n" << std::endl
           << "$ echo aaaaa | "  << argv[0] << " -n -1 -s a" << std::endl << "5" << std::endl;
  }
}

int main(int argc, char** argv) {
  std::ios_base::sync_with_stdio(false);

  // settings
  bool spreadSheetCounting = false;
  bool notZeroIndexed = false;
  bool expressionUsed = false;
  bool reverseCalculation = false;

  // read parameters
  int opt;
  while ((opt = getopt(argc, argv, "e:s1nh")) != -1) {
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
              if (*currLetter != ']') {
                print_help(argv);
                exit(EXIT_FAILURE);
              }
          
              while (scopeStartLetter != scopeEndLetter) {
                ss << scopeStartLetter;
                if (scopeStartLetter < scopeEndLetter)
                  scopeStartLetter++;
                else if (scopeStartLetter > scopeEndLetter)
                  scopeStartLetter--;
              }
              ss << scopeEndLetter;
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
    case 'n':
      reverseCalculation = true;
      break;
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

  // calculation
  bool gotInput = false;
  unsigned long int letterCount = letters.size();
  std::string input;
  BigInteger number;
  while (std::cin >> input) {
    if (!reverseCalculation) {
      number = input;

      if (notZeroIndexed)
        number -= 1;
    
      gotInput = true;
      std::ostringstream ss;
      if (number == 0ul) {
        ss << letters[0];
      } else {
        while ((number > 0ul && !spreadSheetCounting) || (number >= 0ul && spreadSheetCounting)) {
          unsigned long int remainder = number % letterCount;
          ss << letters[remainder];
          number /= letterCount;
          if (spreadSheetCounting)
            number -= 1;
        }
      }
      std::string output = ss.str();
      std::reverse(output.begin(), output.end());
      std::cout << output << std::endl;
    } else {
      number = 0ul;
      bool valid = true;
      for (size_t i=0;i<input.size();++i) {
        size_t pos = letters.find(input[i]);
        if (pos == std::string::npos) {
          valid = false;
          break;
        }
        if (spreadSheetCounting && i!=0)
          number += 1;
        number *= letterCount;
        number += pos;
      }
      if (valid) {
        gotInput = true;
        if (notZeroIndexed)
          number += 1;
        std::cout << number << std::endl;
      }
    }
  }
  return !gotInput;
}
