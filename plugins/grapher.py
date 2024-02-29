#!/usr/bin/python
'''
This script generates a data file that can be read by gnuplot or similar
software. The first column represent the date (cycle * epoch) of the Salis
save file. The second column represents the number of organisms at that given
dat.

To run it, you may call it from within the ./bin/ directory (or wherever you
keep your auto-saved files) inside the SALIS main directory, such as:

../plugins/grapher.py def.sim.*.auto

Calling this will create a single DATA file (output.data) including data from
all files matching the given pattern. This file may, in turn, be passed into
gnuplot for generating graphs.
'''
import os
import sys
from ctypes import *

oname = 'output.data'
salis = CDLL('libsalis.so')
salis.s_getCycle.restype  = c_uint
salis.s_getEpoch.restype  = c_uint
salis.sp_getCount.restype = c_uint

with open(oname, 'w') as output:
	for fileName in sys.argv[1:]:
		salis.s_load(bytes(fileName, 'utf-8'))
		cycle = salis.s_getCycle()
		epoch = salis.s_getEpoch()
		date  = cycle + (epoch * (2 ** 32))
		output.write('{} {}\n'.format(date, salis.sp_getCount()))
		print('data of date {} added to \'output.data\''.format(date))
		salis.s_quit()
