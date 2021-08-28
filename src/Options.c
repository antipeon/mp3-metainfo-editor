#include <FileEditor.h>
#include <Structures.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Show(TextFrame* frames) {
  for (int i = 0; frames[i].id[0] != '\0'; ++i) {
    printf("Property Name: %s\n", frames[i].id);
    printf("Size: %d\n", frames[i].size);
    PrintPropertyValue(frames, i);
    printf("\n");
  }
}

void Get(TextFrame* frames, const char* prop_name) {
  uint32_t prop_name_size = 4;
  if (strlen(prop_name) != prop_name_size) {
    fprintf(stderr, "%s\n", "Incorrect Property Name");
    exit(-1);
  }
  int32_t index = GetIndex(frames, prop_name);
  if (index != -1) {
    PrintPropertyValue(frames, index);
    return;
  }
  fprintf(stderr, "%s\n", "Incorrect Property Name");
  exit(-1);
}

void Set(FILE* file, TextFrame* frames, const char* prop_name,
         char* prop_value) {
  int32_t index = GetIndex(frames, prop_name);

  if (index == -1) {
    fprintf(stderr, "%s\n", "Incorrect Property Name");
    exit(-1);
  }
  int32_t length_prop_value = strlen(prop_value);
  if (length_prop_value > frames[index].size) {
    fprintf(stderr, "%s\n", "Property Value is too Long");
    exit(-1);
  }
  if (length_prop_value < frames[index].size) {
    strcat(prop_value, "\0");
    length_prop_value++;
  }

  fseek(file, frames[index].whence, SEEK_SET);
  UpdateProperty(file, frames, index, prop_name, prop_value, length_prop_value);
}
