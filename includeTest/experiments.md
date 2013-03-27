##Basics##
computer, compiler versions, etc, intrduction

**VC 2012**
Release, 32bit, no optimizations

**GCC 4.7.2 32bit**
g++.exe -c testHeaders.cpp -o testHeaders.o -ftime-report

for 199 got "include level... "

##test 1##
**generator.exe 100 100 includeTest\**

Generates 100 header files, each file contains a class declaration with 100 different member variables

File structure:

picture here

**Results:**
<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>4,25</td>
        <td>3,20</td>
		<td>5.174</td>
		<td>2.741</td>
	</tr>
	<tr>
		<td>2</td>
		<td>4,05</td>
		<td>3,04</td>
		<td>5.051</td>
		<td>2.628</td>
	</tr>
	<tr>
		<td>3</td>
		<td>4,06</td>
		<td>3.03</td>
		<td>5.059</td>
		<td>2.651</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>4.12</b></td>
		<td><b>3.09</b></td>
		<td><b>5.094</b></td>
		<td><b>2.673</b></td>
	</tr>
</table>

**Using additional include guards**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>4.00</td>
		<td>2.98</td>
		<td>3.709</td>
		<td>1.220</td>
	</tr>
	<tr>
		<td>2</td>
		<td>4.03</td>
		<td>3.03</td>
		<td>3.663</td>
		<td>1.197</td>
	</tr>
	<tr>
		<td>3</td>
		<td>4.03</td>
		<td>3.01</td>
		<td>3.703</td>
		<td>1.191</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>4.02</b></td>
		<td><b>3.00</b></td>
		<td><b>3.691</b></td>
		<td><b>1.202</b></td>
	</tr>
</table>

**Using #pragma once**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>4.16</td>
		<td>3.14</td>
		<td>3.720</td>
		<td>1.229</td>
	</tr>
	<tr>
		<td>2</td>
		<td>4.14</td>
		<td>3.13</td>
		<td>3.714</td>
		<td>1.253</td>
	</tr>
	<tr>
		<td>3</td>
		<td>4.14</td>
		<td>3.13</td>
		<td>3.703</td>
		<td>1.233</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>4.15</b></td>
		<td><b>3.13</b></td>
		<td><b>3.712</b></td>
		<td><b>1.238</b></td>
	</tr>
</table>

##test 2##
**generator.exe 132 100 inludeTest\**

Generates 196 header files, each file contains a class declaration with 100 different member variables

File structure:

picture here

**Results:**
<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>5,39</td>
        <td>3,98</td>
		<td>7.490</td>
		<td>4.315</td>
	</tr>
	<tr>
		<td>2</td>
		<td>5,39</td>
		<td>3,98</td>
		<td>7.102</td>
		<td>3.989</td>
	</tr>
	<tr>
		<td>3</td>
		<td>5,41</td>
		<td>3.97</td>
		<td>7.185</td>
		<td>4.044</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>5.39</b></td>
		<td><b>3.98</b></td>
		<td><b>7.259</b></td>
		<td><b>4.116</b></td>
	</tr>
</table>

**Using additional include guards**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>5.39</td>
		<td>3.95</td>
		<td>4.557</td>
		<td>1.422</td>
	</tr>
	<tr>
		<td>2</td>
		<td>5.36</td>
		<td>3.95</td>
		<td>4.668</td>
		<td>1.425</td>
	</tr>
	<tr>
		<td>3</td>
		<td>5.41</td>
		<td>3.96</td>
		<td>4.728</td>
		<td>1.448</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>5.38</b></td>
		<td><b>3.95</b></td>
		<td><b>4.659</b></td>
		<td><b>1.438</b></td>
	</tr>
</table>

**Using #pragma once**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>5.44</td>
		<td>4.01</td>
		<td>4.688</td>
		<td>1.510</td>
	</tr>
	<tr>
		<td>2</td>
		<td>5.50</td>
		<td>4.07</td>
		<td>4.666</td>
		<td>1.496</td>
	</tr>
	<tr>
		<td>3</td>
		<td>5.52</td>
		<td>4.07</td>
		<td>4.757</td>
		<td>1.530</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>5.48</b></td>
		<td><b>4.07</b></td>
		<td><b>4.703</b></td>
		<td><b>1.512</b></td>
	</tr>
</table>

##test 3##
**generator.exe 164 100 inludeTest\**

Generates 164 header files, each file contains a class declaration with 100 different member variables

File structure:

picture here

**Results:**
<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>6,61</td>
        <td>4,96</td>
		<td>7.490</td>
		<td>4.315</td>
	</tr>
	<tr>
		<td>2</td>
		<td>6,59</td>
		<td>4,94</td>
		<td>7.102</td>
		<td>3.989</td>
	</tr>
	<tr>
		<td>3</td>
		<td>6,58</td>
		<td>4.94</td>
		<td>7.185</td>
		<td>4.044</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>6.59</b></td>
		<td><b>4.95</b></td>
		<td><b>7.259</b></td>
		<td><b>4.116</b></td>
	</tr>
</table>

**Using additional include guards**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>5.39</td>
		<td>3.95</td>
		<td>4.557</td>
		<td>1.422</td>
	</tr>
	<tr>
		<td>2</td>
		<td>5.36</td>
		<td>3.95</td>
		<td>4.668</td>
		<td>1.425</td>
	</tr>
	<tr>
		<td>3</td>
		<td>5.41</td>
		<td>3.96</td>
		<td>4.728</td>
		<td>1.448</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>5.38</b></td>
		<td><b>3.95</b></td>
		<td><b>4.659</b></td>
		<td><b>1.438</b></td>
	</tr>
</table>

**Using #pragma once**

<table>
    <tr>
        <td>Run</td>
		<td>GCC 4.7.2</td>
		<td>GCC 4.7.2 (compile only)</td>
		<td>VC 2012</td>
		<td>VC 2012 (compile only)</td>
    </tr>
    <tr>
		<td>1</td>
		<td>5.44</td>
		<td>4.01</td>
		<td>4.688</td>
		<td>1.510</td>
	</tr>
	<tr>
		<td>2</td>
		<td>5.50</td>
		<td>4.07</td>
		<td>4.666</td>
		<td>1.496</td>
	</tr>
	<tr>
		<td>3</td>
		<td>5.52</td>
		<td>4.07</td>
		<td>4.757</td>
		<td>1.530</td>
	</tr>
	<tr>
		<td><b>Average</b></td>
		<td><b>5.48</b></td>
		<td><b>4.07</b></td>
		<td><b>4.703</b></td>
		<td><b>1.512</b></td>
	</tr>
</table>