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

# Checks if the user (RFID UID) passed in parameter is registered in the database

import os
import sys
import sqlite3
import re


while True:
    db_path = os.path.dirname(os.path.abspath(__file__))+'/../'
    rfid = sys.argv[1]

    if not(re.match("^[A-Z0-9]{8}$", rfid) or re.match("^[A-Z0-9]{14}$", rfid)):
        print 3
        sys.exit(1)

    try:
        db = sqlite3.connect(db_path+'mokaddict.sqlite')
        cur = db.cursor()

        cur.execute('''SELECT * FROM uids WHERE rfid=? AND authorised='TRUE' ''',(rfid,))
        res = cur.fetchall()

        if res:
            # print res
            print 1
            sys.exit(1)
        else:
            # print 'No user with UID=%s'%rfid
            print 2
            sys.exit(1)
    except Exception as e:
        print str(e)
        sys.exit(1)

