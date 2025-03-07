#!/bin/sh

# Start
start(){
    echo "Starting aesdsocket..."
    start-stop-daemon --start --name aesdsocket --exec usr/bin/aesdsocket -- "-d"
}

#Stop
stop(){
    echo "Stoping aesdsocket..."
    start-stop-daemon --stop --signal TERM --name aesdsocket
}

case "$1" in
start)
    start
    ;;
stop)
    stop
    ;;
restart)
    stop
    start
    ;;
*)

echo "Usage $0 {start|stop|restart}"

exit 1

esac

exit 0