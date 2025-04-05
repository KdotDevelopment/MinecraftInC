#include <util/string.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

char *string_create(const char *string) {
    char *new_string = malloc(strlen(string) + 1);
    memcpy(new_string, string, strlen(string) + 1);

    return new_string;
}

char *string_create_from_int(int number) {
    int len = number == 0 ? 1 : ceil(log10(abs(number) + 1.0));
    char *string = string_create("");
    for(int i = 0; i < len; i++) {
        int digit = (int)((float)abs(number) / pow(10.0, i)) % 10;
        string_concat_front((char[]){ digit + 48, '\0' }, &string);
    }
    if(number < 0) {
        string_concat_front("-", &string);
    }
    return string;
}

int32_t string_length(char *string) {
    return (int32_t)strlen(string);
}

void string_concat(char **string0, const char *string1) {
    string_insert(string0, string_length(*string0), string1);
}

void string_concat_front(const char *string0, char **string1) {
    string_insert(string1, 0, string0);
}

char* string_sub(char *string, int32_t index_start, int32_t index_end) {
    if(index_start < 0) { index_start = 0; }
    if(index_end > string_length(string) - 1) index_end = string_length(string) - 1;
    if(index_start > index_end) return string_create("");
    char orig = string[index_end + 1];
    string[index_end + 1] = '\0';
    char *newString = string_create(string + index_start);
    string[index_end + 1] = orig;
    return newString;
}

int32_t string_index_of(char *string, const char chr) {
    for(int32_t i = 0; i < string_length(string); i++) {
        if(string[i] == chr) return i;
    }
    return -1;
}

void string_set(char **string0, const char *string1) {
    string_free(*string0);
    *string0 = string_create(string1);
}

void string_set_from_int(char **string, int number) {
    string_free(*string);
    *string = string_create_from_int(number);
}

void string_insert(char **string0, int32_t index, const char *string1) {
    int32_t len1 = string_length(*string0), len2 = (int32_t)strlen(string1);
    if(index > len1) index = len1;
    *string0 = realloc(*string0, len1 + len2 + 1);
    for(int32_t i = len1; i >= index; i--) {
        (*string0)[i + len2] = (*string0)[i];
    }
    memcpy(*string0 + index, string1, len2);
}

void string_remove(char **string, int32_t index_start, int32_t index_end) {
    if(index_start < 0) { index_start = 0; }
    if(index_end > string_length(*string) - 1) index_end = string_length(*string) - 1;
    if(index_start > index_end) { string_set(string, ""); }
    for(int i = 0; i < string_length(*string) - index_end; i++) {
        (*string)[index_start + i] = (*string)[index_end + i + 1];
    }
    *string = realloc(*string, string_length(*string) - (index_end - index_start + 1));
}

uint8_t string_equals(char *string0, const char *string1) {
    return strcmp(string0, string1) == 0;
}

int string_to_int(char *string) {
    int n = 0;
    for(int i = string_length(string) - 1, j = 0; i >= 0; i--, j++) {
        if(string[i] == '-' && i == 0) {
            n *= -1;
            break;
        }
        n += (string[i] - 48) * pow(10, j);
    }
    return n;
}

void string_free(char *string) {
    free(string);
}