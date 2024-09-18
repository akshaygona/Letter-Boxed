# Letter-Boxed

## An implementation of the game in C!


Compile code with the following flags:`-std=c17 -O2 -Wall -Wextra -Werror -pedantic -o letter-boxed`.
Or, just run `make letter-boxed`



This program plays the New York Times puzzle [Letter Boxed](https://www.nytimes.com/puzzles/letter-boxed). 
It takes two command line arguments -- a board file and a dictionary file. Then, it reads words from standard input (`STDIN`) until the board is solved, or until an invalid solution is attempted.

Here is what it looks like to run the program:

```bash
$ ./letter-boxed board1.txt dict.txt
```

```bash
Here is an example board:


    r  o  k
  +---------+
  |         |
 w|         |e
  |         |
 f|         |d
  |         |
 a|         |n
  |         |
  +---------+
    l  c  i

```

A solution to this board could be
```
flan
now
wreck
kid
```

Board input looks as follows for the above board:

```bash
$ cat board1.txt
rok
edn
lci
wfa
```

The dictionary file will have one word per line. The provided dict.txt is a sample dictionary.

When run, the user may encounter errors should the game be played correctly. The error messages are to help state what a player is doing wrong and guide to correct solution.
