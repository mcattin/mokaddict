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

import os
import sys
import datetime
import shutil
import sqlite3
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.Utils import COMMASPACE, formatdate
from email import Encoders


current_date = datetime.datetime.now().strftime("%Y-%m-%d")
current_time = datetime.datetime.now().strftime("%Hh%Mm%Ss")
db_file = "mokaddict.sqlite"
report_file = os.path.dirname(os.path.abspath(__file__))+"/../backup/mokaddict.report."+current_date+".csv"
sender = "matthieu.cattin@cern.ch"

def send_mail(send_from, send_to, subject, text, files=[]):
    assert type(send_to)==list
    assert type(files)==list

    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    msg.attach( MIMEText(text) )

    for f in files:
        part = MIMEBase('application', "octet-stream")
        part.set_payload( open(f,"rb").read() )
        Encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="%s"' % os.path.basename(f))
        msg.attach(part)

    smtp = smtplib.SMTP("smtp.gmail.com", 587)
    # identify ourselves to smtp gmail client
    smtp.ehlo()
    # secure our email with tls encryption
    smtp.starttls()
    # re-identify ourselves as an encrypted connection
    smtp.ehlo()
    smtp.login("mokaddict@gmail.com", "M0k@ddict")
    smtp.sendmail(send_from, send_to, msg.as_string())
    smtp.close()


################################################################################
# Backup database
################################################################################
db_path = os.path.dirname(os.path.abspath(__file__))+"/../"+db_file
bkp_path = os.path.dirname(os.path.abspath(__file__))+"/../backup/"+db_file+"."+current_date
shutil.copy2(db_path, bkp_path)

################################################################################
# Create summary of coffee log database per user and write to csv file
################################################################################
db = sqlite3.connect(db_path)
cur = db.cursor()
f = open(report_file, 'w')

# Get list of users
cur.execute(''' SELECT * FROM addicts ''')
users = cur.fetchall()

# Get consumption per user and write to file
f.write("Date, "+current_date+"\n")
f.write("Time, "+current_time+"\n")
f.write("User, Consumption\n")
for user in users:
    cur.execute(''' SELECT COUNT(*) FROM kfe_log WHERE rfid IN ( SELECT rfid FROM uids WHERE user_id LIKE ( SELECT id FROM addicts WHERE id LIKE ? ) ) AND auth='TRUE' ''',(user[0],))
    consumption = cur.fetchall()
    f.write(user[1]+", %d\n"%consumption[0])

f.close()

################################################################################
# Send the csv file by email
################################################################################

# Build list if users to notify
recipients = ["mokaddict@gmail.com"]
for user in users:
    if("TRUE" == user[6]):
        recipients.append(user[4])

# Send email
subject = "Coffee consumption report - %s"%current_date
body = "Hi,\nThis is an automated report.\nPlease find attached the coffee consumption report generated on:\n  %s %s\n\nYour MokAddict server!"%(current_date, current_time)
send_mail(sender, recipients, subject, body, [report_file])

################################################################################
# Clear coffee log database
################################################################################
cur.execute('''DELETE FROM kfe_log''')
db.commit()
cur.close()
db.close()


sys.exit(1)
