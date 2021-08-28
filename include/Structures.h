#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

#ifndef max_buffer
#define max_buffer 2000
#endif

typedef struct {
  char id[3];
  char version[2];
  char flags;
  char size[4];

} TagHeader;

typedef struct {
  char id[4];
  char size[4];
  // two flags are abc00000 and def00000
  uint8_t flags[2];

} FrameHeader;

typedef struct {
  char id[4];
  // size_type = 1 if size is measured in 2-byted units, 0 otherwise
  char size_type;
  int32_t size;
  char* text8;
  uint16_t* text16;
  // pos of the beginning of the text in the file
  long whence;
} TextFrame;

#define flags_size 3
#define SHOW 0x18
#define SET 0x16
#define GET 0x11
// 00000 - fpsvg; for example 00100 means -s flag is present
// possible resulting flags are: 11000, 10110, 10001
typedef struct args {
  char* prop_name;
  char* prop_value;
  char* filepath;
  uint8_t flag_mask;
} Args;

#endif