#!/usr/bin/zsh 

pid=`echo $$`
./log_client "Msg from $pid"

sleep 2

./log_client "Msg from $pid"