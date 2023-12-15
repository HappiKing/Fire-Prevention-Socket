#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define DHT_PIN 7 
#define MAX_TIMINGS 85

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

int dht_data[5] = {0, 0, 0, 0, 0}; 

void readDHTData() {
    uint8_t last_state = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    dht_data[0] = dht_data[1] = dht_data[2] = dht_data[3] = dht_data[4] = 0;

    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, HIGH);
    delay(10);
    digitalWrite(DHT_PIN, LOW);
    delay(18);
    digitalWrite(DHT_PIN, HIGH);
    delayMicroseconds(30);
    pinMode(DHT_PIN, INPUT);

    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while (digitalRead(DHT_PIN) == last_state) {
            counter++;
            delayMicroseconds(2);
            if (counter == 255) {
                break;
            }
        }
        last_state = digitalRead(DHT_PIN);

        if (counter == 255) {
            break;
        }

        // 처음 두 비트는 항상 0이므로 스킵
        if ((i >= 4) && (i % 2 == 0)) {
            dht_data[j / 8] <<= 1;
            if (counter > 16) {
                dht_data[j / 8] |= 1;
            }
            j++;
        }
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "WiringPi initialization failed\n");
        return 1;
    }
    
    pinMode(Relay_PIN, OUTPUT);
	digitalWrite(Relay_PIN, LOW);

    while (1) {
        readDHTData();

        float humidity = (float)((dht_data[0]) + dht_data[1]);
        float temperature = (float)(((dht_data[2])) + dht_data[3]);

        if (dht_data[2] & 0x80) {
            temperature *= -1.0;
        }

        printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
        
        if (temperature >= 50.00){
			digitalWrite(Relay_PIN, HIGH);
		}
		else {
			digitalWrite(Relay_PIN, LOW);
		}
			

        delay(2000);
    }

    return 0;
}
