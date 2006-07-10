#!/usr/bin/bash
ulimit -n 65535
ndd -set /dev/tcp tcp_recv_hiwat 65536
ndd -set /dev/tcp tcp_xmit_hiwat 65536
ndd -set /dev/tcp tcp_conn_req_max_q 512
ndd -set /dev/tcp tcp_conn_req_max_q0 5120
ndd -set /dev/tcp tcp_fin_wait_2_flush_interval 135000
ndd -set /dev/tcp tcp_time_wait_interval 60000
ndd -set /dev/tcp tcp_naglim_def 1
ndd -set /dev/tcp tcp_keepalive_interval 30000
set rlim_fd_cur=8192
set rlim_fd_max=65536
set maxusers=4096
