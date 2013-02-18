# mgz-diff

Merguez-IT diff library

*This is a Work In Progress.* So test it but don't use it.

## Dependencies

### Compiling

* [cmake](http://www.cmake.org/)
* [MinGW](http://www.mingw.org/wiki/InstallationHOWTOforMinGW) (on Windows)

### Documentation

* [doxygen](http://doxygen.org/) (optional)
* [graphviz](http://graphviz.org/) (optional)

### Testing 

* [gtest](http://code.google.com/p/googletest/) 

## Compiling

Install [cmake](http://www.cmake.org/cmake/help/install.html), [doxygen](http://www.stack.nl/~dimitri/doxygen/install.html) and [graphviz](http://www.graphviz.org/Download.php), then :

### On MacOSX, or Linux

    cd <path to mgz-diff root>
    mkdir build
    cd build
    cmake ..
    make
    make test
    make doc
    make install

### On Windows

You need to install [MinGW](http://www.mingw.org/wiki/InstallationHOWTOforMinGW). Add the bin directory of your MinGW installation in your PATH. Then, open a Windows (not MSYS) console and :

    cd <path to mgz-diff root>
    mkdir build
    cd build
    cmake ..
    mingw32-make
    mingw32-make test
    mingw32-make doc
    mingw32-make install

## Licence

(C) 2013 Merguez-IT

This code was greatly inspired by parts of LibXDiff from Davide Libenzi
http://www.xmailserver.org/xdiff-lib.html

This code use part of the diff algorithm from git by Nicolas Pitre<br />
https://github.com/git/git<br />
(C) 2005 Nicolas Pitre (nico@fluxnic.net)

This code is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.


