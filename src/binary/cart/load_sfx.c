#include "./load_sfx.h"
#include "./cdata.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_sfx(FILE *file_ptr, CData *cdata) {
  char str[169] = "";
  int last_seek = 0;
  int sfx_idx = 0;

  // Regex for section tag
  regex_t section_regex;
  regcomp(&section_regex, "^__[a-z]*__\0?$", 0);
  // Regex for pattern data
  regex_t pat_regex;
  regcomp(&pat_regex, "^[0-9a-f]\\{168\\}\0?$", 0);

  while (fgets(str, 169, file_ptr)) {
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

    // Filter byte
    const char filter_str[3] = {str[0], str[1], 0};
    const int filter_byte = strtol(filter_str, NULL, 16);

    cdata->sfx[sfx_idx].filter_byte = filter_byte;

    // Editor byte
    cdata->sfx[sfx_idx].editor_byte = filter_byte & 1;

    // spd
    const char spd_str[3] = {str[2], str[3], 0};

    cdata->sfx[sfx_idx].spd = strtol(spd_str, NULL, 16);

    // loop start
    const char loop_start_str[3] = {str[4], str[5], 0};

    cdata->sfx[sfx_idx].loop_start = strtol(loop_start_str, NULL, 16);

    // loop end
    const char loop_end_str[3] = {str[6], str[7], 0};

    cdata->sfx[sfx_idx].loop_end = strtol(loop_end_str, NULL, 16);

    for (int i = 0; i < 32; i += 1) {
      const int offset = i * 5 + 8;

      // pitch
      const char pitch_str[3] = {str[offset], str[offset + 1], 0};
      cdata->sfx[sfx_idx].step[i].pitch = strtol(pitch_str, NULL, 16);

      // oscillator
      const char osc_str[2] = {str[offset + 2], 0};
      const int osc_byte = strtol(osc_str, NULL, 16);
      cdata->sfx[sfx_idx].step[i].oscillator = osc_byte & 7;

      // vol
      const char vol_str[2] = {str[offset + 3], 0};
      cdata->sfx[sfx_idx].step[i].vol = strtol(vol_str, NULL, 16);

      // effect
      const char effect_str[2] = {str[offset + 4], 0};
      cdata->sfx[sfx_idx].step[i].effect = strtol(effect_str, NULL, 16);

      // use meta instrument
      cdata->sfx[sfx_idx].step[i].use_meta = osc_byte & 8 ? 0x80 : 0;
    }

    sfx_idx += 1;
    last_seek = ftell(file_ptr);
  }
};
