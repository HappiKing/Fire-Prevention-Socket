import paho.mqtt.client as mqtt
import pandas as pd
from datetime import datetime


df = pd.DataFrame(columns=['temp', 'humi'])
# subscriber callback
def on_message(client, userdata, message):
    topic = message.topic
    value = float(message.payload.decode("utf-8"))  # 문자열을 숫자로 변환
    
    # 현재 시간 가져오기
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # 데이터 프레임에 새로운 행 추가 또는 누적
    global df
    if len(df) == 0 or df.at[df.index[-1], 'timestamp'] != timestamp:
        df = df.append({'timestamp': timestamp, 'temp': 0, 'humi': 0}, ignore_index=True)
    
    # 토픽에 따라 데이터 프레임에 값을 누적
    if topic == "test/temp":
        df.at[df.index[-1], 'temp'] += value
    elif topic == "test/humi":
        df.at[df.index[-1], 'humi'] += value
    
    # 출력
    print(df)



broker_address = "localhost"
client1 = mqtt.Client("client1")
client1.connect(broker_address)
client1.subscribe("test/temp")
client1.subscribe("test/humi")
client1.on_message = on_message
