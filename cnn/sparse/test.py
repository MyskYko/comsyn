import os
import itertools

l = [1,2,3,4,5,6,7,8]

for i in range(1, 9):
    print("sparsity " + str(i))
    cmd = "mkdir s" + str(i)
    os.system(cmd)
    os.chdir("s" + str(i))
    a = list(itertools.combinations(l, i))
    for j in range(9 - i, 10):
        stime = 0
        ncase = 0
        nsuccess = 0
        cmd = "mkdir t" + str(j)
        os.system(cmd)
        os.chdir("t"+str(j))
        for b in a:
            s = ""
            for c in b:
                s += "_"
                s += str(c)
            cmd = "mkdir s" + s
            os.system(cmd)
            os.chdir("s" + s)
            cmd = "python ../../../../gen_cnn_setting.py 6 6 3 3 "
            cmd += str(j)
            cmd += " 2 2 1 onehot_spx_in systolic sparse"
            cmd += s
            os.system(cmd)
            cmd = "gen_cnf setting.txt"
            os.system(cmd)
            f = open("_setting.txt.log")
            lines = f.readlines()
            for line in lines:
                words = line.split()
                if words and words[0] == "CPU":
                    stime += float(words[3])
                if words and words[0] == "SATISFIABLE":
                    nsuccess += 1
            ncase += 1
            os.chdir("..")
        avetime = stime / ncase
        print("cycle " + str(j) + " rate " + str(nsuccess) + " / " + str(ncase) + " time " + str(avetime))
        os.chdir("..")
    os.chdir("..")
