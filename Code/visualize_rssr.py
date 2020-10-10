import pygame
import random
import paho.mqtt.client as mqtt
import time
import json
import numpy as np

import pandas as pd  # module for
from matplotlib import pyplot as plt

# I'm going to plot the results of rssi:
rssi_b1 = []
rssi_b2 = []
x_time = []
t_number = 0

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)





# x = np.linspace(0, 10)

# # Fixing random state for reproducibility
# np.random.seed(19680801)

# fig, ax = plt.subplots()

# ax.plot(x, np.sin(x) + x + np.random.randn(50))
# ax.plot(x, np.sin(x) + 0.5 * x + np.random.randn(50))
# ax.plot(x, np.sin(x) + 2 * x + np.random.randn(50))
# ax.plot(x, np.sin(x) - 0.5 * x + np.random.randn(50))
# ax.plot(x, np.sin(x) - 2 * x + np.random.randn(50))
# ax.plot(x, np.sin(x) + np.random.randn(50))
# ax.set_title("'fivethirtyeight' style sheet")

# plt.show()

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


def rssr_distance(rssi, tx, n):
    return 10**((tx-rssi)/n)


run = True  # The game loop running
beacons = []
esp = []
position_adjustments = [0, 0]


client = connect_mqtt()  # I connect to mqtt broker



timer_update_screen = int(round(time.time()))
refresh_time = 10
# reddrawGameWindow()

while(run):
    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):
        run = False
        client.loop_stop()  # stop the loop
        ax.plot(x_time, rssi_b1, color='tab:blue')
        ax.plot(x_time, rssi_b2, color='tab:orange')
        ax.set_title("RSSI of beacons at 1 meter")
        plt.show()
        timer_update_screen = int(round(time.time()))

