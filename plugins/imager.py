#!/usr/bin/python3
"""
This script generates an PNG image from a SALIS save file (or a list of saved
files), on which each pixel corresponds to a single byte on the simulation.
It's useful for identifying large scale structures and appreciating the
large-scale evolution of the world.

Tu run it, simply call it from within the ./bin/ directory inside the SALIS
main directory, such as:

../plugins/imager.py def.sim.*

Calling this will create a PNG image for each file beggining with "def.sim".
"""
import sys
from ctypes import *
from PIL import Image
files = sys.argv[1:]
salis = CDLL("libsalis.so")
salis.sm_getSize.restype = c_uint

def makeImage(iname):
	salis.s_load(bytes(iname, "utf-8"))
	lsize  = int(salis.sm_getSize() ** 0.5)
	image  = Image.new("RGB", (lsize, lsize), "black")
	pixels = image.load()

	for y in range(lsize):
		for x in range(lsize):
			addr = (y * lsize) + x
			byte = salis.sm_getByteAt(addr)
			pixels[x, y] = (byte // 4, byte // 2, byte)

	salis.s_quit()
	oname = iname + ".png"
	image.save(oname)
	print(oname + " generated")

for f in files:
	makeImage(f)
