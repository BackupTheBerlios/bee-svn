#!/bin/bash

for(( i=1; i<=1000; i++ )); do
    cat add_users.nc | sed -e  "s/userIdx/user$i/g" | nc localhost 7000
done
