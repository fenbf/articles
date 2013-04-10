#Include test - cross#
Finally I am able to present some include experiment results! Previously [I wrote about Code Generator](http://www.bfilipek.com/2013/03/source-code-generator.html) and now I can actually run this tool and get some numbers out of it. I compared **VC11.0** (Visual Studio 2012 For Desktop) and **GCC 4.7.2** (using MinGW32 4.7.2 and DevCpp).

##Basics of experiment##

- **VC11.0** - [Visual Studio 2012 For Desktop](http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-for-windows-desktop), Release Mode, 32bit, no optimizations.
 - I turned build timings on to have detailed build performance.
- **GCC 4.7.2 32bit** - MinGW 4.7.2 version and run from [DevCpp 5.4.1](http://sourceforge.net/projects/orwelldevcpp/)
 - g++.exe -c testHeaders.cpp -o testHeaders.o **-ftime-report**
- **The machine**: Core i5, 4 cores, 4GB RAM, Windows 8 64bit
 - compilation will probably take place only on one core only (there will be one translation unit only)

I run each test 3 times and then compute the average. At the end of post there is a link to detailed spreadsheet. 

##Code structure##
img here...

* `testHeader.cpp` includes N header files
* m-th header file includes N-1 other header files (so we have "cross" include)
* each header file has its proper include guard

**Note**: for 199 got "include level... "

##Test N headers ##
**generator.exe N 100 includeTestOutput/**

**N = 100**
<table>
	<tr><td>GCC</td><td>3,95s</td></tr>
	<tr><td>GCC Compilation</td><td>2,99s</td></tr>
	<tr><td>VC11.0</td><td>2,90s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>2,68s</td></tr>
</table>

**N = 132**
<table>
	<tr><td>GCC</td><td>5,37s</td></tr>
	<tr><td>GCC Compilation</td><td>3,98s</td></tr>
	<tr><td>VC11.0</td><td>4,31s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>4,11s</td></tr>
</table>

**N = 164**
<table>
	<tr><td>GCC</td><td>6,49s</td></tr>
	<tr><td>GCC Compilation</td><td>4,92s</td></tr>
	<tr><td>VC11.0</td><td>6,10s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>5,91s</td></tr>
</table>

**N = 192**
<table>
	<tr><td>GCC</td><td>7,40s</td></tr>
	<tr><td>GCC Compilation</td><td>5,77s</td></tr>
	<tr><td>VC11.0</td><td>7,98s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>7,77s</td></tr>
</table>

**Result plot**
image here...

##Test N headers - additional ifDef ##
**generator.exe N 100 includeTestOutput/ ifDef**

**N = 100**
<table>
	<tr><td>GCC</td><td>3,91s</td></tr>
	<tr><td>GCC Compilation</td><td>2,96s</td></tr>
	<tr><td>VC11.0</td><td>1,44s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,22s</td></tr>
</table>

**N = 132**
<table>
	<tr><td>GCC</td><td>5,35s</td></tr>
	<tr><td>GCC Compilation</td><td>3,91s</td></tr>
	<tr><td>VC11.0</td><td>1,71s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,51s</td></tr>
</table>

**N = 164**
<table>
	<tr><td>GCC</td><td>6,41s</td></tr>
	<tr><td>GCC Compilation</td><td>4,86s</td></tr>
	<tr><td>VC11.0</td><td>1,98s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,77s</td></tr>
</table>

**N = 192**
<table>
	<tr><td>GCC</td><td>7,31s</td></tr>
	<tr><td>GCC Compilation</td><td>5,69s</td></tr>
	<tr><td>VC11.0</td><td>2,16s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,96s</td></tr>
</table>

**Result plot**
image here...

##Test N headers - #pragma once ##
**generator.exe N 100 includeTestOutput/ pragmaOnce**

**N = 100**
<table>
	<tr><td>GCC</td><td>4,02s</td></tr>
	<tr><td>GCC Compilation</td><td>3,08s</td></tr>
	<tr><td>VC11.0</td><td>1,48s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,28s</td></tr>
</table>

**N = 132**
<table>
	<tr><td>GCC</td><td>5,42s</td></tr>
	<tr><td>GCC Compilation</td><td>4,06s</td></tr>
	<tr><td>VC11.0</td><td>1,84s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,65s</td></tr>
</table>

**N = 164**
<table>
	<tr><td>GCC</td><td>6,64s</td></tr>
	<tr><td>GCC Compilation</td><td>5,08s</td></tr>
	<tr><td>VC11.0</td><td>2,06s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>1,86s</td></tr>
</table>

**N = 192**
<table>
	<tr><td>GCC</td><td>7,60s</td></tr>
	<tr><td>GCC Compilation</td><td>5,98s</td></tr>
	<tr><td>VC11.0</td><td>2,39s</td></tr>
	<tr><td>VC11.0 Compilation</td><td>2,20s</td></tr>
</table>

**Result plot**
image here...

##Conclusion##
- The code structure is rather theoretical and does not represent 'common' structures that may appear in projects.
- GCC and VC build code a bit different. GCC linker phase is much longer than in VC.
- **GCC uses lots of optimization** for header files. There is almost no need to do any 'tricks' with includes. Header guards are enough.
- VC likes header files 'tricks'!
  - There is above **2X to 3X speedup** using additional include guards or pragma once
  - **Pragma once** for such code structure seems to be a bit slower than additional include guards: **from 5% to even 10% slower**.
- All in all **VC11.0 is faster** than GCC.

##What I have learnt##
* VC Release mode is not the same as GCC basic compilation
* Make sure you set up proper experiment base
* Automate as much as possible
* Tool for spreadsheet is a must have-software :)
* It is valuable to figure out how to break the compiler