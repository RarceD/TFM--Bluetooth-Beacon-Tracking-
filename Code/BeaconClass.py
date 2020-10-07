import random
import pygame 

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
        pygame.draw.circle(win, (0, 0, 0), (self.x, self.y), 18)
        pygame.draw.circle(win,  self.color, (self.x, self.y), 15)


class Esp:
    def __init__(self, uuid, x, y):
        self.uuid = uuid
        self.beacons = []
        self.x = x
        self.y = y

    def add_beacon(self, beacon):
        self.beacons.append(beacon)