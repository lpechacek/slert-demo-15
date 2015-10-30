#!/bin/bash

# Check if user cpuset is available
if [ ! -e  /sys/fs/cgroup/cpuset/user/cpuset.cpus ]; then
  echo "'user' cpuset is not available."
  echo "Create cpuset using 'cset shield --kthread=on --cpu=X-Y'."
  echo "   where x and y are valid cpu numbers for this system."
  exit 1
fi

# Parse command line options
CONCURRENCY=''

while getopts c opt
do
  case $opt in
  c) CONCURRENCY=1 ;;
  *)
      echo "$0: unknown option $opt" >&2; exit 1 ;;
  esac
done

shift `expr $OPTIND - 1`

# Usage
if test -z "$1" -o -z "$2"; then
  echo "This script will (TO BE COMPLETED LATER)."
  echo
  echo "Usage: "$(basename $0" <CPU# FOR LISTENER> <CPU# FOR PROCESSOR>")
  exit 1
fi

# define useful FS paths
PROG_PATH=$(dirname "$0")
SHARED_DEMO_FILE=/dev/shm/demo_shared_file

unset NOISE_MAKER_PID

$PROG_PATH/network-listener &
LISTENER_PID=$!
EXIT_TRAP_CMD="kill $LISTENER_PID;"
if [ -n "$CONCURRENCY" ]; then
  $PROG_PATH/noise-maker "$SHARED_DEMO_FILE" &
  sleep .5 # ugly way to give noise-maker time to set up the shared file
  NOISE_MAKER_PID=$!
  EXIT_TRAP_CMD+="kill $NOISE_MAKER_PID;"
  MESSAGE_PROCESSOR_OPTS="-f $SHARED_DEMO_FILE"
fi
chrt 20 $PROG_PATH/message-processor $MESSAGE_PROCESSOR_OPTS &
PROCESSOR_PID=$!
EXIT_TRAP_CMD+="kill $PROCESSOR_PID;"
echo "LISTENER_PID: $LISTENER_PID"
echo "PROCESSOR_PID: $PROCESSOR_PID"
cset proc -m "$LISTENER_PID" user
taskset -cp "$1" $LISTENER_PID
cset proc -m "$PROCESSOR_PID" user
taskset -cp "$2" $PROCESSOR_PID
# noise maker is running on the same CPU as message processor for increased
# concurrency
if [ -n "$NOISE_MAKER_PID" ]; then
  echo "NOISE_MAKER_PID: $NOISE_MAKER_PID"
  cset proc -m "$NOISE_MAKER_PID" user
  taskset -cp "$2" $NOISE_MAKER_PID
fi

trap "$EXIT_TRAP_CMD" EXIT

$PROG_PATH/result-logger
