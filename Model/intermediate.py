import sys
import threading
import os
from os import fdopen
import time
"""
odr, odw = os.pipe()
def BACON(inp, w):
	while not w.closed:
		k = inp.read()
		w.write(k)
tmp = sys.stdin
sys.stdin = fdopen(odr)
w = fdopen(odw, 'w')
p = threading.Thread(target=BACON, args=(tmp, w))
p.start()
def TT(w):
	time.sleep(1)
	w.write("-1 -1")
	w.flush()

x = threading.Thread(target=TT, args=(w,))
x.start()
k = input()
w.close()
r.close()
print("Done")
"""

odr, odw = os.pipe()
def B(inp, w):
	while not w.closed:
		k = inp.readline()
		if not w.closed:
			w.write(k)
			w.flush()
tmp = sys.stdin
sys.stdin = fdopen(odr)

w = fdopen(odw, 'w')
p = threading.Thread(target=B, args=(tmp, w))
p.setDaemon(True)
p.start()

def C(w):
	i = 0
	while not w.closed:
		w.write("Boy " + str(i)+"\n")
		w.flush()
		time.sleep(.5)

c = threading.Thread(target=C, args=(w,))
c.setDaemon(True)
c.start()

while True:
	k = sys.stdin.readline()
	print(repr(k))
	if k == "-1 -1\n":
		break
	#print(k)

w.close()

	


