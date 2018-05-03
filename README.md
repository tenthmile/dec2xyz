# Compile
```bash
$ g++ -o dec2xyz dec2xyz.cpp -lgmp
```

# dec2xyz
Convert (positive) numbers from the command line to any other representation.

# Examples
```bash
$ seq 0 16 | ./dec2xyz
0
1
2
3
4
5
6
7
8
9
a
b
c
d
e
f
10

$ echo f | ./dec2xyz -n
15

$ seq 0 8 | ./dec2xyz 01
0
1
10
11
100
101
110
111
1000

$ echo 111 | ./dec2xyz -n 01
7

$ seq 8 11 | ./dec2xyz -e "[0-9][A-F]"
8
9
A
B

$ seq 1 3 | ./dec2xyz -1 -e "[a-z]"
a
b
c

$ seq 23 27 | ./dec2xyz -s -e "[A-Z]"
X
Y
Z
AA
AB

$ echo AAAA | ./dec2xyz -n -s -e "[A-Z]"
18278
```


# Negative Number Examples
```bash
$ printf "%4s\n" $(seq 1 8 | ./dec2xyz -1 10) | tr " " "1"
1111
1110
1101
1100
1011
1010
1001
1000

$ printf "%4s\n" $(seq 1 128 | ./dec2xyz -1 -e "[f-a][9-0]") | tr " " "f"
ffff
fffe
fffd
...
ff82
ff81
ff80
```


# Exotic Examples
```bash
$ echo 0 | ./dec2xyz -e "[[-[]"
[

$ seq 0 3 | ./dec2xyz -1 -s "|"

|
||
|||

$ echo aaaaa | ./dec2xyz -n -1 -s a
5
```


# Create negative numbers more easily
```bash
dec2xyzNegativeToDec() {
    for last in "$@"; do true; done
    units=$1
    lastChar=$(echo $last | tr -d "[" | head -c 1)
    shift
    # bash
    eval dec2xyz -1 "$@" | xargs -n1 printf "%${units}s\n" | tr " " $lastChar
    # zsh
    # eval dec2xyz -1 ${(q):-$@} | xargs -n1 printf "%${units}s\n" | tr " " $lastChar
}

$ dec2xyzNegative 8 -e "[f-a][9-0]"
20000
ffffb1e0
$ dec2xyzNegative 4 10 
1
1111
2
1110
8 
1000
```
