import pygame
import random
import paho.mqtt.client as mqtt
import time
import json
import sys
import math
from BeaconClass import Beacon_Data, get_intersections


PI = 3.141592653589793
beacon_data = []
beacon_data.append(Beacon_Data("test", 22, "esp_test"))

def connect_mqtt():
    broker_address = "broker.mqttdashboard.com"
    # create new instance
    client = mqtt.Client("a1f" + str(random.randint(0, 9234)) + "as34asdf")
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
    global esp, beacon_data
    # print("message topic=",message.topic)
    # print("message retain flag=",message.retain)
    # Example json: {"esp":"A1","beacon":[ {"uuid":5245,"distance":1.23},{"uuid":52345, "distance":1.23 }]}
    # print("message received: ", msg)
    msg = str(message.payload.decode("utf-8"))
    parsed_json = json.loads(msg)
    esp_local = str(parsed_json['esp'])
    for index, b in enumerate(parsed_json['beacon']):
        # Get the distance and the uuid of the beacon:
        beacon_distance = float(
            parsed_json['beacon'][index]['distance'])
        beacon_uuid = str(parsed_json['beacon'][index]['uuid'])
        # print("enter:" ,beacon_uuid)
        # if (len(beacon_data == 0)):
        #     a = Beacon_Data(beacon_uuid, beacon_distance, esp_local)
        #     beacon_data.append(a)
        # Add the beacon to the master if it is not repeat:
        create_new = True
        for index_beacon, saved_beacons in enumerate(beacon_data):
            if beacon_uuid in saved_beacons.name:
                # If it is repeated the beacon:
                # print("Repeat beacon on ", beacon_uuid)
                # Find the the esp associated
                associate_new_esp = True
                # check if the esp is the same:
                if esp_local in saved_beacons.esp:
                    # print("Update rssi")
                    saved_beacons.rssi[index_esp] = beacon_distance
                    associate_new_esp = False
                if associate_new_esp:
                    # print("Same beacon diferent esp")
                    saved_beacons.esp.append(esp_local)
                    saved_beacons.rssi.append(beacon_distance)
                create_new = False
        # If i don`t have beacons I insert them
        if (create_new):
            a = Beacon_Data(beacon_uuid, beacon_distance, esp_local)
            beacon_data.append(a)

def trilateration(beacon_data):
    for b in beacon_data:
        print(b.name)
        if (len(b.esp) == 2):
            x0 = 100
            y0 = 50
            r0 = abs(b.rssi[0]) * 6
            x1 = 700
            y1 = 550
            r1 = abs(b.rssi[1]) * 6
            pygame.draw.circle(win, (77, 126, 189), (x0, y0), int(r0),1)
            pygame.draw.circle(win, (150, 74, 189), (x1, y1), int(r1),1)
            points = get_intersections(x0, y0, r0, x1, y1, r1)
            try:
                intersection_points = []            
                for p in points:
                    intersection_points.append(int(p))
                pygame.draw.circle(win, (234,random.randint(2,255), random.randint(2,255)), (intersection_points[0], intersection_points[1]), 7)
                pygame.draw.circle(win, (234, random.randint(2,255), random.randint(2,255)), (intersection_points[2], intersection_points[3]), 7)
            except:
                pass
            
        elif(len(b.esp) == 1):
            if b.esp[0] == "A2":
                x = int(10*abs(b.rssi[0]) - 200)
                y = 50
                pygame.draw.circle(win, (234,random.randint(2,255), random.randint(2,255)), (x, y), 7)
                print(x,y)
            elif b.esp[0] == "A5":
                x = int(-8.57*abs(b.rssi[0]) + 957.14)
                y = 550
                print(x,y)
                pygame.draw.circle(win, (234,random.randint(2,255), random.randint(2,255)), (x, y), 7)
        elif(len(b.esp) == 3):
            x0 = 100
            y0 = 50
            r0 = abs(b.rssi[0]) * 6
            x1 = 700
            y1 = 550
            r1 = abs(b.rssi[1]) * 6
            x2 = 100
            y2 = 550
            r2 = abs(b.rssi[2]) * 6
            pygame.draw.circle(win, (77, 126, 189), (x0, y0), int(r0),1)
            pygame.draw.circle(win, (150, 74, 189), (x1, y1), int(r1),1)
            pygame.draw.circle(win, (43, 74, 189), (x2, y2), int(r2),1)
            x = [x0, x1, x2]
            y = [y0, y1, y2]
            r = [r0, r1, r2]
            for i in range(1,3):
                points = get_intersections(x0, y0, r0, x[i], y[i], r[i])
                try:
                    intersection_points = []            
                    for p in points:
                        intersection_points.append(int(p))
                    pygame.draw.circle(win, (234,255, random.randint(2,255)), (intersection_points[0], intersection_points[1]), 7)
                    pygame.draw.circle(win, (234,255, random.randint(2,255)), (intersection_points[2], intersection_points[3]), 7)
                except:
                    pass
            for i in range(0,2):
                points = get_intersections(x2, y2, r2, x[i], y[i], r[i])
                try:
                    intersection_points = []            
                    for p in points:
                        intersection_points.append(int(p))
                    pygame.draw.circle(win, (234,255,random.randint(2,255)), (intersection_points[0], intersection_points[1]), 7)
                    pygame.draw.circle(win, (234,255, random.randint(2,255)), (intersection_points[2], intersection_points[3]), 7)
                except:
                    pass
            # print("1 master only")

def reddrawGameWindow():
    global beacon_data
    win.fill((145, 213, 250))
    win.blit(word_image, (0, 0))
    # I set a grid to better positioning
    l_x, l_y = 0, 0
    debug = False
    while (debug and (l_x <= win_x or l_y <= win_y)):
        pygame.draw.line(win, (134, 136, 143), (l_x, 0), (l_x, win_y))
        pygame.draw.line(win, (134, 136, 143), (0, l_y), (win_x, l_y))
        l_y += 50
        l_x += 50

    for e in esp:
        pygame.draw.rect(win, (0, 0, 0), (e.x, e.y, 25, 25))
        pygame.draw.rect(win, (255, 0, 53),
                         (e.x + 5, e.y+5, 25-5*2, 25-5*2))
        for b in e.beacons:
            b.draw(win)
    trilateration(beacon_data)
    pygame.display.update()  # update the screen frames

win_x = 900
win_y = 600
client = connect_mqtt()  # I connect to mqtt broker
pygame.init()  # Inicialize pygame interface
win = pygame.display.set_mode((win_x, win_y))  # dimensions of it
# title of this shit of game
pygame.display.set_caption("House members tracker")
clock = pygame.time.Clock()
font = pygame.font.SysFont('bitstreamverasans', 30, True, True)
# Load the config file with the colors and room dimensions
# I also create global esps:
beacons = []
esp = []

# Render for the first time the window:    w
word_image = pygame.image.load('images/supermarket-floor.jpg')
word_image = pygame.transform.scale(word_image, (win_x, win_y))
reddrawGameWindow()
# Set a timer to reload the game
timer_update_screen = int(round(time.time()))
refresh_time = 5
# The game loop running
run = True
while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run = False
            client.loop_stop()  # stop the loop
    # reddrawGameWindow()
    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):
        # I calculate the positions:
        # trilateration(beacon_data, win)
        # Render the screen:
        reddrawGameWindow()
        # Delete all the info:

        # Reestart the timer:
        timer_update_screen = int(round(time.time()))
pygame.quit()
