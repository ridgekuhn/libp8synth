#include "./load_music.h"
#include "./cdata.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_music(FILE *file_ptr, CData *cdata) {
  char str[12] = "";
  int last_seek = 0;
  int pat_idx = 0;

  // Regex for section tag
  regex_t section_regex;
  regcomp(&section_regex, "^__[a-z]*__\0?$", 0);
  // Regex for pattern data
  regex_t pat_regex;
  regcomp(&pat_regex, "^[0-9]\\{2\\} [0-9a-f]\\{8\\}\0?$", 0);

  while (fgets(str, 12, file_ptr)) {
    // Hit new cart section
    if (regexec(&section_regex, str, 0, NULL, 0) == 0) {
      // Rewind to previous line
      fseek(file_ptr, last_seek, SEEK_SET);

      return;
      // Invalid pattern data
    } else if (regexec(&pat_regex, str, 0, NULL, 0) != 0) {
      last_seek = ftell(file_ptr);
      continue;
    }

    int mask = 0;
    char patterns[8];

    sscanf(str, "%d %s", &mask, patterns);

    cdata->pattern_loop_mask[pat_idx] = mask;

    for (int i = 0; i < 4; i += 1) {
      char str[3] = {patterns[i * 2], patterns[i * 2 + 1], 0};
      int sfx_id = strtol(str, NULL, 16);

      cdata->pattern[pat_idx][i] = sfx_id;
    }

    pat_idx += 1;
    last_seek = ftell(file_ptr);
  }
};
