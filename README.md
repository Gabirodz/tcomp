# tcomp

## Command-line text compressor

`tcomp` is a simple command-line single text file compressor.

## How it works

`tcomp` is based on Huffman Coding to encode and decode the text file.

The main data structure of `tcomp` is the `HuffTree`.
![desc1](https://github.com/Gabirodz/tcomp/blob/main/docs/img/desc1.png)

The `HuffTree` is a multipurpose data structure, `CharFreqNodes` in the
`HuffTree` have both horizonal node connections (see above), and child connections (see below).
These connections have a single direction.

`tcomp` counts the frequency of each character and forms a linked list in the `HuffTree` using 
only horizontal connections. After all character have been counted, the list is sorted by frequency,
and then the two lowest `CharFreqNode` are joined to a `0` char parent, and the parent is inserted in
the proper spot according to the sum of the frequencies of its two childs. Repeating this process
leaves `HuffTree` with a single node, parent of the rest, forming a binary node whose leaves are the original `CharFreqNodes`, and
prioritizing that the most frequent characters stay closer to the tree parent.

![desc2](https://github.com/Gabirodz/tcomp/blob/main/docs/img/desc2.png)

`tcomp` assigns each character a code, by adding a `0` or a `1` depending how many left and right paths were chosen to 
get to the `CharFreqNode`. All character codes are then stored in a `CodeTable` with `CodeNodes`, which is simply a linked
list with all the characters and their respective codes as strings. 

![desc3](https://github.com/Gabirodz/tcomp/blob/main/docs/img/desc3.png)

The codes are written in the compressed file following the above structure.

To decompress the file, `tcomp` rebuilds the `HuffTree` by reading the contents of the first part of the file (up to decimal ASCII 4 "End of Transmission"). 
Then goes bit by bit in the second part, following the tree and writing the character to the output file every time it reaches a node with no childs (a leaf).

## How to run

On Linux:

Clone the repo and run the makefile.

```
$ git clone https://github.com/Gabirodz/tcomp.git
$ cd tcomp
$ make
```


Run the executable.

```
$ ./tcomp.exe <FILE_TO_COMPRESS> <OUTPUT_PATH>
```

## Build the docs

Note: Doxygen is required to build the docs.

```
$ git clone https://github.com/Gabirodz/tcomp.git
$ cd tcomp
$ doxygen Doxifile
$ cd docs/
```

## IMPORTANT

`tcomp` is a simple, quick project I made for a school assignment. It has not been thoroughly tested, so do not use it to compress important files and
always have a backup in case things go wrong. Decimal ASCII 3 "End Of Text" and Decimal ASCII 4 "End Of Transmission" are used by `tcomp` to form the binary compressed file, so compressing a file that includes these characters will go wrong.



