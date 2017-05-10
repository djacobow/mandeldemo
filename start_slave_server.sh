#!/bin/sh

cd ~/mandeldemo
nohup ./slave_server.js > /dev/null < /dev/null 2>&1  &
exit


