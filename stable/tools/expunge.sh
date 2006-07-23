#!/bin/bash

for(( i=1; i<=1000; i++ )); do
    cat expunge.nc | sed -e  "s/userIdx/user$i/g" | nc localhost 143
done
