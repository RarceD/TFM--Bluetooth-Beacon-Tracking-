import pygame
import random
import paho.mqtt.client as mqtt
import time
import json


def reddrawGameWindow():
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

def rssr_distance(rssi, tx, n):
    return 10**((tx-rssi)/n)

run = True  # The game loop running
beacons = []
esp = []
position_adjustments = [0,0]

win_x = 700
win_y = 500

pygame.init()  # Inicialize pygame interface
win = pygame.display.set_mode((win_x, win_y - 20))  # dimensions of it
pygame.display.set_caption("Rolling tracker")  # title of this shit of game
clock = pygame.time.Clock()
bg = pygame.image.load('grass.jpg')
font = pygame.font.SysFont('bitstreamverasans', 30, True, True)


timer_update_screen = int(round(time.time()))
refresh_time = 6
reddrawGameWindow()
# I create the beacons:

# I fake create the masters with their beacons
# esp[0].beacons.append(
#     Beacon(random.randint(10, 699), 260, "test", 1.43))
# esp[0].beacons.append(
#     Beacon(random.randint(10, 699), 260, "no", 2.43))
# esp[1].beacons.append(
#     Beacon(random.randint(10, 699), 260, "si", 2.43))
# esp[1].beacons.append(
#     Beacon(random.randint(10, 699), 260, "test", 1.43))



while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run = False


    # Every X seconds I update the position of the screen:
    if (int(round(time.time())) - timer_update_screen >= refresh_time):


        timer_update_screen = int(round(time.time()))


pygame.quit()
