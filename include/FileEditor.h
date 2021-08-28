#ifndef FRAME_EDITOR_H
#define FRAME_EDITOR_H

#include <Structures.h>
#include <stdint.h>
#include <stdio.h>

void InitTextFrames(TextFrame* frames, uint32_t frames_size);

uint32_t CalculateFrameHeaderSize(char* size, uint8_t size_size);

char IsTextFrameHeader(FILE* file, FrameHeader header);

void AddTextFrameToArray(FILE* file, TextFrame* frames, uint32_t frames_size,
                         uint32_t frames_count, FrameHeader header);

void SetLastElement(TextFrame* frames, uint32_t frames_size,
                    uint32_t frames_count);

void ParseFile(FILE* file, TextFrame* frames, uint32_t frames_size);

void PrintPropertyValue(TextFrame* frames, uint32_t index);

int32_t GetIndex(TextFrame* frames, const char* prop_name);

void UpdateProperty(FILE* file, TextFrame* frames, int32_t index,
                    const char* prop_name, char* prop_value,
                    int32_t length_prop_value);

#endif
