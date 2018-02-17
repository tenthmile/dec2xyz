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
```

# Exotic Examples
```bash
$ echo 0 | ./dec2xyz -e "[[-[]"
[

$ seq 0 3 | ./dec2xyz -1 -s "|"

|
||
|||
```
