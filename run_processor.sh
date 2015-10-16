#!/bin/bash

# CPUs 1, 7, 13, 19 have been isolated
taskset -c 1 ./network-listener &
LISTENER_PID=$!
taskset -c 7 ./message-processor &
PROCESSOR_PID=$!
trap "kill $LISTENER_PID; kill $PROCESSOR_PID" EXIT

./result-logger
