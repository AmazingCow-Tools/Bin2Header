# Bin2Header

**Made with <3 by [Amazing Cow](http://www.amazingcow.com).**



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Description:

```bin2header``` - Convert any file to a C/C++ header.

```bin2header``` is a tool to create a header file with the contents of 
any given file into an array of hexdecimal values.    
This can be util in cases that we want embedding the file direct into program.

Beware that the size of output file can grow very fast.

<br>

As usual, you are **very welcomed** to **share** and **hack** it.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Usage:

``` 
   bin2header -h | -v 
   bin2header [-V] [-ib <size>] [-a <name>] [-o <filename>] <input-filename> 

Options: 
  *-h --help    : Show this screen. 
  *-v --version : Show app version and copyright. 

   -V -verbose : Enable Verbose mode. 

   -i --indent-size <size> : Indentation in spaces. 
   -b --block-size  <size> : How many block will be read each time. 
                             Greater block size increases speed in 
                             expense of memory. 

   -a --array-name <name> : The name of resulting array. 
                            If not set will be the same of output filename. 
   -o --output <filename> : The name of output file. 
                            If not set will be the same of input filename  
                            with the '.h' extension. 

Notes:
  In (-a <name>) all characters that doesn't make a valid C identifier will 
  be replaced with '_' (underscore) character.

  Options marked with * are exclusive, i.e. the bin2header will run that 
  and exit successfully after the operation.
```



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Install:

Use the Makefile.

``` bash
    make install
```

Or to uninstall

``` bash
    make uninstall
```



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Dependencies:

This project uses / depends on:

* AmazingCow's [libstdcow](http://www.github.com/AmazingCow-Libs/libstdcow).    
  It's a submodule so a ```git submodule update --init --recursive```
  will init it nicely.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Environment and Files: 

```bin2header``` do not create / need any other files or environment vars.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## License:

This software is released under GPLv3.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## TODO:

Check the TODO file for general things.

This projects uses the COWTODO tags.   
So install [cowtodo](https://github.com/AmazingCow-Tools/COWTODO/) and run:

``` bash
$ cd path/for/the/project
$ cowtodo 
```

That's gonna give you all things to do :D.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## BUGS:

We strive to make all our code the most bug-free as possible - But we know 
that few of them can pass without we notice ;).

Please if you find any bug report to [bugs_opensource@amazingcow.com]() 
with the name of this project and/or create an issue here in Github.



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Source Files:

* AUTHORS.txt
* CHANGELOG.txt
* COPYING.txt
* Makefile
* README.md
* TODO.txt
* src/bin2header.c



<!-- ####################################################################### -->
<!-- ####################################################################### -->

## Others:
Check our repos and take a look at our [open source site](http://opensource.amazingcow.com).
