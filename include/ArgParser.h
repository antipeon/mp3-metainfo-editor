#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <Structures.h>
#include <stdint.h>

void SetArgFlags(uint8_t* flag_mask, const char current_option);

void SetPropertyFields(int current_option, char* prop_name, char* prop_value,
                       char* filepath);

Args* ParseArgs(int argc, char* const argv[]);

void FreeArgsMemory(Args* arguments_ptr);

#endif
