#! /usr/bin/python

"""
Copyright Matthieu Cattin 2014

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

# Write RFID UID to a text file

import os
import sys
import re

rfid = sys.argv[1]
file_path = os.path.dirname(os.path.abspath(__file__))+'/rfid.txt'


if not(re.match("^[A-Z0-9]*$", rfid)):
    print 2
    sys.exit(1)
else:
    f = open(file_path,'w')
    f.write(rfid)
    f.close()
    print 1
    sys.exit(1)
