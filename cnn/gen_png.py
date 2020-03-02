import sys
import math
import itertools
import time
import os
from PIL import Image, ImageDraw, ImageFont

def parse_assign(f, mat_row, mat_col, num_cycle):
    assign = []
    for k in range(0, num_cycle + 1):
        assign_ = []
        for i in range(0, mat_row):
            for j in range(0, mat_col):
                assign__ = []
                assign_.append(assign__)
        assign.append(assign_)
        
    line = f.readline()
    while line:
        data = line.split()
        cycle = int(data[0])
        node = int(data[1])
        var = data[2]
        if var not in assign[cycle][node]:
            assign[cycle][node].append(var)
        line = f.readline()

    return assign

def parse_com(f, mat_row, mat_col, num_cycle, assign):
    com = []
    for k in range(0, num_cycle):
        com_ = []
        for i in range(0, mat_row * mat_col):
            com__ = []            
            for j in range(0, mat_row * mat_col):
                com___ = []
                com__.append(com___)
            com_.append(com__)
        com.append(com_)
        
    line = f.readline()
    while line:
        data = line.split()
        cycle = int(data[0])
        node_from = int(data[1])
        node_to = int(data[2])
        for i in range(3, len(data)):
            if data[i] not in com[cycle][node_from][node_to]:
                if data[i] not in assign[cycle][node_to] and data[i] in assign[cycle + 1][node_to]:
                    com[cycle][node_from][node_to].append(data[i])
        line = f.readline()

    return com

#direction ... U, R, D, L (up, right, down, left)
#size ... size of head
def write_arrow(draw, x, y, length, size, direction):
    arrow_color = (250, 107, 64)  # orange
    if direction == 'U':
        draw.line((x, y, x, y - length), fill=arrow_color, width=5)
        draw.line((x - size, y - length + size, x, y - length), fill=arrow_color, width=5)
        draw.line((x + size, y - length + size, x, y - length), fill=arrow_color, width=5)
    elif direction == 'R':
        draw.line((x, y, x + length, y), fill=arrow_color, width=5)
        draw.line((x + length - size, y - size, x + length, y), fill=arrow_color, width=5)
        draw.line((x + length - size, y + size, x + length, y), fill=arrow_color, width=5)
    elif direction == 'D':
        draw.line((x, y, x, y + length), fill=arrow_color, width=5)
        draw.line((x - size, y + length - size, x, y + length), fill=arrow_color, width=5)
        draw.line((x + size, y + length - size, x, y + length), fill=arrow_color, width=5)
    elif direction == 'L':
        draw.line((x, y, x - length, y), fill=arrow_color, width=5)
        draw.line((x - length + size, y - size, x - length, y), fill=arrow_color, width=5)
        draw.line((x - length + size, y + size, x - length, y), fill=arrow_color, width=5)

def gen_img(mat_row, mat_col, num_cycle, assign, com):
    # color
    white = (255, 255, 255)
    black = (0, 0, 0)
    block_color = 0
    white_color = (255, 255, 255)  # white
    ioblock_color = (255, 252, 204)  # yellow (226, 240, 217)  # green

    # config
    rectsize = 100
    fontsize = 20
    arrowsize = 10
    padding = rectsize
    margin = padding // 2
    img = [Image.new('RGB', ((rectsize + padding) * mat_col, (rectsize + padding) * mat_row), white) for i in range(0, num_cycle + 1)]
    draw = [ImageDraw.Draw(img[i]) for i in range(0, num_cycle + 1)]
    font = ImageFont.truetype('/usr/share/fonts/dejavu/DejaVuSerif.ttf', fontsize, encoding='utf-8')
    font2 = ImageFont.truetype('/usr/share/fonts/dejavu/DejaVuSerif.ttf', 30, encoding='utf-8')

    for k in range(0, num_cycle + 1):
