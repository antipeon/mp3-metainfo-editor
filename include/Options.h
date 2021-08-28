#ifndef OPTIONS_H
#define OPTIONS_H

#include <Structures.h>
#include <stdio.h>

void Show(TextFrame* frames);

void Get(TextFrame* frames, const char* prop_name);

void Set(FILE* file, TextFrame* frames, const char* prop_name,
         char* prop_value);

#endif
