# 릴레이와 온습도 센서를 활용한 화재시 전기 차단 소켓

## 목표
- 목적, 응용에 부합하는 센서 데이터를 취득하고, 프로세스 간 통신(IPC), MariaDB(또는 MySQL)를 사용하여 데이터를 저장하고, MQTT를 통해 데이터를 전송하는 Linux 프로젝트.
- 온습도 센서를 이용하여 주변 온도와 습도를 측정한다. 화재, 전선의 저항으로 인한 온도 상승 등 비정상적인 온도 상황이 발생한다면 전선에 흐르는 전류를 차단하여 화재를 예방하고, 2차 사고를 방지한다.

## 도구 및 기술
- 리눅스 기반 시스템
- C 프로그래밍 언어
- IPC 메커니즘 (예: 파이프, 소켓, 공유메모리, 메시지큐, 세마포어)
- MariaDB (또는 MySQL) 데이터베이스
- MQTT 데이터 전송
- 센서 데이터 취득

## 작업
### 센서 데이터 생성 (C)
C 프로그램을 작성하여 센서 데이터를 취득합니다.
IPC 메커니즘을 사용하여 이 데이터를 다른 프로세스와 통신하도록 만듭니다.

### 프로세스 간 통신 (IPC)
C에서 IPC 메커니즘을 구현합니다.
시뮬레이션된 센서 데이터 취득 프로세스와 다른 프로세스 간에 데이터를 교환합니다.

### MariaDB 데이터베이스 통합
MariaDB에 연결합니다.
데이터베이스 및 센서 데이터를 저장할 테이블을 생성합니다.
시뮬레이션된 센서 데이터를 MariaDB 데이터베이스에 삽입하는 메커니즘을 개발합니다.

### MQTT 통합
Python 스크립트를 작성하여 센서 데이터를 MQTT 브로커에 전송합니다.
MQTT 데이터는 실시간으로 전송되어야 합니다.

### 데이터 모니터링 및 시각화
센서 데이터를 모니터링하고 시각화합니다.
사용가능한 라이브러리를 사용하여 실시간 플롯을 생성합니다.
IPC 및 MQTT로부터 수신한 데이터를 표시합니다.

## 파일 구조
```
.
├── IPC
│   ├── Common.h
│   ├── client
│   ├── client.cpp
│   ├── server
│   └── server.cpp
├── MQTT & Dashboard
│   ├── Common.h
│   ├── client
│   ├── client.cpp
│   ├── recive.py
│   ├── server
│   ├── server.cpp
│   └── view.py
├── README.md
├── Senser Data Create
│   ├── dht11
│   ├── dht11.c
│   ├── flug_sys
│   ├── flug_sys.c
│   └── relay.c
└── report.docx

```

## 실행 방법
### 센서 데이터 생성
<img width="452" alt="image" src="https://github.com/user-attachments/assets/7404a0bf-8c00-42b0-9723-5858ad58480b" />

통신 방식 : 1 – wired

1. ``` bash
   gcc -o sensor sensor.c -lwiringPi
   ```
2. ``` bash
   ./sensor
   ```
<img width="452" alt="image" src="https://github.com/user-attachments/assets/20c4366b-3468-4738-8cb3-d174b92bfbc8" />

### 릴레이 모듈 제어
1. ``` bash
   gcc -o sensor relay.c -lwiringPi
   ```
2. ``` bash
   ./relay
   ```

### 프로세스 간 통신(Socket)
1. ``` bash
   gcc -o sensor server.c -lwiringPi
   ```
2. ``` bash
   ./server
   ```
3. ``` bash
   gcc -o sensor client.c -lwiringPi
   ```
4. ``` bash
   ./client
   ```
<img width="318" alt="image" src="https://github.com/user-attachments/assets/e9041f7f-5bde-4a7a-a4ed-28587613fe5a" />

### mariaDB 결과
<img width="367" alt="image" src="https://github.com/user-attachments/assets/2d15d5a0-830d-47e1-9a1a-8ff45242d8a3" />

## 최종 실행 결과
<img width="451" alt="image" src="https://github.com/user-attachments/assets/456bb971-1725-45fe-8719-3b7793030747" />
<img width="452" alt="image" src="https://github.com/user-attachments/assets/6ed2ab9b-6da1-4b01-ab47-7de5c8d1f468" />

-	DHT 11로부터 온습도 데이터를 생성하여 소켓 통신 성공
-	클라이언트 측에서 서버 측으로 전달한 데이터를 mariaDB를 활용해 저장
-	서버는 mariaDB에 저장하며 MQTT로 데이터를 전송 성공
-	MQTT로 subscribe를 하면 값을 읽을 수 있음
-	mariaDB에 저장된 값을 불러와 데이터 모니터링 및 시각화

