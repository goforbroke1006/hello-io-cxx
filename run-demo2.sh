#!/bin/bash

# arguments
CLIENT_BINARY=./cmake-build-debug/client/client
CONCURRENCY=$1
INTERVAL=0.01
TEST_DURATION=60

# constants
SERVER_PORT=12000

# ========== ========== run script ========== ==========

pkill -9 -f "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT}" || true # stop old clients

set -e

BEFORE_TIMESTAMP=$(date +%s)
CURRENT_TIMESTAMP=$(date +%s)

while [ $((CURRENT_TIMESTAMP - BEFORE_TIMESTAMP)) -lt $TEST_DURATION ]; do
  sleep "${INTERVAL}" # wait 1 millisecond before run next client
  # shellcheck disable=SC2034
  for ci in $(seq 1 $CONCURRENCY); do
    (
      bash -c "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT}"
      echo "communication finished..."
    ) &
  done
  echo "next bunch of clients (${CONCURRENCY}) ..."
  CURRENT_TIMESTAMP=$(date +%s)
done

echo 'stopping server and clients...'

pkill -9 -f "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT}" || true # stop old clients

sleep 5
