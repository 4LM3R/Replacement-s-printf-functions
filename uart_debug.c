#include "uart_debug.h"
#include <xc.h>
#include <stdint.h>
#include <string.h>
#include "utl.h"
#include "softwaretimer.h"
#include "enginecontrol.h"
#include "sensor.h"
#include "alarms.h"
#include "userinterface.h"
#include "sensor.h"
#include "generatormeasure.h"
#include "rtcc.h"
#include "userio.h"
#include "adc1.h"

#include "sensorpiccom.h"


static struct{
    char data[UART_DEBUG_BUFFER_SIZE];
    uint8_t in;
    uint8_t out;
} debug_buffer = {.in = 0, .out = 0};
static uint8_t debug_timer = SOFTWARE_TIMER_NO_TIMER;

/**
 *     <b>Function prototype:</b><br>   _U2TXInterrupt(void)
 * <br>
 * <br><b>Description:</b><br>          UART 2 interrupt routine.
 * <br>                                 Fills the uarts buffer from the circular buffer
 * <br>
 * <br><b>Precondition:</b><br>         Uart debugging must be initialized
 * <br>
 * <br><b>Inputs:</b><br>               None
 * <br>
 * <br><b>Outputs:</b><br>              None
 * <br>
 * <br><b>Example:</b><br>              
 */
void __attribute__((interrupt(auto_psv))) _U2TXInterrupt(void){
	// Fill the buffer till full or no more character are available
	while (!U2STAbits.UTXBF && (debug_buffer.in != debug_buffer.out)) {
		// Write character to transmit buffer
		U2TXREG = debug_buffer.data[debug_buffer.out++];
        if (debug_buffer.out == UART_DEBUG_BUFFER_SIZE) debug_buffer.out = 0;
	}
	
	// Clear interrupt flag
	_U2TXIF = 0;
}

/**
 * Function prototype:  void debug_string(char *str)
 * Description:         Prints a null terminated string to the uart port
 */
void debug_string(char *str){
    uint8_t temp_in;
    
    // Fill the buffer
    while (*str != '\0') {
        temp_in = debug_buffer.in + 1;
        if (temp_in == UART_DEBUG_BUFFER_SIZE) temp_in -= UART_DEBUG_BUFFER_SIZE;
#ifdef UART_DEBUG_WAIT_TILL_SEND
        // wait till room is available
        while (temp_in == debug_buffer.out) {
            Nop();
        }
#else
        if (temp_in == debug_buffer.out) {
            break;              // No more room is available in the buffer
        }
#endif
        debug_buffer.data[debug_buffer.in] = *str;
        debug_buffer.in++;
        str++;
        if (debug_buffer.in == UART_DEBUG_BUFFER_SIZE) debug_buffer.in = 0;
    }
    
    
    _U2TXIE = 0;                  // disable interrupt
    // Fill the buffer till full or no more character are available
    // Needs to be done to trigger the start of the interrupts
	while (!U2STAbits.UTXBF && (debug_buffer.in != debug_buffer.out)) {
		// Write character to transmit buffer
		U2TXREG = debug_buffer.data[debug_buffer.out++];
        if (debug_buffer.out == UART_DEBUG_BUFFER_SIZE) debug_buffer.out = 0;
	}
    _U2TXIE = 1;                  // enable interrupt
}

/**
 * Function prototype:  void debug_char(char value)
 * Description:         Prints an char to the uart port
 */
void debug_char(char value){
    char temp_str[2] = " \0";
    // Convert the value to a string
    temp_str[0] = value;
    // Put the string in the buffer
    debug_string(temp_str);
}

/**
 * Function prototype:  void debug_uint(uint32_ value)
 * Description:         Prints an unsigned integer to the uart port
 */
void debug_uint(uint32_t value){
    char temp_str[16];
    // Convert the value to a string
    utl_ui32toa(value, temp_str, 10);
    // Put the string in the buffer
    debug_string(temp_str);
}

/**
 * Function prototype:  void debug_int(int32_t value)
 * Description:         Prints an integer to the uart port
 */
void debug_int(int32_t value){
    char temp_str[16];
    // Convert the value to a string
    utl_i32toa(value, temp_str, 10);
    // Put the string in the buffer
    debug_string(temp_str);
}

void debug_int_len(int32_t value, uint8_t len){
    char temp_str[16];
    // Convert the value to a string
    utl_i32toa_l(value, temp_str, 10, len);
    // Put the string in the buffer
    debug_string(temp_str);
}

