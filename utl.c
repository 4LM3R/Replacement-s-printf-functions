#include "utl.h"
#include <stdint.h>

static const char hex_chars[] = "0123456789ABCDEF";


/*
 * Function:        char *utl_itoa(int value, char *str, uint8_t radix)
 * 
 * Description:     Converts an integer to a null terminated string
 *                  Returns max 16 chars
 * 
 * Parameters:      int value          The value to convert
 *                  char *str          Pointer to a string buffer
 *                  uint8_t radix      The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *             Pointer to the string buffer
 */
char *utl_itoa(int value, char *str, uint8_t radix)
{
  char temp[16];
  uint16_t rest,windex,n;
  char *ptr;
  uint16_t w,rd;

  ptr=str;                        // save string ptr   

  if (radix < 2 || radix > 16)    // wrong radix       
    return (ptr);

  if (value < 0 && radix == 10)   // negative number ? 
  {                               // and radix 10      
    value = -value;
    *str++ = '-';
  }
  w = value;
  rd = radix;
  windex = 0;                     // Do conversion     
  do
  {
    n = (uint16_t) ((uint16_t)(value) / radix);
    rest = (uint16_t) ((uint16_t)(value) % radix);      // rest is char LSB first 
//    rest = value - (radix*(value/radix));
    temp[windex++] = hex_chars[rest];
    value = n;
  } while (value != 0);

  windex--;
  for (n = windex ; n != 65535 ; n--)  // swap LSB MSB  
    *str++ = temp[n];
  *str = '\0';
  return (ptr);
}

/*
 * Function:        char *utl_uitoa(unsigned int value, char *str, uint8_t radix)
 * 
 * Description:     Converts an unsigned integer to a null terminated string
 *                  Returns max 16 chars
 * 
 * Parameters:      unsigned int value The value to convert
 *                  char *str          Pointer to a string buffer
 *                  uint8_t radix      The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *             Pointer to the string buffer
 */
char *utl_uitoa(unsigned int value, char *str, uint8_t radix)
{
  char temp[16];
  uint16_t rest,windex,n;
  char *ptr;
  uint16_t w,rd;

  ptr=str;                        // save string ptr

  if (radix < 2 || radix > 16)    // wrong radix
    return (ptr);

//  if (value < 0 && radix == 10)   // negative number ?
//  {                               // and radix 10
//    value = -value;
//    *str++ = '-';
//  }
  w = value;
  rd = radix;
  windex = 0;                     // Do conversion
  do
  {
    n = (uint16_t) ((uint16_t)(value) / radix);
    rest = (uint16_t) ((uint16_t)(value) % radix);      // rest is char LSB first
//    rest = value - (radix*(value/radix));
    temp[windex++] = hex_chars[rest];
    value = n;
  } while (value != 0);

  windex--;
  for (n = windex ; n != 65535 ; n--)  // swap LSB MSB
    *str++ = temp[n];
  *str = '\0';
  return (ptr);
}

/*
 * Function:        char *utl_ultoa(unsigned long value, char *str, uint8_t radix)
 * 
 * Description:     Converts an unsigned long to a null terminated string
 *                  Returns max 24 chars
 * 
 * Parameters:      unsigned long value The value to convert
 *                  char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *              Pointer to the string buffer
 */
char *utl_ultoa(unsigned long value, char *string, uint8_t radix)
{
  char temp[33];
  uint8_t windex, rest;
  unsigned long lnew;
  char *ptr;

  ptr=string;                     /* save string ptr   */

  if (radix < 2 || radix > 16)    /* wrong radix       */
    return (ptr);

  windex = 0;                      /* Do conversion     */
  do
  {
    lnew = value / radix;
    rest =  value % radix;   /* rest is char LSB first */
//    rest = value - (radix*(value/radix));
    temp[windex++] = hex_chars[rest];
    value = lnew;
  } while (value != 0);
  windex--;
  for (lnew = windex ; lnew != 0xFFFFFFFF ; --lnew)   /* swap LSB MSB     */
    *string++ = temp[lnew];
  *string = '\0';
  return (ptr);
}

/*
 * Function:        char *utl_lltoa(long value, char *str, uint8_t radix)
 * 
 * Description:     Converts a long long integer to a null terminated string
 *                  Returns max 32 chars
 * 
 * Parameters:      long long value    The value to convert
 *                  char *str          Pointer to a string buffer
 *                  uint8_t radix      The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *             Pointer to the string buffer
 */
