import pygame
import random

pygame.init()

win_x = 700
win_y = 500
win = pygame.display.set_mode((win_x, win_y - 20))  # dimensions of it
pygame.display.set_caption("Rolling tracker")  # title of this shit of game
clock = pygame.time.Clock()  
bg = pygame.image.load('road.jpg')
font = pygame.font.SysFont('bitstreamverasans', 30, True, True)


class Beacon:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def draw(self, win):
        pygame.draw.circle(win, (0, 0, 0), (self.x, self.y), 18)
        pygame.draw.circle(win, (217, 26, 26), (self.x, self.y), 15)


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
    beacon.draw(win)
    # pygame.draw.rect(win, (0, 0, 0), (550, 10, 120, 40))
    # pygame.draw.rect(win, (255, 0, 0), (555, 15, 110, 30))
    pygame.display.update()  # update the screen frames


run = True  # The game loop running
beacon = Beacon(300, 260)
while(run):
    pygame.time.delay(50)  # 64x64 images
    for event in pygame.event.get():  # Check for events of close
        if event.type == pygame.QUIT:
            run = False
    reddrawGameWindow()  # For drawing all the canvas
pygame.quit()
