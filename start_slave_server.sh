#!/bin/sh

cd ~/mandeldemo
echo "Starting slave server..."
nohup ./slave_server.js > /dev/null < /dev/null 2>&1  &
echo "slave server (probably) started."
exit


