#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Description: Convert scheme hex format(intel hex?) to pure binary
# representation.
# Created on Четверг, 17 Июль 2014.
#

""" Convert scheme hex format(intel hex?) to pure binary representation. """

import sys

def convert_hex (fname):
    bin_fname = fname[:-3] + "bin"
    print "Converting", fname, "to", bin_fname
    f_in  = open(fname, "r")
    f_out = open(bin_fname, "w")
    # Skip 2 first lines(segment entry, ?) and last line(hex ending line)
    for line in f_in.readlines()[2:-1]:
        # Skip ':', len(2b), addr(4b), type(2b) at the beginning of the entry
        # and checksum at the end
        f_out.write(line[9:-3].decode("hex"))
    f_out.close()
    f_in.close()


# hex filename as argument, will create 2nd file of binary format in the same
# directory as provided file, naming convention as follows:
# dir/{filename}.hex -> dir/{filename}.bin
if __name__ == '__main__':
    convert_hex(sys.argv[1])
