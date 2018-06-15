#!/usr/bin/python
'''
This script generates a PNG image from a SALIS save file (or a list of saved
files), on which each pixel corresponds to a single byte on the simulation.
It's useful for identifying large scale structures and appreciating the
macro-evolution of the world.

Tu run it, simply call it from within the ./bin/ directory inside the SALIS
main directory, such as:

../plugins/imager.py def.sim.*.auto

Calling this will create a PNG image for each file matching the given pattern.

By default, this script ignores any existing simulation files whose name
matches an existing image on the same directory. You may force it to overwrite
existing images by passing the '-o' flag before the file list parameter.
'''
import os
import sys
from ctypes import *
from PIL import Image

if sys.argv[1] == '-o':
	overwrite = True
	files     = sys.argv[2:]
else:
	overwrite = False
	files     = sys.argv[1:]

salis = CDLL('libsalis.so')
salis.sm_getSize.restype = c_uint

def makeImage(iname):
	oname = iname + '.png'

	if not overwrite:
		if os.path.isfile('./' + oname):
			print(oname + ' skipped')
			return

	salis.s_load(bytes(iname, 'utf-8'))
	lsize  = int(salis.sm_getSize() ** 0.5)
	image  = Image.new('L', (lsize, lsize), 'black')
	pixels = image.load()

	for y in range(lsize):
		for x in range(lsize):
			addr = (y * lsize) + x
			byte = salis.sm_getByteAt(addr)
			pixels[x, y] = byte

	salis.s_quit()
	image.save(oname)
	print(oname + ' generated')

for f in files:
	makeImage(f)
