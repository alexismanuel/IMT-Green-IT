#include "./Button.h"

Button::Button(int myPin) {
    pin = myPin;
    pinMode(pin, INPUT);
}

void Button::check() {
    int val = digitalRead(pin);
    if (lastval != val && lastval == 1) {
        cpt++;
        j=millis();
    }
    lastval=val;
}

int Button::getNumber() {
    unsigned long int i = millis();
    while (i-j<2000) {
        i=millis();
        check();
    }
    int tmp=cpt;
    cpt=0;
    return tmp;
}