/**
 * Function prototype:  void debug_hex(uint32_t value)
 * Description:         Prints an unsigned integer as a hex string to the uart port
 */
void debug_hex(uint32_t value){
    char temp_str[16];
    // Convert the value to a string
    utl_ui32toa(value, temp_str, 16);
    // Put the string in the buffer
    debug_string(temp_str);
}

/**
 * Function prototype:  void debug_uart_init(void)
 * Description:         Configures the UART2 peripheral for debug output
 */
void debug_uart_init(void){
    uint32_t brg_value;
    
	//setup pin mapping
	//uart tx - pin 48 - RP79,RD15
	//uart rx - pin 47 - RP78,RD14
	_RP120R = _RPOUT_U2TX;
	_U2RXR = 119;
    
    //Calculate brg value from the desired data rate and the peripheral clock
    brg_value = (UART_DEBUG_PCLK>>1) / UART_DEBUG_DATA_RATE;
    brg_value++;
    brg_value >>= 1;
    brg_value--;

    U2BRG = brg_value & 0x0000FFFF; //Set baudrate
    U2MODEbits.UEN = 0b00;          //UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/BCLK pins are controlled by port latches
    U2MODEbits.BRGH = 0b1;          //BRG generates 4 clocks per bit period (4x baud clock, High-Speed mode)
    U2MODEbits.URXINV = 1;          //UxRX Idle state is ?0?
    U2MODEbits.PDSEL = 0b00;        //8-bit data, no parity
    U2MODEbits.STSEL = 0b0;         //One Stop bit
    
    //Setup interrupts
    U2STAbits.UTXISEL1 = 0b1;       //Interrupt when a character is transferred to the Transmit Shift Register (TSR) and as a result, the transmit buffer becomes empty
    U2STAbits.UTXISEL0 = 0b0;
    _U2TXIP = 3;                    //Interrupt priority of 3. (0 is lowest and 7 is highest)
    _U2TXIF = 0;                    //Clear UART TX interrupt flags
    _U2TXIE = 1;                    //Enable UART TX interrupt
    
    U2MODEbits.UARTEN  = 1;         //UARTx is enabled; all UARTx pins are controlled by UARTx as defined by UEN<1:0>
    U2STAbits.UTXEN = 1;            //Transmit is enabled, UxTX pin is controlled by UARTx
    
    //Init one second timer
    debug_timer = software_timer_create(SOFTWARE_TIMER_MODE_CONTINUOUS, 1000);
    software_timer_start(debug_timer);
    //debug_timer = SOFTWARE_TIMER_NO_TIMER;
}

/**
 * Function prototype:  void debug_process(void)
 * Description:         Prints predefined debug data to the uart every second.
 *                      This function should be called in every loop of the main.
 */
