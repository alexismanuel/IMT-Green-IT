#include <Arduino.h>
#include <LowPower.h>

void setup () {

}

void loop() {
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);
}
