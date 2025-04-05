#pragma once

#include <stdint.h>

char *string_create(const char *string);
char *string_create_from_int(int number);
int32_t string_length(char *string);
void string_concat(char **string0, const char *string1);
void string_concat_front(const char *string0, char **string1);
char* string_sub(char *string, int32_t index_start, int32_t index_end);
int32_t string_index_of(char *string, const char chr);
void string_set(char **string0, const char *string1);
void string_set_from_int(char **string, int number);
void string_insert(char **string0, int32_t index, const char *string1);
void string_remove(char **string, int32_t index_start, int32_t index_end);
uint8_t string_equals(char *string0, const char *string1);
int string_to_int(char *string);
void string_free(char *string);