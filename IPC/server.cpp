#include "Common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <mariadb/mysql.h>
#include <time.h>

// MariaDB Connection pameters
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASSWORD "rhtn1720"
#define DB_NAME "sensor"

// TCP Socket Parameters
#define SERVERPORT 9000
#define BUFSIZE    512

#define MAX_VALUES 10

void processValues(char* buf) {
	
	MYSQL *conn = mysql_init(NULL);
	
	if (conn == NULL) {
		fprintf(stderr, "mysql_init() failed\n");
		exit(EXIT_FAILURE);
	}
	
	if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) {
		fprintf(stderr, "mysql_real_connect() failed\n");
		mysql_close(conn);
		exit(EXIT_FAILURE);
	}
	
	char query[255];
	sprintf(query,"create table if not exists sensordata (id INT AUTO_INCREMENT PRIMARY KEY, time DATETIME, temp FLOAT(10), humi FLOAT(10))");
	
	if (mysql_query(conn, query) != 0) {
		fprintf(stderr,"MariaDB query execution failed: %s\n", mysql_error(conn));
	}
	
    char* values[MAX_VALUES];
    int valueCount = 0;

    char* token = strtok(buf, " ");
    while (token != NULL && valueCount < MAX_VALUES) {
        values[valueCount] = token;
        valueCount++;
        token = strtok(NULL, " ");
    }
    
    
    time_t currentTime;
    struct tm *localTime;
    
    time(&currentTime);
    
    localTime = localtime(&currentTime);
    
    char currentTimeString[20];
    sprintf(currentTimeString, "%04d-%02d-%02d %02d:%02d:%02d",
            localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

	
    
    
    
    sprintf(query, "INSERT INTO sensordata (time, temp, humi) VALUES ('%s', '%s', '%s')",currentTimeString, values[0], values[1]);
    if (mysql_query(conn, query) != 0) {
		fprintf(stderr,"MariaDB query execution failed: %s\n", mysql_error(conn));
	}
	
    printf("Token Data:%s %s %s\n", currentTimeString,values[0], values[1]);
    

}



int _recv_ahead(SOCKET s, char *p)
{
	static __thread int nbytes = 0;
	static __thread char buf[1024];
	static __thread char *ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (nbytes == 0)
			return 0;
		ptr = buf;
	}

	--nbytes;
	*p = *ptr++;
	return 1;
}

int recvline(SOCKET s, char *buf, int maxlen)
{
	int n, nbytes;
	char c, *ptr = buf;

	for (n = 1; n < maxlen; n++) {
		nbytes = _recv_ahead(s, &c);
		if (nbytes == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}
		else
			return SOCKET_ERROR;
	}

	*ptr = 0;
	return n;
}

int main(int argc, char *argv[])
{
	
	int retval;


	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");


	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char buf[BUFSIZE + 1];



	while (1) {

		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}


		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] Connect IP addr=%s, Port Number=%d\n",
			addr, ntohs(clientaddr.sin_port));


		while (1) {

			retval = recvline(client_sock, buf, BUFSIZE + 1);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			printf("[TCP/%s:%d] %s", addr, ntohs(clientaddr.sin_port), buf);
			processValues(buf);

		}


		close(client_sock);
		printf("[TCP Server] Disconnect IP addr=%s, Port Number=%d\n",
			addr, ntohs(clientaddr.sin_port));
	}


	close(listen_sock);
	return 0;
}
