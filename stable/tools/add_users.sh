#!/bin/bash

HOST=$1
if [ "$1" == "" ]; then
    HOST="localhost"
fi

for(( i=600; i<=100000; i++ )); do
    cat add_users.nc | sed -e  "s/userIdx/user$i/g" | nc $HOST 7000
done
