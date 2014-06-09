<<<<<<< HEAD
import os
import commands

fpath = "make.txt"

f = open(fpath, "w")

a = os.popen("make").readlines()
f.write(a)

=======
import os
import commands

fpath = "make.txt"

f = open(fpath, "w")

a = os.popen("make").readlines()
f.write(a)

>>>>>>> 7106fe59abf3eb268a3817f0055ea152547157e9
f.close()