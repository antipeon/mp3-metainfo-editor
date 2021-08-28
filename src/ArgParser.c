#include <ArgParser.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SetArgFlags(uint8_t* flag_mask, const char current_option) {
  switch (current_option) {
    case 'f':
      *flag_mask |= (1 << 4);
      break;

    case 'p':
      *flag_mask |= (1 << 3);
      break;

    case 's':
      *flag_mask |= (1 << 2);
      break;

    case 'v':
      *flag_mask |= (1 << 1);
      break;

    case 'g':
      *flag_mask |= 1;
      break;

    default:
      return;
  }
}

void SetPropertyFields(int current_option, char* prop_name, char* prop_value,
                       char* filepath) {
  switch (current_option) {
    case 0:
      // no options setting a flag
      break;

    case 'p':
      // puts ("option --show\n");
      break;

    case 'f':
      // printf ("option --filepath with path '%s'\n", optarg);
      strcpy(filepath, optarg);
      break;

    case 's':
      // printf ("option --set with prop_name '%s'\n", optarg);
      strcpy(prop_name, optarg);
      break;

    case 'v':
      // printf ("option --value with prop_value '%s'\n", optarg);
      strcpy(prop_value, optarg);
      break;

    case 'g':
      // printf ("option --get with prop_name '%s'\n", optarg);
      strcpy(prop_name, optarg);
      break;

    case '?':
      // getopt_long already printed an error message
      break;

    default:
      exit(-1);
  }
}

Args* ParseArgs(int argc, char* const argv[]) {
  if (argc > 4) {
    fprintf(stderr, "%s\n", "too many command line arguments");
    exit(-1);
  }

  char* prop_name = malloc(max_buffer);
  if (prop_name == NULL) {
    fprintf(stderr, "%s\n", "no memory to allocate");
    exit(-1);
  }
  char* prop_value = malloc(max_buffer);
  if (prop_value == NULL) {
    fprintf(stderr, "%s\n", "no memory to allocate");
    exit(-1);
  }
  char* filepath = malloc(max_buffer);
  if (filepath == NULL) {
    fprintf(stderr, "%s\n", "no memory to allocate");
    exit(-1);
  }
  // possible resulting flags are: 11000, 10110, 10001
  uint8_t flags[flags_size] = {SHOW, SET, GET};
  uint8_t flag_mask = 0;

  while (1) {
    static struct option long_options[] = {
        {"filepath", required_argument, 0, 'f'},
        {"show", no_argument, 0, 'p'},
        {"set", required_argument, 0, 's'},
        {"value", required_argument, 0, 'v'},
        {"get", required_argument, 0, 'g'},
        {0, 0, 0, 0}};
    // getopt_long stores the option index here
    int option_index = 0;

    int current_option =
        getopt_long(argc, argv, "", long_options, &option_index);

    // the end of the options detected
    if (current_option == -1) break;
    SetArgFlags(&flag_mask, current_option);
    SetPropertyFields(current_option, prop_name, prop_value, filepath);
  }

  // dealing with  remaining command line arguments (not options)
  if (optind < argc) {
    fprintf(stderr, "%s\n",
            "non-option command line\
arguments are not allowed");
    exit(-1);
  }

  // validating resulting flag mask
  char is_good_flag_mask = 0;
  for (int i = 0; i < flags_size; ++i) {
    if (flag_mask == flags[i]) {
      is_good_flag_mask = 1;
      break;
    }
  }
  if (!is_good_flag_mask) {
    fprintf(stderr, "%s\n", "wrong combination of flags");
    exit(-1);
  }

  Args* arguments = malloc(sizeof(Args));
  if (arguments == NULL) {
    fprintf(stderr, "%s\n", "no memory to allocate");
    exit(-1);
  }
  arguments->prop_name = prop_name;
  arguments->prop_value = prop_value;
  arguments->filepath = filepath;
  arguments->flag_mask = flag_mask;
  return arguments;
}

void FreeArgsMemory(Args* arguments_ptr) {
  free(arguments_ptr->prop_name);
  free(arguments_ptr->prop_value);
  free(arguments_ptr->filepath);
  free(arguments_ptr);
}
