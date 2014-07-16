#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Description: 
# Created on Четверг, 17 Июль 2014.
#

""" TODO: Docstring"""

import sys

def convert_hex (fname):
    print "Converting ", fname
    f_in  = open(fname, "r")
    f_out = open(fname[:-3] + "bin", "w")
    for line in f_in.readlines()[2:-1]:
        f_out.write(line[9:-3].decode("hex"))
    f_out.close()
    f_in.close()

if __name__ == '__main__':
    convert_hex(sys.argv[1])
