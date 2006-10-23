#!/bin/bash

HOST=$1
if [ "$1" == "" ]; then
    HOST="localhost"
fi
ADDUSERS="user admin\r\ncucu\r\nupdate domain name localdomain\r\nadd account name userIdx passwd userIdx\r\ncommit\r\ncommit\r\nquit\r\n"

for(( i=1; i<=100000; i++ )); do
    echo "-------- Adding user $i --------"
    echo -e ${ADDUSERS} | sed -e  "s/userIdx/user$i/g" | nc $HOST 7000
    echo "-------- done --------"
done
