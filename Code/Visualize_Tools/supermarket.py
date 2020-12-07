import pygame
import random
import paho.mqtt.client as mqtt
import time
import json
import sys
import math
from BeaconClass import Beacon, Esp

class Beacon_Data():
    def __init__(self, name, rssi, esp):
        self.name = name
        self.esp = []
        self.rssi = []
        self.esp.append(esp)
        self.rssi.append(rssi)
        self.print_itself()

    def print_itself(self):
        print("Beacon: " + self.name)
        for i in range(0, len(self.esp)):
            print("-> ", self.esp[i], " - ", str(self.rssi[i]))
    def add_rssi(self, esp, rssi):
        self.esp.append(esp)
        self.rssi.append(rssi)
    def update_rssi(self, new_rssi, position):
        self.rssi[position] = new_rssi


PI = 3.141592653589793
beacon_data = []
beacon_data.append(Beacon_Data("test", 22, "esp_test"))
# all_beacons = []


def connect_mqtt():
    broker_address = "broker.mqttdashboard.com"
    client = mqtt.Client("asd1f1234asdf1234asdf")  # create new instance
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
    global esp, beacon_data, Beacon_Data
    # print("message topic=",message.topic)
    # print("message retain flag=",message.retain)
    # Example json: {"esp":"A1","beacon":[ {"uuid":5245,"distance":1.23},{"uuid":52345, "distance":1.23 }]}
    msg = str(message.payload.decode("utf-8"))
    # print("message received: ", msg)
    parsed_json = json.loads(msg)
    # for e in esp:
    #     if (parsed_json['esp'] == e.uuid):
    #         # print(parsed_json['esp'])
    esp_local = parsed_json['esp']
    for index, b in enumerate(parsed_json['beacon']):
        # Get the distance and the uuid of the beacon:
        beacon_distance = float(
            parsed_json['beacon'][index]['distance'])
        beacon_uuid = str(parsed_json['beacon'][index]['uuid'])
        print(beacon_uuid, esp_local)
        print(beacon_data)

        if (len(beacon_data == 0)):
            a = Beacon_Data(beacon_uuid, beacon_distance, esp_local)
            beacon_data.append(a)

        # Add the beacon to the master if it is not repeat:
        create_new = True
        for index_beacon, saved_beacons in enumerate(beacon_data):
            if beacon_uuid in saved_beacons.name:
                #If it is repeated the beacon:
                print("Repeat beacon on ", beacon_uuid)
                #Find the the esp associated
                associate_new_esp = True
                for index_esp, esp_repeat in saved_beacons.esp:
                    if (e.uuid == esp_repeat):
                        saved_beacons[index_esp].distance =  beacon_distance
                        print("Update position")
                        create_new = False
                        associate_new_esp = False
                if associate_new_esp:
                    saved_beacons.esp.append(parsed_json['esp'])
                    saved_beacons.rssi.append(beacon_distance)
            #If i don`t have beacons I insert them
        
        if (create_new):
            print("Create beacon", beacon_uuid)
            a = Beacon_Data(beacon_uuid, beacon_distance, esp_local)
        print(len(beacon_data))


        

        # for l in beacon_data:
        #     l.print_itself()
        # all_beacons = []
        # for s in e.beacons:
        #     all_beacons.append(s.uuid)
        # if (len(e.beacons) == 0):
        #     e.beacons.append(
        #         Beacon(150, 400, beacon_uuid, beacon_distance))
        #     # print('First beacon created: ', e.beacons[0].uuid)
        # else:
        #     if (beacon_uuid in all_beacons):
        #         pass
        #     else:
        #         e.beacons.append(
        #             Beacon(150, 400, beacon_uuid, beacon_distance))


def reddrawGameWindow():
    win.fill((145, 213, 250))
    win.blit(word_image, (0, 0))

    # I set a grid to better positioning
    l_x, l_y=0, 0
    debug=True
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
    esp_center=[75, 25]
    r=100

    for i in range(0, 2):
        pass
    range_max=7
    for i in range(0, range_max):
        rectangle=[-r + esp_center[0], -r+esp_center[1], 2*r, 2*r]
        # pygame.draw.rect(win, (0, 0, 240), rectangle, 2)
        pygame.draw.arc(win,  (64, 143, 222), rectangle, 3*PI/2, 0, 2)
        r += 100
    esp_center=[755, 575]
    r=100
    for i in range(0, range_max):
        rectangle=[-r + esp_center[0], -r+esp_center[1], 2*r, 2*r]
        # pygame.draw.rect(win, (0, 0, 240), rectangle, 2)
        pygame.draw.arc(win,  (217, 85, 41), rectangle, PI/2, PI, 2)
        r += 100
    pygame.display.update()  # update the screen frames


def checkBeacons(esp):
    global position_adjustments
    for e in esp:
        for b in e.beacons:
            print('Master ESP:', e.uuid, 'has: ')
            print('->', b.uuid, 'distance: ', b.distance,
                  'is assigned to: ', b.esp_assigned)


def visualize_calculations(esp):
    # Plot on the screen:
    for e in esp:
        for b in e.beacons:
            if e.uuid == "A2":
                b.x=10*abs(b.distance) - 200
                b.y=50
                print(b.distance, b.x)
            elif e.uuid == "A5":
                b.x=-8.57*abs(b.distance) + 957.14
                b.y=550
            else:
                b.x=e.x + random.randint(-20, 20) + position_adjustments[0]
                b.y=e.y + random.randint(-20, 20) + position_adjustments[1]


position_adjustments=[0, 0]
win_x=900
win_y=600
client=connect_mqtt()  # I connect to mqtt broker
pygame.init()  # Inicialize pygame interface
win=pygame.display.set_mode((win_x, win_y))  # dimensions of it
# title of this shit of game
pygame.display.set_caption("House members tracker")
clock=pygame.time.Clock()
font=pygame.font.SysFont('bitstreamverasans', 30, True, True)
# Load the config file with the colors and room dimensions
# I also create global esps:
beacons=[]
esp=[]
esp.append(Esp("A5", 75, 25))
esp.append(Esp("A2", 725, 550))
# Render for the first time the window:    w
word_image=pygame.image.load('images/supermarket-floor.jpg')
word_image=pygame.transform.scale(word_image, (win_x, win_y))
reddrawGameWindow()
# Set a timer to reload the game
timer_update_screen=int(round(time.time()))
refresh_time=5
# The game loop running
run=True
while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run=False
            client.loop_stop()  # stop the loop
    # reddrawGameWindow()
    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):
        # print(beacon_data)
        checkBeacons(esp)
        visualize_calculations(esp)
        # Render the screen:
        reddrawGameWindow()
        # Delete all the info:
        for e in esp:
            e.beacons=[]
        # Reestart the timer:
        timer_update_screen=int(round(time.time()))
pygame.quit()