char *utl_lltoa(long val, char *s, uint8_t radix)
{ 
  char *p,t;
  uint8_t ch;
  uint16_t l,rd;

  if (val < 0 && radix == 10)     /* negative number ? */
  {                               /* and radix 10      */
    val = -val;
    *s++ = '-';
  }
  l = val;
  rd = radix;
  p=s;
  do
  {
//    ch= (BYTE) ((val % radix) & 0x0f);
    ch= (uint8_t) ((l % rd) & 0x0f);
    if (ch>=10)
      *p= (char)(ch)+55; /* -10+'A';*/
    else
      *p= (char)(ch)+'0';
    p++;
//    val = val / radix;
    l = l / rd;
//  } while (val);
  } while (l);
  *p='\0';
  p--; ch=0;
  while (p>(s+ch))  /* the converted string is turned around! */
  {
    t= *p;
    *p= s[ch];
    s[ch]=t;
    ch++; p--;
  }
  return(s);
}


/*
 * Function:        char *utl_i32toa(int32_t value, char *str, uint8_t radix)
 * 
 * Description:     Converts an 32 bit integer to a null terminated string
 *                  Returns max 24 chars
 * 
 * Parameters:      int32_t value      The value to convert
 *                  char *str          Pointer to a string buffer
 *                  uint8_t radix      The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *             Pointer to the string buffer
 */
char *utl_i32toa(int32_t value, char *str, uint8_t radix) {
    char temp[24];
    uint32_t rest, index;
    char *ptr;
    int8_t i;

    ptr=str;                                // Save string ptr
    if (radix < 2 || radix > 16) {          // Wrong radix
        return ptr;
    }
    if (value < 0) {                        // Negative number 
        value = -value;
        *str++ = '-';
    }

    index = 0;                              // Do conversion
    do {
        rest = value % radix;               // Rest is char LSB first
        temp[index++] = hex_chars[rest];
        value = value / radix;
    } while (value != 0 && index < 24);

    index--;
    for (i = index ; i >= 0 ; i--) {        // Swap LSB MSB
        *str++ = temp[i];
    }
    *str = '\0';
    return ptr;
}

/*
 * Function:        char *utl_ui32toa(uint32_t value, char *str, uint8_t radix)
 * 
 * Description:     Converts an 32 bit unsigned integer to a null terminated string
 *                  Returns max 24 chars
 * 
 * Parameters:      uint32_t value      The value to convert
 *                  char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *              Pointer to the string buffer
 */
char *utl_ui32toa(uint32_t value, char *str, uint8_t radix) {
    char temp[24];
    uint32_t rest, index;
    char *ptr;
    int8_t i;

    ptr=str;                                // Save string ptr
    if (radix < 2 || radix > 16) {          // Wrong radix
        return ptr;
    }
    index = 0;                              // Do conversion
    do {
        rest = value % radix;               // Rest is char LSB first
        temp[index++] = hex_chars[rest];
        value = value / radix;
    } while (value != 0 && index < 24);

    index--;
    for (i = index ; i >= 0 ; i--) {        // Swap LSB MSB
        *str++ = temp[i];
    }
    *str = '\0';
    return ptr;
}

/*
 * Function:        char *utl_itoa(int value, char *str, uint8_t radix, uint8_t len)
 * 
 * Description:     Converts an integer to a null terminated string 
 *                  padded with 0 to a specified length
 *                  Returns max 24 chars
 * 
 * Parameters:      int value           The value to convert
 *                  char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *              Pointer to the string buffer
 */
char *utl_itoa_l(int value, char *str, uint8_t radix, uint8_t len)
{
  char temp[16];
  uint16_t rest,index,n;
  char *ptr;
  uint16_t w,rd;

  ptr=str;                     /* save string ptr   */
  
  if (len>8)
    return (ptr);                 // wrong length

  if (radix < 2 || radix > 16)    /* wrong radix       */
    return (ptr);

  if (value < 0 && radix == 10)   /* negative number ? */
  {                               /* and radix 10      */
    value = -value;
    *str++ = '-';
  }
  w = value;
  rd = radix;
  index = 0;                      /* Do conversion     */
  do
  {
    n = (uint16_t) ((uint16_t)(value) / radix);
//    rest = (WORD) ((WORD)(value) % radix);      /* rest is char LSB first */
    rest = value - (radix*(value/radix));
    temp[index++] = hex_chars[rest];
    value = n;
  } while (value != 0);
  while(index<len)
    temp[index++] = '0';

  index--;
  for (n = index ; n != 65535 ; n--)  /* swap LSB MSB     */
    *str++ = temp[n];
  *str = '\0';
  return (ptr);
}

/*
 * Function:        char *utl_i32toa(int32_t value, char *str, uint8_t radix, uint8_t len)
 * 
 * Description:     Converts an 32 bit integer to a null terminated string 
 *                  padded with 0 to a specified length
 *                  Returns max 24 chars
 * 
 * Parameters:      int32_t value       The value to convert
 *                  char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         char *              Pointer to the string buffer
 */
