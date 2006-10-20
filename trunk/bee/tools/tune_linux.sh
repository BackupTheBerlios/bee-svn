#!/bin/bash
#ulimit -n 65535
#ndd -set /dev/tcp tcp_recv_hiwat 65536
#ndd -set /dev/tcp tcp_xmit_hiwat 65536
#ndd -set /dev/tcp tcp_conn_req_max_q 512
#ndd -set /dev/tcp tcp_conn_req_max_q0 5120
#ndd -set /dev/tcp tcp_fin_wait_2_flush_interval 135000
#ndd -set /dev/tcp tcp_time_wait_interval 60000
#ndd -set /dev/tcp tcp_naglim_def 1
#ndd -set /dev/tcp tcp_keepalive_interval 30000
#set rlim_fd_cur=8192
#set rlim_fd_max=65536
#set maxusers=4096

#echo 300000 > /proc/sys/kernel/pid_max
echo 100000 > /proc/sys/kernel/threads-max
ulimit -s 32
ulimit -n 100000
ulimit -a
#core file size        (blocks, -c) 0
#data seg size         (kbytes, -d) unlimited
#file size             (blocks, -f) unlimited
#max locked memory     (kbytes, -l) unlimited
#max memory size       (kbytes, -m) unlimited
#open files                    (-n) 100000
#pipe size          (512 bytes, -p) 8
#stack size            (kbytes, -s) 32
#cpu time             (seconds, -t) unlimited
#max user processes            (-u) 100000
#virtual memory        (kbytes, -v) unlimited
