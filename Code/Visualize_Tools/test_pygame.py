import pygame
import math

pygame.init()
win = pygame.display.set_mode((500, 500))
clock = pygame.time.Clock()
# The correct method to solve for intersection points of two circles is algebraically.
# NOT using points (x, y coordinates) because of infinite precision of coordinate system (real numbers).


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


h, k = 200, 200
r = 100
run = True
while run:
    clock.tick(60)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        if event.type == pygame.KEYDOWN:
            print(pygame.key.name(event.key))

    win.fill((66, 67, 69))
    x0, y0 = 0, 0
    r0 = 5
    x1, y1 = 2, 2
    r1 = 5

    # intersecting with (x1, y1) but not with (x0, y0)
    x2, y2 = -1, 0
    r2 = 2.5

    circle1 = plt.Circle((x0, y0), r0, color='b', fill=False)
    circle2 = plt.Circle((x1, y1), r1, color='b', fill=False)
    circle3 = plt.Circle((x2, y2), r2, color='b', fill=False)

    fig, ax = plt.subplots()
    ax.set_xlim((-10, 10))
    ax.set_ylim((-10, 10))
    ax.add_artist(circle1)
    ax.add_artist(circle2)
    ax.add_artist(circle3)

    intersections = get_intersections(x0, y0, r0, x1, y1, r1)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections
        plt.plot([i_x3, i_x4], [i_y3, i_y4], '.', color='r')

    intersections = get_intersections(x0, y0, r0, x2, y2, r2)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections
        plt.plot([i_x3, i_x4], [i_y3, i_y4], '.', color='r')

    intersections = get_intersections(x1, y1, r1, x2, y2, r2)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections
        plt.plot([i_x3, i_x4], [i_y3, i_y4], '.', color='r')

    plt.gca().set_aspect('equal', adjustable='box')

    pygame.display.flip()
