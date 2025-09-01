#pragma once

#include <nbt/nbt_base.h>

#include <stdint.h>

typedef struct {
    struct minecraft_s *minecraft;
    char *text;
    char *title;
    int64_t start;
} progress_bar_t;

progress_bar_t progress_bar_create(struct minecraft_s *minecraft);
void progress_bar_set_title(progress_bar_t *bar, char *title);
void progress_bar_set_text(progress_bar_t *bar, char *text);
void progress_bar_set_progress(progress_bar_t *bar, int progress);
nbt_base_t progress_bar_read(FILE *chunk_file);
void progress_bar_write(FILE *chunk_file, nbt_base_t *nbt);