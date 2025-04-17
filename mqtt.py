import paho.mqtt.client as paho
import sys

ID = 'pc_sub'
BROKER = "mqtt.eclipseprojects.io"
PORT = 1883
topic = ''
payload = ''


def onMessage(client, userdata, msg):
    global topic, payload
    topic = msg.topic
    payload = msg.payload.decode()


client = paho.Client(ID)
client.on_message = onMessage

if client.connect(BROKER, PORT, 60) != 0:
    print("Could not connect to MQTT Broker!")
    sys.exit(-1)

client.subscribe("esp/faixa/carro")
client.subscribe("esp/123")