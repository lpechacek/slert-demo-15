#!/bin/bash

# Check if user cpuset is available
if [ ! -e  /sys/fs/cgroup/cpuset/user/cpuset.cpus ]; then
  echo "'user' cpuset is not available."
  echo "Create cpuset using 'cset shield --kthread=on --cpu=X-Y'."
  echo "   where x and y are valid cpu numbers for this system."
  exit 1
fi

# Usage
if test -z "$1" -o -z "$2"; then
  echo "This script will (TO BE COMPLETED LATER)."
  echo
  echo "Usage: "$(basename $0" <CPU# FOR LISTENER> <CPU# FOR PROCESSOR>")
  exit 1
fi

./network-listener &
LISTENER_PID=$!
./message-processor &
PROCESSOR_PID=$!
echo "LISTENER_PID: $LISTENER_PID"
echo "PROCESSOR_PID: $PROCESSOR_PID"
cset proc -m "$LISTENER_PID" user
taskset -cp "$1" $LISTENER_PID
cset proc -m "$PROCESSOR_PID" user
taskset -cp "$2" $PROCESSOR_PID

trap "kill $LISTENER_PID; kill $PROCESSOR_PID" EXIT

./result-logger
