bmpbox
======

A static library for encoding general files to a bitmap image file.  

How to install **bmpbox**
----------------
First of, please make sure you have **qmake** and **GNU Toolchain** installed.  
Also **QT core libraries** is a must!

* Linux
  1. Run in a terminal:

     <code>qmake bmpbox.pro</code>  
     This command will generate a Makefile.
  2. In a terminal:  
     <code>sudo make install</code>
     Installs static library and headers into the system.


How to use **bmpbox**
-----------------------
BMPBox is a static library. Binaries are installed in <b>/usr/lib/bmpbox</b> and 
the headers can be found in <b>usr/include/bmpbox</b>. That is on most UNIX like systems.  
  
If you want to use BMPBox in your project please include this options at compile time:  
  
  <b>-L/path/to/the/static/library</b>  
  <b>-lbmpbox</b>  
  <b>-I/path/to/where/headers/are/installed</b>   
    
    
Licence
-------
Please take a look at **LICENSE** file

Contacts
--------

For any problems, questions, advices regarding coldswap please contact me at fioan89@gmail.com  
Bug reporting goes to https://github.com/fioan89/bmpbox/issues
