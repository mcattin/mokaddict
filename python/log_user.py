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

# Log RFID UID and timestamp to database

import os
import sys
import sqlite3
import time
import datetime

#timestamp = datetime.datetime.now()
timestamp = time.strftime("%Y-%m-%d")
# print timestamp

db_path = os.path.dirname(os.path.abspath(__file__))+'/../'
rfid = sys.argv[1]
auth = sys.argv[2]
# print rfid

try:
    db = sqlite3.connect(db_path+'mokaddict.sqlite')
    cur = db.cursor()

    cur.execute('''INSERT INTO kfe_log(date,rfid,auth) VALUES (?,?,?)''',(timestamp, rfid, auth,))

    db.commit()
    db.close()

    print 1
    sys.exit(1)

except Exception as e:
    print str(e)
    sys.exit(1)
