#!/bin/bash

for(( i=1; i<=100000; i++ )); do
    cat expunge.nc | sed -e  "s/userIdx/user$i/g" | nc gigi 143
done
