# tcomp

## Command-line text compressor

`tcomp` is a command-line single text file compressor.

## How it works

`tcomp` utilizes Huffman Coding to encode and decode the text file.
The main data structure of `tcomp` is the `HuffTree`.


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

