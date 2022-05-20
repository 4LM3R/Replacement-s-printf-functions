#ifndef UTL_H
#define UTL_H

#include <stdint.h>

char *utl_itoa(int value, char *str, uint8_t radix);
char *utl_uitoa(unsigned int value, char *str, uint8_t radix);
char *utl_ltoa(long value, char *str, uint8_t radix);
char *utl_ultoa(unsigned long value, char *str, uint8_t radix);
char *utl_lltoa(long value, char *str, uint8_t radix);

char *utl_i32toa(int32_t value, char *str, uint8_t radix);
char *utl_ui32toa(uint32_t value, char *str, uint8_t radix);

char *utl_itoa_l(int value, char *str, uint8_t radix, uint8_t len);
char *utl_i32toa_l(uint32_t value, char *str, uint8_t radix, uint8_t len);

int32_t utl_atoi32(char *str, uint8_t radix);
uint32_t utl_atoui32(char *str, uint8_t radix);
int utl_hstoi(char *s);

uint16_t utl_calc_crc(uint8_t *pdata, uint32_t ui_size);


#endif
