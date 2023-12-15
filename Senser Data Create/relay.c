#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define Relay_PIN 26

void Relay_On()
{
	pinMode(Relay_PIN, OUTPUT);
	digitalWrite(Relay_PIN, HIGH);
}
	
void Relay_Off()
{
	pinMode(Relay_PIN, OUTPUT);
	digitalWrite(Relay_PIN, LOW);
}

int main(void)
{
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "WiringPi initialization failed\n");
        return 1;
    }
    while (1) {
		Relay_On();
		delay(5000);
		Relay_Off();
		delay(5000);
	}
	
    return 0;
}
