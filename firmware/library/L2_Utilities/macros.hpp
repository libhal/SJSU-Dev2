// This file is meant for general purpose macros that can be used across the
// SJSU-Dev2 environment.
#pragma once
// SJ2_SECTION will place a variable or function within a given section of the
// executable. It uses both attribute "section" and "used". Section attribute
// places variable/function into that section and "used" labels the symbol as
// used to ensure that the compiler does remove this symbol at link time.
#define SJ2_SECTION(section_name) __attribute__((used, section(section_name)))
// SJ2_USED will use void casting as a means to convince the compiler that the
// variable has been used in the software, to remove compiler warnings about
// unused variables.
// NOTE: this will not stop the compiler from optimizing this variable out.
#define SJ2_USED(variable) (void)variable
// These macros are used to stringify define values. For example:
//
//      #define VALUE true
//      #define STRING "value = " STRINGIFY(s) => "value = true"
//
#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s