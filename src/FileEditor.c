#include <Structures.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitTextFrames(TextFrame* frames, uint32_t frames_size) {
  for (int i = 0; i < frames_size; ++i) {
    frames[i].size = -1;
  }
}

uint32_t CalculateFrameHeaderSize(char* size, uint8_t size_size) {
  uint32_t result = size[size_size - 1];
  for (int32_t i = size_size - 2; i >= 0; --i) {
    int32_t j = size_size - i - 1;
    uint32_t cur = size[i];
    result += (cur << (7 * j));
  }
  return result;
}

char IsTextFrameHeader(FILE* file, FrameHeader header) {
  char is_text_frame_id = 1;
  for (int i = 0; i < 4; ++i) {
    char cur = header.id[i];
    if (!(('0' <= cur && cur <= '9') || ('A' <= cur && cur <= 'Z'))) {
      fseek(file, 1 - sizeof(FrameHeader), SEEK_CUR);
      is_text_frame_id = 0;
      break;
    }
  }
  if (!is_text_frame_id) {
    return 0;
  }

  char is_text_frame_header_flags = 1;
  for (int i = 0; i < 2; ++i) {
    if ((header.flags[i] << 3) != 0) {
      is_text_frame_header_flags = 0;
      break;
    }
  }
  if (!is_text_frame_header_flags) {
    return 0;
    ;
  }
  return 1;
}

void AddTextFrameToArray(FILE* file, TextFrame* frames, uint32_t frames_size,
                         uint32_t frames_count, FrameHeader header) {
  // dump
  // printf("ID: %s\n", header.id);
  // printf("Size: %d\n", frame_size);
  // printf("Flags: %X %X\n", header.flags[0], header.flags[1]);
  uint32_t frame_size = CalculateFrameHeaderSize(header.size, 4);

  uint8_t text_encoding;
  fread(&text_encoding, sizeof(char), 1, file);
  // if (memcmp(header.id, "TYER", 4)) {
  // 	text_encoding = 0;
  // }
  frames[frames_count].whence = ftell(file);

  uint32_t character_size = sizeof(uint8_t);
  uint32_t text_size = (frame_size - sizeof(text_encoding)) / character_size;
  frames[frames_count].size_type = 0;
  if (text_encoding == 1) {
    // it is 16bit Unicode encoded
    frames[frames_count].size_type = 1;
    character_size = sizeof(uint16_t);
    text_size = (frame_size - sizeof(text_encoding)) / sizeof(uint16_t);
    // printf("%d\n 2-byte fields", text_size);
    uint16_t endian_data;
    text_size--;
    fread(&endian_data, character_size, 1, file);
    frames[frames_count].whence = ftell(file);
    uint16_t* text = (uint16_t*)malloc(text_size * character_size);
    frames[frames_count].text16 = text;
    for (int i = 0; i < text_size; ++i) {
      fread(&text[i], character_size, 1, file);
    }
  } else {
    // it is ISO-8859-1 encoded
    char* text = (char*)malloc(text_size * character_size);
    frames[frames_count].text8 = text;
    for (int i = 0; i < text_size; ++i) {
      fread(&text[i], character_size, 1, file);
    }
  }

  memcpy(frames[frames_count].id, header.id, 4);
  if (frames[frames_count].size == -1) {
    frames[frames_count].size = text_size;
  }
}

void SetLastElement(TextFrame* frames, uint32_t frames_size,
                    uint32_t frames_count) {
  if (frames_count < frames_size) {
    memcpy(frames[frames_count].id, "\0\0\0\0", 4);
  }
  memcpy(frames[frames_size - 1].id, "\0\0\0\0", 4);
}

void ParseFile(FILE* file, TextFrame* frames, uint32_t frames_size) {
  fseek(file, 0, SEEK_SET);
  TagHeader tag;
  if (fread(&tag, sizeof(TagHeader), 1, file) != 1) {
    printf("Could not read the tag\n");
    exit(0);
  }

  uint32_t header_size = CalculateFrameHeaderSize(tag.size, 4);
  // dump tag header
  // printf("ID: %s\n", tag.id);
  // printf("Version: %X.%X\n", tag.version[0], tag.version[1]);
  // printf("Flags: %X\n", tag.flags);
  // printf("Size: %d bytes\n", header_size);

  uint32_t count = 0;
  uint32_t frames_count = 0;
  char cur_ch;
  while (count < header_size && fread(&cur_ch, sizeof(char), 1, file) == 1) {
    count++;
    if (cur_ch != 'T') {
      continue;
    }

    // checking if it is a text frame header
    FrameHeader header;
    fseek(file, -1, SEEK_CUR);
    if (fread(&header, sizeof(FrameHeader), 1, file) != 1) {
      // it is not
      break;
    }

    if (!IsTextFrameHeader(file, header)) {
      continue;
    }

    // it is a text frame header
    if (frames_count > frames_size) {
      fprintf(stderr, "%s\n", "frame_size too small");
      exit(-1);
    }

    AddTextFrameToArray(file, frames, frames_size, frames_count, header);
    frames_count++;
  }

  SetLastElement(frames, frames_size, frames_count);
}

void PrintPropertyValue(TextFrame* frames, uint32_t index) {
  printf("Property Value: ");
  if (frames[index].size_type == 1) {
    for (int j = 0; j < frames[index].size && frames[index].text16[j] != 0;
         ++j) {
      // we believe that it is Latin-1 - so we ignore the 2nd byte
      printf("%c", (char)frames[index].text16[j]);
    }
    printf("\n");
  } else {
    printf("%s\n", frames[index].text8);
  }
}

int32_t GetIndex(TextFrame* frames, const char* prop_name) {
  uint32_t prop_name_size = 4;
  for (int i = 0; frames[i].id[0] != '\0'; ++i) {
    if (memcmp(frames[i].id, prop_name, prop_name_size) == 0) {
      return i;
    }
  }
  return -1;
}

void UpdateProperty(FILE* file, TextFrame* frames, int32_t index,
                    const char* prop_name, char* prop_value,
                    int32_t length_prop_value) {
  if (frames[index].size_type == 0) {
    // Unicode
    int32_t text_size = frames[index].size;
    for (int i = 0; i < text_size; ++i) {
      frames[index].text8[i] = '\0';
    }
    memcpy(frames[index].text8, prop_value, text_size);
    if (fwrite(frames[index].text8, sizeof(uint8_t), text_size, file) !=
        text_size) {
      fprintf(stderr, "%s\n", "error while writing to file");
      exit(-1);
    }
  } else {
    // ISO-8859-1
    uint16_t prop_value_extended[max_buffer];
    for (int i = 0; i < length_prop_value; ++i) {
      prop_value_extended[i] = 0;
      prop_value_extended[i] = prop_value[i];
    }
    int32_t text_size = frames[index].size;
    for (int i = length_prop_value; i < text_size; ++i) {
      prop_value_extended[i] = 0;
    }
    memcpy(frames[index].text16, prop_value_extended,
           text_size * sizeof(uint16_t));
    if (fwrite(frames[index].text16, sizeof(uint16_t), text_size, file) !=
        text_size) {
      fprintf(stderr, "%s\n", "error while writing to file");
      exit(-1);
    }
  }
}