#        draw[k].rectangle((0, (rectsize + padding) * mat_row * k, (rectsize + padding) * mat_col - 1, (rectsize + padding) * mat_row * (k + 1) - 1), outline=black)
        draw[k].text((0, 0), "Cycle" + str(k+1), font=font2, fill=black)
        for i in range(0, mat_row):
            for j in range(0, mat_col):
                x = margin + (rectsize + padding) * j
                y = margin + (rectsize + padding) * i
                if i == 0 or j == 0:
                    draw[k].rectangle((x, y, x + rectsize, y + rectsize), fill=ioblock_color)
                else:
                    draw[k].rectangle((x, y, x + rectsize, y + rectsize), outline=black)
                dx = 0
                dy = 0
                next_dx = 0
                for var in assign[k][j + i * mat_col]:
                    size = draw[k].textsize(var)
                    if dx + size[0] > rectsize:
                        print("oversize or too many vars in node, wrapped as ...")
                        draw[k].text((x + dx, y + dy), "...", font=font, fill=black)
                        break
                    if dy + size[1] > rectsize:
                        dx += next_dx
                        dy = 0
                        next_dx = 0
                    draw[k].text((x + dx, y + dy), var, font=font, fill=black)
                    dy += size[1]
                    if size[0] > next_dx:
                        next_dx = size[0]

    if com:
        for k in range(0, num_cycle):
            for i_from in range(0, mat_row):
                for j_from in range(0, mat_col):
                    node_from = j_from + i_from * mat_col
                    x_from = margin + (rectsize + padding) * j_from
                    y_from = margin + (rectsize + padding) * i_from
                    direction = 'U'
                    i_to = i_from - 1
                    j_to = j_from
                    if i_to >= 0:
                        node_to = j_to + i_to * mat_col
                        if len(com[k][node_from][node_to]) > 0:
                            write_arrow(draw[k], x_from + rectsize // 2 - arrowsize, y_from, padding, arrowsize, direction)
                            for var in com[k][node_from][node_to]:
                                draw[k].text((x_from - padding // 2, y_from - padding // 2), var, font=font, fill=black)
                                dy += size[1]
                    direction = 'R'
                    i_to = i_from
                    j_to = j_from + 1
                    if j_to < mat_col:
                        node_to = j_to + i_to * mat_col
                        if len(com[k][node_from][node_to]) > 0:
                            write_arrow(draw[k], x_from + rectsize, y_from + rectsize // 2 - arrowsize, padding, arrowsize, direction)
                            dy = -arrowsize * 3
                            for var in com[k][node_from][node_to]:
                                size = draw[k].textsize(var)
                                dy += -size[1]
                                if size[0] > padding or -dy > rectsize // 2:
                                    print("oversize or too many vars in communication, wrapped as ...")
                                    draw[k].text((x_from + rectsize, y_from + rectsize // 2 + dy), "...", font=font, fill=black)
                                    break
                                draw[k].text((x_from + rectsize, y_from + rectsize // 2 + dy), var, font=font, fill=black)
                    direction = 'D'
                    i_to = i_from + 1
                    j_to = j_from
                    if i_to < mat_row:
                        node_to = j_to + i_to * mat_col
                        if len(com[k][node_from][node_to]) > 0:
                            write_arrow(draw[k], x_from + rectsize // 2 + arrowsize, y_from + rectsize, padding, arrowsize, direction)
                            for var in com[k][node_from][node_to]:
                                draw[k].text((x_from + rectsize // 2 + arrowsize * 2, y_from + rectsize + padding // 2), var, font=font, fill=black)
                    direction = 'L'
                    i_to = i_from
                    j_to = j_from - 1
                    if j_to >= 0:
                        node_to = j_to + i_to * mat_col
                        if len(com[k][node_from][node_to]) > 0:
                            write_arrow(draw[k], x_from, y_from + rectsize // 2 + arrowsize, padding, arrowsize, direction)
                            dy = arrowsize * 2
                            for var in com[k][node_from][node_to]:
                                size = draw[k].textsize(var)
                                if size[0] > padding or dy > rectsize // 2:
                                    print("oversize or too many vars in communication, wrapped as ...")
                                    draw[k].text((x_from - padding, y_from + rectsize // 2 + dy), "...", font=font, fill=black)
                                    break
                                draw[k].text((x_from - padding, y_from + rectsize // 2 + dy), var, font=font, fill=black)
                                dy += size[1]

    return img

if __name__ == "__main__":
    argv = sys.argv
    if len(argv) < 5:
        print("matrix_row matrix_column num_cycle assignment_file (communication_file)")
        exit()

    mat_row = int(argv[1])
    mat_col = int(argv[2])
    num_cycle = int(argv[3])
    assign_file_name = argv[4]
    com_file_name = False
    if len(argv) == 6:
        com_file_name = argv[5]

    try:
        assign_file = open(assign_file_name, "r")
        com_file = False
        if com_file_name:
            com_file = open(com_file_name, "r")
    except:
        print("matrix_row matrix_column num_cycle assignment_file (communication_file)")
        print("assignment file (or commnication file) cannot be open")
        exit()
        
    assign = parse_assign(assign_file, mat_row, mat_col, num_cycle)
    com = False
    if com_file:
        com = parse_com(com_file, mat_row, mat_col, num_cycle, assign)
        
    img = gen_img(mat_row, mat_col, num_cycle, assign, com)
    
    try:
#        img[0].save(assign_file_name + "_2.gif", save_all=True, append_images=img[1:], optimize=False, duration=1000, loop=0)
        for i in range(0, num_cycle + 1):
            img[i].save("img" + str(i+1) + ".png")
    except:
        print("image file ([assignment file name].png) cannot be open")
        exit()
        
exit()