char *utl_i32toa_l(uint32_t value, char *str, uint8_t radix, uint8_t len) {
    char temp[24];
    uint32_t rest, index;
    char *ptr;
    int8_t i;

    ptr=str;                                // Save string ptr
    if (radix < 2 || radix > 16) {          // Wrong radix
        return ptr;
    }
    index = 0;                              // Do conversion
    do {
        rest = value % radix;               // Rest is char LSB first
        temp[index++] = hex_chars[rest];
        value = value / radix;
    } while (value != 0 && index < 24);
    
    while (index<len) {
        temp[index++] = '0';
    }
    index--;
    for (i = index ; i >= 0 ; i--) {        // Swap LSB MSB
        *str++ = temp[i];
    }
    *str = '\0';
    return ptr;
}

/*
 * Function:        int32_t utl_atoi32(char *str, uint8_t radix)
 * 
 * Description:     Converts a string to a 32 bit integer
 * 
 * Parameters:      char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         int32_t            Result of the conversion
 */
int32_t utl_atoi32(char *str, uint8_t radix) {
    char temp;
    int32_t value = 0;
    uint8_t negative = 0;
    
    if (radix < 2 || radix > 10) {          // Wrong radix
        return 0;
    }
    if (*str == '-') {
        negative = 1;
        str++;
    }
    
    while (*str != '\0') {
        temp = *str;
        // Convert char to int
        if ('0'<=temp && temp<='9') temp -= '0';
        else temp = 0;
        if (temp > radix) temp = 0;
        // Add to value
        value *= radix;
        value += temp;
        str++;
    }
    if (negative) value *= -1;
    return value;
}

/*
 * Function:        uint32_t utl_atoui32(char *str, uint8_t radix)
 * 
 * Description:     Converts a string to a 32 bit unsigned integer
 * 
 * Parameters:      char *str           Pointer to a string buffer
 *                  uint8_t radix       The radix to use for the conversion (10->dec, 16->hex)
 *
 * Returns:         uint32_t            Result of the conversion
 */
uint32_t utl_atoui32(char *str, uint8_t radix) {
    char temp;
    uint32_t value = 0;
    
    if (radix < 2 || radix > 16) {          // Wrong radix
        return 0;
    }
    while (*str != '\0') {
        temp = *str;
        // Convert char to int
        if      ('0'<=temp && temp<='9')    temp -= '0';
        else if ('a'<=temp && temp<='f')    temp = temp - 'a' + 10;
        else if ('A'<=temp && temp<='F')    temp = temp - 'A' + 10;
        else                                temp = 0;
        
        if (temp > radix) temp = 0;
        // Add to value
        value *= radix;
        value += temp;
        str++;
    }
    return value;
}

uint8_t utl_hatoi(char c)
{
  return (c > '9' ? (c&0x4f)-'A'+10 : c-'0');
}

int utl_hstoi(char *s)
{ int val;

  if (!*s)
    return 0;

  val=utl_hatoi(*s++);
  while (*s)
    val= val*16 + utl_hatoi(*s++);

  return(val);
} 

#define CRC_POLY_CRC16_CCITT 0x1021 // X^16 + X^12 + X^5 + 1

/**
 *     <b>Function prototype:</b><br>   static void hash_crc_16ccitt(WORD *crc,BYTE data)
 * <br>
 * <br><b>Description:</b><br>          Calculate CRC 16 CCITT
 * <br>
 * <br><b>Precondition:</b><br>         None
 * <br>
 * <br><b>Inputs:</b><br>               WORD *crc:  The crc to add this byte to
 * <br>                                 BYTE data:  byte to add on CRC
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              hash_crc_16ccitt(&wCrc,*pdata);
 */
static void hash_crc_16ccitt(uint16_t *crc,uint8_t data) {
    uint8_t cnt;

    *crc = ((uint16_t)data << 8) ^ *crc;
    for (cnt = 0; cnt < 8; cnt++) {
        if (*crc & 0x8000) {
            *crc = (*crc << 1) ^ CRC_POLY_CRC16_CCITT;
        } else {
            *crc <<= 1;
        }
    }
}

/**
 *     <b>Function prototype:</b><br>   UINT16 utl_calc_crc(UINT8 *pdata, UINT32 ui_size)
 * <br>
 * <br><b>Description:</b><br>          Calculates the CRC 16 CCITT of a byte array buffer.
 * <br>
 * <br><b>Precondition:</b><br>         None
 * <br>
 * <br><b>Inputs:</b><br>               UINT8 *pdata:   Pointer to a byte buffer.
 * <br>                                 UINT32 ui_size: Size of the array
 * <br>
 * <br><b>Outputs:</b><br>              The crc
 * <br>
 * <br><b>Example:</b><br>              crc = utl_calc_crc(eeprom_data.raw, EEPROM_DATA_SIZE-2);    //Get crc
 */
uint16_t utl_calc_crc(uint8_t *pdata, uint32_t ui_size) {
   int n=0;
   uint16_t wCrc = 0x1D0F;

   for (n=0; n<ui_size ; n++) {
      hash_crc_16ccitt(&wCrc,*pdata);
      pdata++;
   }
   return wCrc;
}
