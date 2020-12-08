import random
import pygame 
import math

class Beacon:
    def __init__(self, x, y, uuid, distance):
        self.x = x
        self.y = y
        self.uuid = uuid
        self.distance = distance
        self.filter_distance = [];
        self.color = (random.randint(10, 255), random.randint(
            10, 255), random.randint(10, 255))
        self.esp_assigned = "nop"

    def draw(self, win):
        self.x = int(self.x)
        self.y = int(self.y)
        pygame.draw.circle(win, (0, 0, 0), (self.x, self.y), 18)
        pygame.draw.circle(win,  self.color, (self.x, self.y), 15)
    def draw_range(self, win, center, radio):
        pygame.draw.circle(win, (0, 0, 0), (self.x, self.y), radio)
        pygame.draw.circle(win,  self.color, (self.x, self.y), 15)


class Esp:
    def __init__(self, uuid, x, y):
        self.uuid = uuid
        self.beacons = []
        self.x = x
        self.y = y

    def add_beacon(self, beacon):
        self.beacons.append(beacon)
    def get_position(self):
        pos = [self.x, self.y]
        return pos
    def rssr_distance(self, rssi, txCalibratedPower):
        ratio_db = txCalibratedPower - rssi
        ratio_linear = 10**(ratio_db / 10)
        r = math.sqrt(ratio_linear)
        return r

class Beacon_Data():
    def __init__(self, name, rssi, esp):
        self.name = name
        self.esp = []
        self.rssi = []
        self.esp.append(esp)
        self.rssi.append(rssi)
        print("NEW Beacon: " + self.name)

    def print_itself(self):
        print(self.name)
        for i in range(0, len(self.esp)):
            print("-> ", self.esp[i], " - ", str(self.rssi[i]))
        print("___________________________________________ ")

    def add_rssi(self, esp, rssi):
        self.esp.append(esp)
        self.rssi.append(rssi)

    def update_rssi(self, new_rssi, position):
        self.rssi[position] = new_rssi

def get_intersections(x0, y0, r0, x1, y1, r1):
    # circle 1: (x0, y0), radius r0
    # circle 2: (x1, y1), radius r1
    d = math.sqrt((x1-x0)**2 + (y1-y0)**2)
    # non intersecting
    if d > r0 + r1:
        return None
    # One circle within other
    if d < abs(r0-r1):
        return None
    # coincident circles
    if d == 0 and r0 == r1: 
        return None
    else:
        a = (r0**2-r1**2+d**2)/(2*d)
        h = math.sqrt(r0**2-a**2)
        x2 = x0+a*(x1-x0)/d
        y2 = y0+a*(y1-y0)/d
        x3 = x2+h*(y1-y0)/d
        y3 = y2-h*(x1-x0)/d
        x4 = x2-h*(y1-y0)/d
        y4 = y2+h*(x1-x0)/d
        return (x3, y3, x4, y4)