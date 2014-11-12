#! /bin/sh

LOGIN="root"
REMOTE_HOST="mokaddict.local"
CMD="scp -i ~/.ssh/id_rsa -r"

#$CMD mokaddict.sqlite $LOGIN@$REMOTE_HOST:/mnt/sda1/
$CMD python/* $LOGIN@$REMOTE_HOST:/mnt/sda1/python/
$CMD arduino/* $LOGIN@$REMOTE_HOST:/mnt/sda1/arduino/
$CMD web/* $LOGIN@$REMOTE_HOST:/mnt/sda1/web/