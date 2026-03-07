// NOTICE: This file is not accurate and only partially implemented

#include "./cdata.h"
#include <stdint.h>
#include <stdlib.h>

CData *codo_create_pico8_cart() {
  long *data = calloc(0x10002, sizeof(int32_t));
  CData *cdata = (CData *)data;

  return cdata;
}
