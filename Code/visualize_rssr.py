import pygame
import random
import paho.mqtt.client as mqtt
import time
import json

import pandas as pd  # module for
from matplotlib import pyplot as plt
plt.style.use('seaborn-dark')

# I'm going to plot the results of rssi:
x_rssi = []
y_time = []
t_number = 0


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
    global x_rssi, y_time, t_number
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
        x_rssi.append(str(beacon_distance))
        y_time.append(str(t_number))

        # y_time.append(2)
        t_number += 1
        print(x_rssi, '34')


def reddrawGameWindow():
    """
    # always print first the background
    img_pos = 200
    win.fill((255, 255, 255))
    win.blit(bg, (0, 0))

    render_text = ['A1', 'A2', 'A3']
    text_pos_y = img_pos - 40
    for index, r in enumerate(render_text):
        x = font.render(r, 1, (0, 0, 0))
        win.blit(x, (100 + 230*index, text_pos_y))
        if index % 2 == 0:
            text_pos_y = img_pos + 120
        else:
            text_pos_y = img_pos - 40
    # for beacon in beacons:
    #     beacon.draw(win)
    for e in esp:
        for b in e.beacons:
            b.draw(win)
    # pygame.draw.rect(win, (0, 0, 0), (550, 10, 120, 40))
    # pygame.draw.rect(win, (255, 0, 0), (555, 15, 110, 30))
    pygame.display.update()  # update the screen frames
    """
    pass

def rssr_distance(rssi, tx, n):
    return 10**((tx-rssi)/n)


run = True  # The game loop running
beacons = []
esp = []
position_adjustments = [0, 0]

win_x = 700
win_y = 500

client = connect_mqtt()  # I connect to mqtt broker
pygame.init()  # Inicialize pygame interface
win = pygame.display.set_mode((win_x, win_y - 20))  # dimensions of it
pygame.display.set_caption("Rolling tracker")  # title of this shit of game
clock = pygame.time.Clock()
bg = pygame.image.load('grass.jpg')
font = pygame.font.SysFont('bitstreamverasans', 30, True, True)


timer_update_screen = int(round(time.time()))
refresh_time = 6
reddrawGameWindow()

while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run = False
            client.loop_stop()  # stop the loop
            plt.plot(y_time, x_rssi,
                     linestyle='--', marker='.', label='nop')
            plt.show()

    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):

        timer_update_screen = int(round(time.time()))
pygame.quit()
