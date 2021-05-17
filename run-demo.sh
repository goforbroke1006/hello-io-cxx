#!/bin/bash

# arguments
SERVER_BINARY=$1
CLIENT_BINARY=$2
CONCURRENCY=$3
INTERVAL=$4
TEST_DURATION=$5

# validate arguments
if [[ ! -f ${SERVER_BINARY} ]]; then
  echo 'wrong path'
  exit 1
fi

# constants
SERVER_PORT=12002
SERVER_TARGET=$(basename "$SERVER_BINARY")
LOG_FILENAME=./${SERVER_TARGET}-${CONCURRENCY}-output.log

# ========== ========== run script ========== ==========

pkill -9 -f "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT}" || true # stop old clients
pkill -9 -f "${SERVER_BINARY} 0.0.0.0 ${SERVER_PORT}" || true   # stop old server

set -e

rm -f "${LOG_FILENAME}"
touch "${LOG_FILENAME}"
echo 'prepare log file'

{
  echo "========== ========== ========== ========== =========="
  echo "========== ========== ========== ========== =========="
  echo "========== Client:      ${CLIENT_BINARY}"
  echo "========== Server:      ${SERVER_BINARY}"
  echo "========== Concurrency: ${CONCURRENCY}"
  echo "========== ========== ========== ========== =========="
  echo "========== ========== ========== ========== =========="
} >>"${LOG_FILENAME}"

bash -c "${SERVER_BINARY} 0.0.0.0 ${SERVER_PORT}" &
sleep 5

BEFORE_TIMESTAMP=$(date +%s)
CURRENT_TIMESTAMP=$(date +%s)

while [ $((CURRENT_TIMESTAMP - BEFORE_TIMESTAMP)) -lt $TEST_DURATION ]; do
  sleep "${INTERVAL}" # wait 1 millisecond before run next client
  # shellcheck disable=SC2034
  for ci in $(seq 1 $CONCURRENCY); do
    (
      bash -c "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT} >>${LOG_FILENAME} 2>&1"
      echo "communication finished..."
    ) &
  done
  echo "next bunch of clients (${CONCURRENCY}) ..."
  CURRENT_TIMESTAMP=$(date +%s)
done

#read -p -r "Press enter to continue..."
#while true; do
#  linesCount=$(cat "${LOG_FILENAME}" | grep -v '==' | wc -l)
#  if [[ $linesCount -eq $CONCURRENCY ]]; then
#    break
#  fi
#  sleep 1
#done

echo 'stopping server and clients...'

pkill -9 -f "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT}" || true # stop old clients
pkill -9 -f "${SERVER_BINARY} 0.0.0.0 ${SERVER_PORT}" || true   # stop old server

sleep 5
