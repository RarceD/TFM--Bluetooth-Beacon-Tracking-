import pygame
import random
import paho.mqtt.client as mqtt
import time
import json
import sys
import math
from BeaconClass import Beacon, Esp

PROPORTIONALITY = 250 / 3.3
PI = 3.141592653589793


class Room:
    def __init__(self, name, pos_x, pos_y, width, length, esp_name, esp_x, esp_y, color):
        self.name = str(name)
        self.pos_x = pos_x
        self.pos_y = pos_y
        self.width = width
        self.length = length
        self.esp = Esp(esp_name, esp_x, esp_y)
        self.color = color

    def plot(self, win):
        # Plot the room:
        pygame.draw.rect(win, (0, 0, 0),
                         (self.pos_x, self.pos_y, self.width, self.length))
        pygame.draw.rect(win, self.color,
                         (self.pos_x + 5, self.pos_y + 5, self.width - 5*2, self.length-5*2))
        # Plot the Esp:
        x = self.esp.get_position()
        pygame.draw.rect(win, (0, 0, 0), (x[0], x[1], 25, 25))
        pygame.draw.rect(win, (255, 0, 53),
                         (self.esp.x + 5, self.esp.y+5, 25-5*2, 25-5*2))
        # Plot the text:
        x = font.render(self.name, 1, (0, 0, 0))
        medium_x = abs(self.width - self.pos_x)
        medium_y = abs(self.length - self.pos_y)
        win.blit(x, (self.pos_x + medium_x/2 - len(self.name)
                     * 4, self.pos_y + round(self.length/2) - 30))


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
    # print("message topic=",message.topic)
    # print("message retain flag=",message.retain)
    # Example json: {"esp":"A1","beacon":[ {"uuid":5245,"distance":1.23},{"uuid":52345, "distance":1.23 }]}
    msg = str(message.payload.decode("utf-8"))
    # print("message received: ", msg)
    parsed_json = json.loads(msg)
    global esp
    for e in esp:
        if (parsed_json['esp'] == e.uuid):
            # print(parsed_json['esp'])
            for index, b in enumerate(parsed_json['beacon']):
                # Get the distance and the uuid of the beacon:
                beacon_distance = float(
                    parsed_json['beacon'][index]['distance'])
                # beacon_distance = e.rssr_distance(beacon_distance, -69)
                beacon_uuid = str(parsed_json['beacon'][index]['uuid'])

                # Add the beacon to the master if it is not repeat:
                # print('Checking if ', beacon_uuid, 'in the list ')
                all_beacons = []
                for s in e.beacons:
                    all_beacons.append(s.uuid)

                if (len(e.beacons) == 0):
                    e.beacons.append(
                        Beacon(150, 400, beacon_uuid, beacon_distance))
                    # print('First beacon created: ', e.beacons[0].uuid)
                else:
                    if (beacon_uuid in all_beacons):
                        pass
                    else:
                        e.beacons.append(
                            Beacon(150, 400, beacon_uuid, beacon_distance))


def load_file(esp):
    rooms = []
    try:
        file = open("input.txt", "r+")
        inputFile = file.readlines()
        file.close()
        room_colors = []
        esp_coordinates = []
        room_coordinates = []
        for line in range(1, 9):
            color = inputFile[line][1:-2]
            res = eval(color)
            room_colors.append(res)
        for line in range(11, 18):
            pos = inputFile[line][1:-2]
            coordinates = pos.split()
            esp_coordinates.append(coordinates)
        for line in range(20, 27):
            pos = inputFile[line][1:-2]
            coordinates = pos.split()
            room_coordinates.append(coordinates)
        # Create all the rooms:
        for i in range(0, 7):
            rooms.append(Room(room_coordinates[i][4],
                              int(room_coordinates[i][0]),
                              int(room_coordinates[i][1]),
                              int(room_coordinates[i][2]),
                              int(room_coordinates[i][3]),
                              esp_coordinates[i][2],
                              int(esp_coordinates[i][0]),
                              int(esp_coordinates[i][1]), room_colors[i]))
            esp.append(Esp(esp_coordinates[i][2],
                           int(esp_coordinates[i][0]),
                           int(esp_coordinates[i][1])))

        return rooms
    except:
        print("No se ha cargado el archivo")


def reddrawGameWindow():
    win.fill((145, 213, 250))
    # I set a grid to better positioning
    l_x, l_y = 0, 0
    debug = True
    while (debug and (l_x <= win_x or l_y <= win_y)):
        pygame.draw.line(win, (134, 136, 143), (l_x, 0), (l_x, win_y))
        pygame.draw.line(win, (134, 136, 143), (0, l_y), (win_x, l_y))
        l_y += 50
        l_x += 50

    for r in rooms:
        r.plot(win)

    for e in esp:
        for b in e.beacons:
            b.draw(win)
    esp_center = [50, 400]
    rectangle = [100, 400, 150, 150]
    # pygame.draw.rect(win, (0,0,240),rectangle , 2)
    # pygame.draw.arc(win,  (223,0,0), rectangle, 3*PI/2, 0, 10)
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
                b.x = 4.2857*abs(b.distance)-28.571
                b.y = 275
            elif e.uuid == "A3":
                b.x = 4.2857*abs(b.distance)-28.571
                b.y = 475
            elif e.uuid == "A5":
                b.x = -3.5714*abs(b.distance)+807.14
                b.y = 125
            else:
                b.x = e.x + random.randint(-20, 20) + position_adjustments[0]
                b.y = e.y + random.randint(-20, 20) + position_adjustments[1]


position_adjustments = [0, 0]
win_x = 750
win_y = 900
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
rooms = load_file(esp)
# Render for the first time the window:
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
        checkBeacons(esp)
        visualize_calculations(esp)
        # Render the screen:
        reddrawGameWindow()
        # Delete all the info:
        for e in esp:
            e.beacons = []
        # Reestart the timer:
        timer_update_screen = int(round(time.time()))
pygame.quit()
