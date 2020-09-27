import pygame
import random
import paho.mqtt.client as mqtt
import time
import json


class Beacon:
    def __init__(self, x, y, uuid, distance):
        self.x = x
        self.y = y
        self.uuid = uuid
        self.distance = distance
        self.color = (random.randint(10, 255), random.randint(
            10, 255), random.randint(10, 255))

    def draw(self, win):
        pygame.draw.circle(win, (0, 0, 0), (self.x, self.y), 18)
        pygame.draw.circle(win,  self.color, (self.x, self.y), 15)


class Esp:
    def __init__(self, uuid):
        self.uuid = uuid
        self.beacons = []

    def add_beacon(self, beacon):
        self.beacons.append(beacon)


def connect_mqtt():
    broker_address = "broker.mqttdashboard.com"
    client = mqtt.Client("asdf1234asdf1234asdf")  # create new instance
    client.on_message = on_message  # attach function to callback
    print("connecting to broker")
    client.connect(broker_address)  # connect to broker
    print("Subscribing to topic", "master_beacon")
    client.subscribe("master_beacon")
    print("Publishing message to topic", "master_beacon_ack")
    client.publish("master_beacon/ack", "ok")
    client.loop_start()  # start the loop
    return client


def on_message(client, userdata, message):
    # print("message topic=",message.topic)
    # print("message retain flag=",message.retain)
    # Example json: {"esp":"A1","beacon":[ {"uuid":5245,"distance":1.23},{"uuid":52345, "distance":1.23 }]}
    msg = str(message.payload.decode("utf-8"))
    print("message received: ", msg)
    parsed_json = (json.loads(msg))
    global esp
    for e in esp:
        if (parsed_json['esp'] == e.uuid):
            # print(parsed_json['esp'])
            for index, b in enumerate(parsed_json['beacon']):
                # Get the distance and the uuid of the beacon:
                beacon_distance = float(
                    parsed_json['beacon'][index]['distance'])
                beacon_uuid = str(parsed_json['beacon'][index]['uuid'])
                # Add the beacon to the master if it is not repeat:
                # print(index)
                size_of_beacons = len(e.beacons)
                if (size_of_beacons == 0):
                    e.beacons.append(
                        Beacon(0, 0, beacon_uuid, beacon_distance))
                    print('First beacon created: ', e.beacons[0].uuid)
                else:
                    i = 0
                    while i < size_of_beacons:
                        print('Element:', i, beacon_uuid, e.beacons[i].uuid)
                        if (beacon_uuid != str(e.beacons[i].uuid)):
                            e.beacons.append(
                                Beacon(0, 0, beacon_uuid, beacon_distance))
                            print('I create a beacon:', e.beacons[i].uuid)
                            break
                        i += 1
        b = 0
        while (b < len(e.beacons)):
            print('Beacon nº', b, 'name:', e.beacons[b].uuid)
            b += 1

            # e.add_beacon(0,0,parsed_json['beacon'], 0)

    # I parse the msg and received the info:
    beacons.append(Beacon(300, 260, "E2:23:6A:54", 2.43))
    beacons.append(Beacon(400, 260, "A3:57:98:2C", 2.43))

    beacons[0].x = 500


run = True  # The game loop running
beacons = []
esp = []

esp.append(Esp("A1"))
esp.append(Esp("A2"))
esp.append(Esp("A3"))

win_x = 700
win_y = 500
client = connect_mqtt()  # I connect to mqtt broker
pygame.init()  # Inicialize pygame interface
win = pygame.display.set_mode((win_x, win_y - 20))  # dimensions of it
pygame.display.set_caption("Rolling tracker")  # title of this shit of game
clock = pygame.time.Clock()
bg = pygame.image.load('road.jpg')
font = pygame.font.SysFont('bitstreamverasans', 30, True, True)


def reddrawGameWindow():

    # always print first the background
    img_pos = 200
    win.fill((255, 255, 255))
    win.blit(bg, (255, img_pos))
    win.blit(bg, (0, img_pos))

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


timer_update_screen = int(round(time.time()))
refresh_time = 3
reddrawGameWindow()
# I create the beacons:

# I fake create the masters with their beacons
esp[0].beacons.append(
    Beacon(random.randint(10, 699), 260, "E2:23:6A:54", 1.43))
esp[0].beacons.append(
    Beacon(random.randint(10, 699), 260, "V2:23:6A:54", 2.43))
esp[1].beacons.append(
    Beacon(random.randint(10, 699), 260, "V2:23:6A:54", 1.43))
esp[1].beacons.append(
    Beacon(random.randint(10, 699), 260, "32:34:5A:22", 2.43))
esp[2].beacons.append(
    Beacon(random.randint(10, 699), 260, "V3:23:6A:54", 1.43))
esp[2].beacons.append(
    Beacon(random.randint(10, 699), 260, "C2:34:5A:22", 2.43))
for e in esp:
    print('Master ESP:', e.uuid, 'has: ')
    for b in e.beacons:
        print('->', b.uuid, 'distance: ', b.distance)

while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run = False
            client.loop_stop()  # stop the loop

    reddrawGameWindow()  # For drawing all the canvas
    # Every 5 seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):
        # First I copy all the beacons in a local list:
        is_in_esp = []
        local_beacon = []
        for e in esp:
            for b in e.beacons:
                if any(b.uuid in s for s in local_beacon):
                    pass
                else:
                    local_beacon.append(b.uuid)
        print('______The scan results______')
        for b in local_beacon:
            print(b)
            is_in_esp.append('')
        # Search for each beacon if is in more than one esp:
        for b in local_beacon:
            for e in esp:
                for x in e.beacons:
                    if (b == x.uuid):
                        print('Is in ESP', e.uuid)
                        # is_in_esp[]
        timer_update_screen = int(round(time.time()))


pygame.quit()
