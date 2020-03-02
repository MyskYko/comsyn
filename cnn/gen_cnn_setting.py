import sys
import math
import itertools
import time
import os

argv = sys.argv
if len(argv) < 9:
    print("matrix_row matrix_column window_row window_column cycle pad_row pad_col stride (option)")
    exit()

mat_row = int(argv[1])
mat_col = int(argv[2])
win_row = int(argv[3])
win_col = int(argv[4])
min_cycle = int(argv[5])
pad_row = int(argv[6])
pad_col = int(argv[7])
stride = int(argv[8])
options = []
if len(argv) > 9:
    options = argv[9:]
options.sort()

#file_name = "cnn_m" + str(mat_row) + "x" + str(mat_col) + "_w" + str(win_row) + "x" + str(win_col) + "_c" + str(min_cycle) + "_p" + str(pad_row) + "x" + str(pad_col) + "_s" + str(stride);
#for option in options:
#    file_name += "_" + option
#file_name += ".txt"
file_name = "setting.txt"
#print(file_name)
f = open(file_name, "w")

f.write("cycle\n" + str(min_cycle) + "\n")

num_node = (mat_row + pad_row) * (mat_col + pad_col)
f.write("\nnode\n" + str(num_node) + "\n")

f.write("\nspx\n")
for i in range(0, num_node):
    for j in range(0, num_node):
        if (i % (mat_col + pad_col) != mat_col + pad_col - 1 and j - i == 1) or (i // (mat_col + pad_col) != mat_row + pad_row - 1 and j - i == mat_col + pad_col) or (i % (mat_col + pad_col) != 0 and i - j == 1) or (i // (mat_col + pad_col) != 0 and i - j == mat_col + pad_col):
            f.write("1 ")
        else:
            f.write("0 ")
    f.seek(f.tell() - 1)
    f.write("\n")

f.write("\ninitial\n")
for i in range(0, mat_row + pad_row):
    for j in range(0, mat_col + pad_col):
        if i < pad_row or j < pad_col:
            f.write("\n")
            continue
        f.write("I[" + str(i - pad_row + 1) + "][" + str(j - pad_col + 1) + "]\n")

weight = []
for option in options:
    a = option.split("_")
    if a[0] == "sparse":
        weight = [[1 for i in range(0, win_col)] for j in range(0, win_row)]
        for b in a[1:]:
            weight[int(b) // win_col][int(b) % win_col] = 0
        
if weight:
    f.write("\nfinal\n")
    out_row = -(-mat_row // stride)
    out_col = -(-mat_col // stride)
    start_row = pad_row + (mat_row - out_row) // 2
    start_col = pad_col + (mat_col - out_col) // 2
    end_row = start_row + out_row
    end_col = start_col + out_col
    for i in range(0, mat_row + pad_row):
        for j in range(0, mat_col + pad_col):
            if i < start_row or j < start_col or i >= end_row or j >= end_col:
                f.write("\n")
                continue
            pos = f.tell()
            for k in range(0, win_row):
                for l in range(0, win_col):
                    if weight[k][l] == 1:
                        if (i - start_row) * stride + k < mat_row and (j - start_col) * stride + l < mat_col:
                            f.write("I[" + str((i - start_row) * stride + k + 1) + "][" + str((j - start_col) * stride + l + 1) + "] ")
                            pos = f.tell() - 1
            f.seek(pos)
            f.write("\n")
else:
    f.write("\nfinal\n")
    out_row = -(-mat_row // stride)
    out_col = -(-mat_col // stride)
    start_row = pad_row + (mat_row - out_row) // 2
    start_col = pad_col + (mat_col - out_col) // 2
    end_row = start_row + out_row
    end_col = start_col + out_col
    for i in range(0, mat_row + pad_row):
        for j in range(0, mat_col + pad_col):
            if i < start_row or j < start_col or i >= end_row or j >= end_col:
                f.write("\n")
                continue
            pos = f.tell()
            for k in range(0, win_row):
                for l in range(0, win_col):
                    if (i - start_row) * stride + k < mat_row and (j - start_col) * stride + l < mat_col:
                        f.write("I[" + str((i - start_row) * stride + k + 1) + "][" + str((j - start_col) * stride + l + 1) + "] ")
                        pos = f.tell() - 1
            f.seek(pos)
            f.write("\n")

        
for option in options:
    f.write("\n" + option + "\n")

