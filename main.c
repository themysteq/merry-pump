#define F_CPU 8000000
#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
//#include <util/atomic.h>
#include <stdbool.h>

#define IDLE_STATE 0x03
#define WORKING_STATE 0x09
#define FAIL_STATE 0x00
#define PUMPING_SYSTEM_PORT PORTD
#define SENSOR_PORT PIND
#define PUMPING_SYSTEM_DDR DDRD
#define INPUT_TANK_PIN PD5
#define OUTPUT_TANK_PIN PD6
#define PUMP_PIN PD7
#define VALVE_PIN PD4
#define FAIL_LED_PIN PB3
#define WORKING_LED_PIN PB2
#define IDLE_LED_PIN PB1
#define LED_PORT PORTB
#define LED_PORT_DDR DDRB

volatile unsigned char current_state = IDLE_STATE;

void fail_led_on(void){
    LED_PORT = (1<<FAIL_LED_PIN);
}
void fail_led_off(void){
    //LED_PORT = ~(1<<FAIL_LED_PIN);
}
void working_led_on(void){
    LED_PORT = (1<<WORKING_LED_PIN);
}
void working_led_off(void){
   // LED_PORT = ~(1<<WORKING_LED_PIN);
}
void idle_led_on(void){
    LED_PORT = (1<<IDLE_LED_PIN);
}
void idle_led_off(void){
   // LED_PORT &= ~(1<<IDLE_LED_PIN);
}
void stop_pump(void){
    PUMPING_SYSTEM_PORT &= ~(1<<PUMP_PIN);
}

void start_pump(void){
    PUMPING_SYSTEM_PORT |= (1<<PUMP_PIN);
}

void close_valve(void){
    PUMPING_SYSTEM_PORT &= ~(1<<VALVE_PIN);
}

void open_valve(void){
    PUMPING_SYSTEM_PORT |= (1<<VALVE_PIN);
}

unsigned char check_input_tank_empty(void){
    //true if empty
    return (unsigned char)0+(unsigned char)((SENSOR_PORT)&(1<<INPUT_TANK_PIN));
}

unsigned char check_output_tank_empty(void){
    //true if empty
    return (unsigned char)0+(unsigned char)((SENSOR_PORT)&(1<<OUTPUT_TANK_PIN));
}

void handle_fail_state(void){
    fail_led_on();
    stop_pump();
    close_valve();
    if(check_input_tank_empty()) {
        current_state = FAIL_STATE;
    }
    else {
        current_state = IDLE_STATE;
    }

}

void handle_idle_state(void){
    stop_pump();
    close_valve();
    idle_led_on();
    if(check_input_tank_empty()>0) {
        current_state = FAIL_STATE;
    }
    else if(check_output_tank_empty())
    {
        current_state = WORKING_STATE;
    }
    else {
        current_state = IDLE_STATE;
    }

 }

void handle_working_state(void){
    open_valve();
    start_pump();
    working_led_on();
    if(check_input_tank_empty()) { current_state = FAIL_STATE;
        return;
    }
    else if(check_output_tank_empty()){
        current_state = WORKING_STATE;
        return;
    }
    else {
        current_state = IDLE_STATE;
        return;
    }

    //return current_state;
}

void init_system(void){
    _delay_ms(1000);
    LED_PORT_DDR |= (1<<FAIL_LED_PIN)|(1<<IDLE_LED_PIN)|(1<<WORKING_LED_PIN);
    fail_led_on();
    _delay_ms(1000);
    working_led_on();
    _delay_ms(1000);
    idle_led_on();
    _delay_ms(1000);
    //PUMPING_SYSTEM_DDR &= (~(1<<OUTPUT_TANK_PIN))&(~(1<<INPUT_TANK_PIN)); //input 0
    PUMPING_SYSTEM_DDR |= (1<<PUMP_PIN)|(1<<VALVE_PIN); //output 1
    stop_pump();
    close_valve();
    current_state = IDLE_STATE;
    _delay_ms(1000);
}
int main(void){
    init_system();
    while(1){
            _delay_ms(100);
            if(current_state == FAIL_STATE) {
                handle_fail_state();
            }
            else if(current_state == WORKING_STATE) {
                handle_working_state();
            }
            else {
                handle_idle_state();
            }


        }

}