#include <Arduino.h>
#include "../lib/Button/src/Button.h"

int BLUELED   = 6;
int ORANGELED = 9;
Button buttonLeft = Button(4);
Button buttonRight = Button(5);
int leftstate = 0;
int rightstate = 0;

void setup() {
    Serial.begin(9600);
    
}

void loop() { // run over and over
    
    buttonLeft.check();
    buttonRight.check();
    int nbleft = buttonLeft.getNumber();
    int nbright = buttonRight.getNumber();
    Serial.println(nbleft);
    Serial.println(nbright);
}
