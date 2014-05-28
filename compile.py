import os
import commands

fpath = "make.txt"

f = open(fpath, "w")

a = os.popen("make").readlines()
f.write(a)

f.close()