#include "Common.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define DHT_PIN 7 
#define MAX_TIMINGS 85

#define Relay_PIN 26

char *SERVERIP = (char *)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    50



// Function to send data to the server
void sendDataToServer(SOCKET sock, float temperature, float humidity) {
    char buf[BUFSIZE];
    int len;

    len = snprintf(buf, BUFSIZE, "%.2f %.2f\n", temperature, humidity);

    printf("[TCP Client] Send Data: %s", buf);

    int retval = send(sock, buf, len, 0);
    if (retval == SOCKET_ERROR) {
        err_display("send()");
    }
}

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
	
	int retval;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");


	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

    while (1) {
        readDHTData();

        float humidity = (float)((dht_data[0]) + dht_data[1]);
        float temperature = (float)(((dht_data[2])) + dht_data[3]);

        if (dht_data[2] & 0x80) {
            temperature *= -1.0;
        }

        // printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);
        
        if (temperature >= 50.00){
			digitalWrite(Relay_PIN, HIGH);
		}
		else {
			digitalWrite(Relay_PIN, LOW);
		}
		
		sendDataToServer(sock, temperature, humidity);

        delay(2000);
    }

    return 0;
}
