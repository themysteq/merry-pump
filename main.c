#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <util/atomic.h>

#define IDLE_STATE 0x03
#define WORKING_STATE 0x09
#define FAIL_STATE 0x00
#define PUMPING_SYSTEM_PORT PORTD
#define PUMPING_SYSTEM_DDR DDRD
#define INPUT_TANK_PIN PD5
#define OUTPUT_TANK_PIN PD6
#define PUMP_PIN PD7
#define VALVE_PIN PD4
#define FAIL_LED_PIN PB1
#define WORKING_LED_PIN PB2
#define IDLE_LED_PIN PB3
#define LED_PORT PORTB
#define LED_PORT_DDR DDRB



volatile unsigned char current_state = FAIL_STATE;

void fail_led(void){
    //TODO
}
void working_led(void){
    //TODO
}
void idle_led(void){
    //TODO
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
    //true if not empty
    return (unsigned char)((PUMPING_SYSTEM_PORT)&(1<<INPUT_TANK_PIN));
}

unsigned char check_output_tank_full(void){
    //true if full
    return (unsigned char)((PUMPING_SYSTEM_PORT)&(1<<OUTPUT_TANK_PIN));
}

unsigned char handle_fail_state(void){
    stop_pump();
    close_valve();
    fail_led();
    if(check_input_tank_empty()) return FAIL_STATE;
    if(!check_input_tank_empty()) return IDLE_STATE;

    return FAIL_STATE;

}

unsigned char handle_idle_state(void){
    stop_pump();
    close_valve();
    idle_led();
    if(check_input_tank_empty()) return FAIL_STATE;
    if(check_output_tank_full()) return IDLE_STATE;
    if(!check_output_tank_full()) return WORKING_STATE;

    return IDLE_STATE;
 }

unsigned char handle_working_state(void){
    open_valve();
    start_pump();
    working_led();
    if(check_output_tank_full()) return IDLE_STATE;
    if(check_input_tank_empty()) return FAIL_STATE;
    if(!check_output_tank_full()) return WORKING_STATE;

    return IDLE_STATE;
}

void init_system(void){
    _delay_ms(4000);
    PUMPING_SYSTEM_DDR &= (~(1<<OUTPUT_TANK_PIN))&(~(1<<INPUT_TANK_PIN)); //input 0
    PUMPING_SYSTEM_DDR |= (1<<PUMP_PIN)|(1<<VALVE_PIN); //output 1
    stop_pump();
    close_valve();
    current_state = FAIL_STATE;
    _delay_ms(4000);
}
int main(void){
    init_system();
    while(1){
        _delay_ms(100);
        switch(current_state) {
            case IDLE_STATE:
                current_state = handle_idle_state();
                break;

            case FAIL_STATE:
                current_state = handle_fail_state();
                break;

            case WORKING_STATE:
                current_state = handle_working_state();
                break;
        }
    }

}