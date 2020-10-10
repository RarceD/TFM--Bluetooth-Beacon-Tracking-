import random
import paho.mqtt.client as mqtt
import time
import json

from matplotlib import pyplot as plt

# I'm going to plot the results of rssi:
rssi_b1 = []
rssi_b2 = []
x_time = []
t_number = 0

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
plt.style.use('fivethirtyeight')
def connect_mqtt():
    broker_address = "broker.mqttdashboard.com"
    client = mqtt.Client("asdf1234asdf1234asdf")  # create new instance
    client.on_message = on_message  # attach function to callback
    print("connecting to broker")
    client.connect(broker_address)  # connect to broker
    print("Subscribing to topic", "master_beacon")
    client.subscribe("master_beacon")
    print("Publishing message to topic", "master_beacon_ack")
    msg = '''{"ok":true}'''
    client.publish("master_beacon/ack", msg)
    client.loop_start()  # start the loop
    return client

def on_message(client, userdata, message):
    global rssi_b1, rssi_b2, t_number, x_time
    # print("message topic=",message.topic)
    # print("message retain flag=",message.retain)
    # Example json: {"esp":"A1","beacon":[ {"uuid":5245,"distance":1.23},{"uuid":52345, "distance":1.23 }]}
    msg = str(message.payload.decode("utf-8"))
    parsed_json = json.loads(msg)
    print('_________________')
    for index, b in enumerate(parsed_json['beacon']):
        # Get the distance and the uuid of the beacon:
        beacon_distance = float(parsed_json['beacon'][index]['distance'])
        beacon_uuid = str(parsed_json['beacon'][index]['uuid'])

        ##I add the devices to a list:
        rssi_b1.append(beacon_distance)
        rssi_b2.append(beacon_distance+2.0)
        x_time.append(str(t_number))
        t_number += 1


def rssi_distance(rssi, tx, n):
    return 10**((tx-rssi)/n)
def rssi_filter(rssi):
    filter_rssi = 0
    for r in rssi:
        filter_rssi+=r
    return filter_rssi/len(rssi_b1)

run = True  # The game loop running
beacons = []
esp = []
position_adjustments = [0, 0]

client = connect_mqtt()  # I connect to mqtt broker

timer_update_screen = int(round(time.time()))
refresh_time = 3
# reddrawGameWindow()

while(run):
    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):
        # run = False
        client.loop_stop()  # stop the loop
        ax.plot(x_time, rssi_b1)
        ax.plot(x_time, rssi_b2)
        ax.set_title("Diferences between RSSI from 2 beacons at 1 meter")
        ax.set_ylim([-100,-20])
        ax.set_xlabel("time in seconds")
        ax.set_ylabel("RSSI")
        # ax.set_style('fivethirtyeight')
        # ax.set_style.use('fivethirtyeight')

        #I apply a filter and get the distance:
        filter_rssi = rssi_filter(rssi_b1)
        distance_rssi = rssi_distance(filter_rssi, 13, 2.2)

        print(filter_rssi, distance_rssi)
        
        plt.show()
        timer_update_screen = int(round(time.time()))

