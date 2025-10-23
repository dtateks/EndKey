#!/bin/bash

# Script to restart EndKey after granting permissions
echo "Restarting EndKey..."

# Kill existing processes
killall EndKey EndKeyHelper 2>/dev/null
sleep 2

# Launch with slight delay
sleep 1
open "/Applications/EndKey.app"

# Kill old background process and launch new one
sleep 2
killall EndKey 2>/dev/null
/Applications/EndKey.app/Contents/MacOS/EndKey &

echo "EndKey restarted. Please test Vietnamese typing now."
