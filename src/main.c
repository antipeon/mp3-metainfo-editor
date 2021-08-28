#include <ArgParser.h>
#include <FileEditor.h>
#include <Options.h>
#include <Structures.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* const argv[]) {
  Args* arguments_ptr = ParseArgs(argc, argv);
  Args arguments = *(arguments_ptr);

  FILE* file = fopen(arguments.filepath, "r+");

  if (file == NULL) {
    fprintf(stderr, "%s\n", "error opening file");
    exit(-1);
  }
  TextFrame frames[max_buffer];
  InitTextFrames(frames, max_buffer);
  ParseFile(file, frames, max_buffer);

  if (arguments.flag_mask == SHOW) {
    Show(frames);
  } else if (arguments.flag_mask == SET) {
    Set(file, frames, arguments.prop_name, arguments.prop_value);
  } else if (arguments.flag_mask == GET) {
    Get(frames, arguments.prop_name);
  } else {
    assert(0 && "not reachable");
  }

  fclose(file);
  FreeArgsMemory(arguments_ptr);
  return 0;
}
