#!/usr/bin/python
'''
This script generates a data file that can be read by gnuplot or similar
software. Columns, representing different data fields, may be generated via
command line arguments. Current available columns include:

- current cycle:       always present
- number of organisms: activate with '-o' flag

To run it, simply call it from within the ./bin/ directory inside the SALIS
main directory, such as:

../plugins/grapher.py -o def.sim.*.auto

Calling this will create a single DATA file including data from all files
matching the given pattern. This file may, in turn, be passed into gnuplot
for generating graphs.
'''
import os
import sys
from ctypes import *

current_arg = 1
data_flags  = []

while sys.argv[current_arg][0] == '-':
	if sys.argv[current_arg] == '-o':
		data_flags.append('organism_count')
		print('organism count will be graphed')
	else:
		print('{} is not a valid flag'.format(sys.argv[current_arg]))

	current_arg += 1

files = sys.argv[current_arg:]
oname = 'output.data'
salis = CDLL('libsalis.so')
salis.s_getCycle.restype      = c_uint
salis.s_getEpoch.restype      = c_uint
salis.sp_getCount.restype = c_uint

with open(oname, 'w') as output:
	for f in files:
		salis.s_load(bytes(f, 'utf-8'))
		cycle = salis.s_getCycle()
		epoch = salis.s_getEpoch()
		date  = cycle + (epoch * (2 ** 32))
		output.write('{} {}\n'.format(date, salis.sp_getCount()))
		print('info of date {} added to output.data'.format(date))
		salis.s_quit()