void debug_process(void){
    static uint8_t line = 255;
    uint8_t available_size;
    //rtcc_timestamp_t rtcc_timestamp;
    
#ifdef UART_DEBUG_TIMED_MESSAGES
    if (get_software_timer_is_expired(debug_timer) == SOFTWARE_TIMER_TRUE) {
        // Start writing debug data
        line = 0;
    }
#endif
    
    // Get empty size in buffer
    if (debug_buffer.in > debug_buffer.out) {
        available_size = debug_buffer.in - debug_buffer.out;
    } else {
        available_size = (debug_buffer.in + UART_DEBUG_BUFFER_SIZE) - debug_buffer.out;
    }
    // Only write new debug lines if half of the buffer is empty
    // This way there is always room for instant debug messages
    if (available_size > (UART_DEBUG_BUFFER_SIZE / 2)) {
        switch (line) {
            case 0:
                if (get_ecu_state() == OFF) debug_string("OFF");
                if (get_ecu_state() == IDLE) debug_string("IDLE");
                if (get_ecu_state() == PUMPING) debug_string("PUMP");
                if (get_ecu_state() == GLOWING) debug_string("GLOW");
                if (get_ecu_state() == CRANKING) debug_string("START");
                if (get_ecu_state() == START_DELAY) debug_string("START_DELAY");
                if (get_ecu_state() == SAFETY_ON_DELAY) debug_string("PRERUNNING");
                if (get_ecu_state() == RUNNING) debug_string("RUNNING");
                if (get_ecu_state() == STOPPING) debug_string("STOPPING");
                if (get_ecu_state() == ALARM) debug_string("ALARM");
                debug_string(" ");
                if (get_ecu_mode() == LOCAL_ONLY) debug_string("LOCAL_ONLY");
                if (get_ecu_mode() == MANUAL) debug_string("MANUAL");
                if (get_ecu_mode() == AUTOMATIC) debug_string("AUTOMATIC");
                debug_string("\r\n");
                break;
            
            case 1:
                debug_string("Button: ");
                debug_uint(get_user_interface_button_state(BUTTON_LOCAL_ROM_START_STOP, BUTTON_DOWN));
                debug_uint(get_user_interface_button_state(BUTTON_LOCAL_ROM_MODE, BUTTON_DOWN));
                debug_uint(get_user_interface_button_state(SWITCH_LOCAL_ROM_LOCAL, BUTTON_DOWN));
                debug_uint(get_user_interface_button_state(SWITCH_LOCAL_ROM_REMOTE, BUTTON_DOWN));
                debug_uint(get_user_interface_button_state(BUTTON_REMOTE_ROM_START_STOP, BUTTON_DOWN));
                debug_string("\r\n");
                break;
                
            case 2:
                debug_string("Dig sensor closed: ");
                debug_uint(get_sensor_digital_is_closed(SENSOR_DIGITAL_SWITCH_1));
                debug_uint(get_sensor_digital_is_closed(SENSOR_DIGITAL_SWITCH_2));
                debug_uint(get_sensor_digital_is_closed(SENSOR_DIGITAL_SWITCH_3));
                debug_uint(get_sensor_digital_is_closed(SENSOR_DIGITAL_SWITCH_4));
                debug_string(" activated: ");
                debug_uint(get_sensor_digital_is_activated(SENSOR_DIGITAL_SWITCH_1));
                debug_uint(get_sensor_digital_is_activated(SENSOR_DIGITAL_SWITCH_2));
                debug_uint(get_sensor_digital_is_activated(SENSOR_DIGITAL_SWITCH_3));
                debug_uint(get_sensor_digital_is_activated(SENSOR_DIGITAL_SWITCH_4));
                debug_string(" - alt: ");
                debug_uint(get_sensor_alt_feedback_is_activated());
                
                debug_string("\r\n");
                break;
            
            case 3:
                debug_string("E stop closed: ");
                debug_uint(get_sensor_e_stop_is_closed());
                debug_string(" activated: ");
                debug_uint(get_sensor_e_stop_is_activated());
                debug_string("\r\n");
                break;
                
            case 4:
                debug_string("Analog sensor res: ");
                //debug_uint(get_sensor_analog_value(SENSOR_ANALOG_INPUT_1));
                debug_uint(get_sensor_pic_engine_analog_sensor_raw(SENSOR_PIC_COM_AN_SENSOR_1));
                debug_string(" ");
                //debug_uint(get_sensor_analog_value(SENSOR_ANALOG_INPUT_2));
                debug_uint(get_sensor_pic_engine_analog_sensor_raw(SENSOR_PIC_COM_AN_SENSOR_2));
                debug_string(" ");
                debug_uint(get_userio_analog_input_value());
                debug_string(" ");
                debug_uint(get_adc1_raw_value(ADC1_RESULT_USER_SENSOR));
                debug_string("\r\n");
                break;
                
            case 5:
                debug_string("PT100: ");
                
                debug_uint(get_sensor_pic_pt100_temperature_raw(1));
                debug_string(" ");
                debug_uint(get_generator_measure_temperature_100mdeg(GENERATOR_MEASURE_TEMPERATURE_1));
                debug_string(" ");
                debug_uint(get_generator_measure_temperature_100mdeg(GENERATOR_MEASURE_TEMPERATURE_2));
                debug_string(" ");
                debug_uint(get_generator_measure_temperature_100mdeg(GENERATOR_MEASURE_TEMPERATURE_3));
                debug_string("\r\n");
                break;
                
            case 6:
                debug_string("Battery: ");
                debug_uint(get_sensor_battery_voltage_mv());
                debug_string(" ");
                debug_uint(get_adc1_raw_value(ADC1_RESULT_BATT_SENSE));
                debug_string("\r\n");
                break;
                
            case 7:
                debug_string("Generator V: ");
                debug_uint(get_generator_measure_voltage_100mv(GENERATOR_MEASURE_PHASE_1));
                debug_string(" ");
                debug_uint(get_generator_measure_voltage_100mv(GENERATOR_MEASURE_PHASE_2));
                debug_string(" ");
                debug_uint(get_generator_measure_voltage_100mv(GENERATOR_MEASURE_PHASE_3));
                debug_string("\r\n");
                break;
                
            case 8:
                debug_string("Generator A: ");
                debug_uint(get_generator_measure_current_100ma(GENERATOR_MEASURE_PHASE_1));
                debug_string(" ");
                debug_uint(get_generator_measure_current_100ma(GENERATOR_MEASURE_PHASE_2));
                debug_string(" ");
                debug_uint(get_generator_measure_current_100ma(GENERATOR_MEASURE_PHASE_3));
                debug_string("\r\n");
                break;
                
            case 9:
                debug_string("Generator VA: ");
                debug_uint(get_generator_measure_phase_power_va(GENERATOR_MEASURE_PHASE_1));
                debug_string(" ");
                debug_uint(get_generator_measure_phase_power_va(GENERATOR_MEASURE_PHASE_2));
                debug_string(" ");
                debug_uint(get_generator_measure_phase_power_va(GENERATOR_MEASURE_PHASE_3));
                debug_string("\r\n");
                break;
                
            case 10:
                debug_string("Generator: ");
                debug_uint(get_generator_measure_voltage_freq_10mhz());
                debug_string(" Hz ");
                debug_uint(get_generator_measure_rpm());
                debug_string(" RPM ");
                debug_uint(get_generator_measure_total_power_va());
                debug_string(" VA\r\n");
                break;
                
            case 11:
                debug_string("Sensor alarm: ");
                debug_uint(get_alarms_state(ALARM_SENSOR_DIGITAL_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_SENSOR_DIGITAL_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_SENSOR_DIGITAL_3));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_SENSOR_DIGITAL_4));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_SENSOR_ANALOG_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_SENSOR_ANALOG_2));
                debug_string("\r\n");
                break;
                
            case 12:
                debug_string("Generator alarm: ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_LOW_VOLTAGE_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_LOW_VOLTAGE_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_VOLTAGE_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_VOLTAGE_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_CURRENT_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_CURRENT_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_POWER_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERATOR_HIGH_POWER_2));
                debug_string("\r\n");
                break;
                
            case 13:
                debug_string("Engine alarm: ");
                debug_uint(get_alarms_state(ALARM_BATTERY_LOW_VOLTAGE));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_BATTERY_FAILED_TO_CHARGE));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_ENGINE_LOW_RPM_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_ENGINE_LOW_RPM_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_ENGINE_HIGH_RPM_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_ENGINE_HIGH_RPM_1));
                debug_string("\r\n");
                break;
                
            case 14:
                debug_string("ECU alarm: ");
                debug_uint(get_alarms_state(ALARM_GENERIC_FAILED_TO_START));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_FAILED_TO_STOP));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_E_STOP));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_MAINTENANCE));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_USER_DIG_1));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_USER_DIG_2));
                debug_string(" ");
                debug_uint(get_alarms_state(ALARM_GENERIC_USER_AN));
                debug_string("\r\n");
                break;
                
            case 15:
                debug_string("PIC com state: ");
                debug_uint(get_sensor_pic_com_state());
                debug_string("\r\n");
                break;
                
                /*
            case 16:
                rtcc_timestamp = get_rtcc_timestamp();
                debug_uint(rtcc_timestamp.hour);
                debug_string(":");
                debug_uint(rtcc_timestamp.min);
                debug_string(":");
                debug_uint(rtcc_timestamp.sec);
                debug_string(" ");
                debug_uint(rtcc_timestamp.day);
                debug_string("-");
                debug_uint(rtcc_timestamp.month);
                debug_string("-20");
                debug_uint(rtcc_timestamp.year);
                debug_string("\r\n");
                break;
                
            case 17:
                if (get_rtcc_backup_battery_good()) {
                    debug_string("RTCC backup battery ok\r\n");
                } else {
                    debug_string("RTCC backup battery fail\r\n");
                }
                break;*/
                
                
            default:
                if (line != 255) {
                    debug_string("\r\n");
                    line = 255;
                }
                break;
        }
        if (line<255) {
            line++;
        }
    }
}

int8_t uart_debug_ready(void) {
    if (debug_buffer.in == debug_buffer.out) {
        return 1;
    } else {
        return 0;
    }
}
