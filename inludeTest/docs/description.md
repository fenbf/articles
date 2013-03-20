#Test Source Generator #
Generates C++ source code file structure that can be use to test compilation/build performance.

##What?##
We would like to create:

**A header file**

	#pragma once // optional
	#ifndef _INCLUDED_HEADER_HEADER_0_H
	    #define _INCLUDED_HEADER_HEADER_0_H
	
	// optional extra line
	#ifndef STD_STRING_INCLUDED
	    #define STD_STRING_INCLUDED
	    #include <string>
	#endif
		
	// includes (with optional header guard)
	#ifndef _INCLUDED_HEADER_HEADER_1_H
	    #include "header_1.h"
	#endif 
	#ifndef _INCLUDED_HEADER_HEADER_2_H
	    #include "header_2.h"
	#endif 
    // ...

    // class declaration here

**A class declaration**

	// class (complexity = 3): 
	class header_0_Class
	{
	private:
	    int    m_variable_0;
	    double m_variable_1;
	    float  m_variable_2;	
	public:
	    header_0_Class()
	     :  m_variable_0( 0 )
	     ,  m_variable_1( 0.0 )
	     ,  m_variable_2( 0.0f )
	    {    }
	
	    void setm_variable_0(int v)    { m_variable_0 = v; }	
	    int getm_variable_0() const    { return m_variable_0; }	
	    void setm_variable_1(double v) { m_variable_1 = v; }	
	    double getm_variable_1() const { return m_variable_1; }	
	    void setm_variable_2(float v)  { m_variable_2 = v; }	
	    float getm_variable_2() const  { return m_variable_2; }	
	};    
It is a simple class with N-different variables with getters and setters.

**A Cpp file**

	#ifndef _INCLUDED_HEADER_HEADER_0_H
	    #include "header_0.h"
	#endif 
	#ifndef _INCLUDED_HEADER_HEADER_1_H
	    #include "header_1.h"
	#endif 
	#ifndef _INCLUDED_HEADER_HEADER_2_H
	    #include "header_2.h"
	#endif 
	
	int main()
	{
	    header_0_Class object_header_0_Class;
	    header_1_Class object_header_1_Class;
	    header_2_Class object_header_2_Class;
	    return 0;
	}

Source file where all of our classes are used.

##Why?##
We would like to test compile time of such code structure. We have several options to test:

* **code structure**: Simple test involves only one cpp file that include all header files. Each header file includes all other header files as well. But we can create some other structures.
* **#pragma once**: How this improves the compilation time?
* **additional include guard**: How this improves the compilation time?

##Structure##
**Generator** 

Main class that is runs the generation process. Created in `main`, in `generator.cpp` file.

It has some important members:

* *m_headerCount* - number of headers to generate
* *m_complexCount* - how generated classes are complex, or simply how many fields they have.

**SourceFile** - Base class that describe source file to be generated and the creation method. Holds info like *fileName*, *type* (Header or Cpp), and a list of header files that are included - both *.h* files and *.cpp* files can include things.

**HeaderFile** - Can create a header file.

**CppFile** - Can create a cpp file.

**ClassGenerator** - Knows how to create declaration of a class.

##Examples##

**generator.exe 3 3 path ifDef pragmaOnce**

Generates 3 header files with 3 different classes that have 3 members inside. We use all headers options so both pragma and additional include guards will be used.

	#pragma once
	#ifndef _INCLUDED_HEADER_HEADER_0_H
	    #define _INCLUDED_HEADER_HEADER_0_H
	
	#ifndef STD_STRING_INCLUDED
	    #define STD_STRING_INCLUDED
	    #include <string>
	#endif

	#ifndef _INCLUDED_HEADER_HEADER_1_H
	    #include "header_1.h"
	#endif 
	#ifndef _INCLUDED_HEADER_HEADER_2_H
	    #include "header_2.h"
	#endif 

**generator.exe 3 3 path**

Same as above, but header file looks like that:

	#ifndef _INCLUDED_HEADER_HEADER_0_H
	    #define _INCLUDED_HEADER_HEADER_0_H
	
	#include <string>
	#include "header_1.h"
	#include "header_2.h"


##Todo##
* Classes like SourceFile, HeaderFile and CppFile violates **SRP** (they hold info about a file and at the same time they are used to generate such file). Maybe there should be some additional class like *HeaderGenerator*, *CppGenerator* or similar.
* automatic makefile/project creation would be helpful as well