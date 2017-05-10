#!/bin/sh

cd ~/mandeldemo
nohup ./slave_server.js 2>&1 < /dev/null &


