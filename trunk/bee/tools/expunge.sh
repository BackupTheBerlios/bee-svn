#!/bin/bash
EXPUNGE="a login userIdx userIdx\r\na select inbox\r\na store 1:* +flags (\deleted)\r\na expunge\r\na logout\r\n"

for(( i=1; i<=100000; i++ )); do
    echo ${EXPUNGE} | sed -e  "s/userIdx/user$i/g" | nc gigi 143
done
