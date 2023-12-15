import streamlit as st
import pandas as pd
import time
import pymysql
import altair as alt

def generate_data():
    db = pymysql.connect(host="localhost", user="root", password="rhtn1720", charset="utf8")
    cursor = db.cursor(pymysql.cursors.DictCursor)
    cursor.execute('USE sensor;')
    
    select_query = 'SELECT * FROM sensordata'
    cursor.execute(select_query)
    
    df_org = cursor.fetchall()
    df_org = pd.DataFrame(df_org)
    df = pd.DataFrame(df_org, columns=['temp', 'humi'])
    df['time'] = pd.to_datetime(df_org['time'])
    
    return df

try:
    data = generate_data()

    st.title("temp/humi real time dashboard")

    # Altair 차트 생성
    chart = alt.Chart(data).mark_line().encode(
        x='time:T',
        y=alt.Y('temp:Q', scale=alt.Scale(domain=[-10, 80]), title='Temperature (°C)'),
        color=alt.value('red')
    ).properties(
        width=600,
        height=300
    ) + \
    alt.Chart(data).mark_line().encode(
        x='time:T',
        y=alt.Y('humi:Q', scale=alt.Scale(domain=[-10, 100]), title='Humidity (%)'),
        color=alt.value('blue')
    )

    chart_component = st.altair_chart(chart, use_container_width=True)

    ph1 = st.empty()
    ph2 = st.empty()

    while True:
        new_data = generate_data()
        data = pd.concat([data, new_data])

        updated_chart = alt.Chart(data).mark_line().encode(
            x='time:T',
            y=alt.Y('temp:Q', title='Temperature (°C)'),
            color=alt.value('red')
        ).properties(
            width=600,
            height=300
        ) + \
        alt.Chart(data).mark_line().encode(
            x='time:T',
            y=alt.Y('humi:Q', title='Humidity (%)'),
            color=alt.value('blue')
        )

        chart_component.altair_chart(updated_chart)

        ph1.metric(
            label="Temp",
            value=f"{data.iloc[-1,0]} °C"
        )
        ph2.metric(
            label="Humi",
            value=f"{data.iloc[-1,1]} %"
        )
        time.sleep(1)

except Exception as e:
    st.title(f"Error: {e}")
