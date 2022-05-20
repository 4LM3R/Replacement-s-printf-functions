#ifndef UART_DEBUG_H
#define	UATR_DEBUG_H

#include <stdint.h>


#define UART_DEBUG_PCLK         50000000    // Peripheral clock
#define UART_DEBUG_DATA_RATE    115200
#define UART_DEBUG_BUFFER_SIZE  200

#define DEBUG_NUMBER_LINES      4
#define DEBUG_TEXT_LENGTH       32
#define DEBUG_VALUE_LENGTH      10
#define DEBUG_LINE_LENGTH       (DEBUG_TEXT_LENGTH + DEBUG_VALUE_LENGTH + 2)  //add 2 for the \n\r characters

// Uncomment to enable waiting for all debug data to be send
//#define UART_DEBUG_WAIT_TILL_SEND
// Uncomment to enable timed debug messages
#define UART_DEBUG_TIMED_MESSAGES


/**
 *     <b>Function prototype:</b><br>   void debug_string(char *str)
 * <br>
 * <br><b>Description:</b><br>          Prints a null terminated string to the uart port
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               char *str:  Pointer to a null terminated string
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              debug_string("Hello universe!");  // Print to debug uart
 */
void debug_string(char *str);

/**
 * Function prototype:  void debug_char(char value)
 * Description:         Prints an char to the uart port
 */
void debug_char(char value);

/**
 *     <b>Function prototype:</b><br>   void debug_uint(UINT32 value)
 * <br>
 * <br><b>Description:</b><br>          Prints an unsigned integer to the uart port
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               UINT32 value:  Value to print
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              debug_uint(313);  // Print to debug uart
 */
void debug_uint(uint32_t value);

void debug_int_len(int32_t value, uint8_t len);

/**
 *     <b>Function prototype:</b><br>   void debug_int(INT32 value)
 * <br>
 * <br><b>Description:</b><br>          Prints an integer to the uart port
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               INT32 value:  Value to print
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              debug_uint(-313);  // Print to debug uart
 */
void debug_int(int32_t value);

/**
 *     <b>Function prototype:</b><br>   void debug_hex(UINT32 value)
 * <br>
 * <br><b>Description:</b><br>          Prints an unsigned integer as a hex string to the uart port
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               UINT32 value:  Value to print
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              debug_hex(0xF313F);  // Print to debug uart
 */
void debug_hex(uint32_t value);

/**
 *     <b>Function prototype:</b><br>   void debug_uart_init(void)
 * <br>
 * <br><b>Description:</b><br>          Configures the UART2 peripheral for debug output
 * <br>
 * <br><b>Precondition:</b><br>         None
 * <br>
 * <br><b>Inputs:</b><br>               None
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              debug_uart_init();  //Initialize Debug
 */
void debug_uart_init(void);

/**
 *     <b>Function prototype:</b><br>   void debug_process(void)
 * <br>
 * <br><b>Description:</b><br>          Prints predefined debug data to the uart every second.
 * <br>                                 This function should be called in every loop of the main.
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               None
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              void debug_process(void);   // Debug data
 */
void debug_process(void);

int8_t uart_debug_ready(void);


#endif	// _DEBUG_H
