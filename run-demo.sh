#!/bin/bash

# arguments
SERVER_BINARY=$1
CLIENT_BINARY=$2
CONCURRENCY=$3

# validate arguments
if [[ ! -f ${SERVER_BINARY} ]]; then
  echo 'wrong path'
  exit 1
fi

# correct with default values
if [[ -z $CONCURRENCY ]]; then CONCURRENCY=100; fi

# constants
SERVER_PORT=12000
SERVER_TARGET=$(basename "$SERVER_BINARY")
LOG_FILENAME=./${SERVER_TARGET}-output.log

# run script

OLD_SERVER_PID=$(lsof -t -i:${SERVER_PORT})
if [[ ! -z $OLD_SERVER_PID ]]; then
  echo 'stopping old server'
  kill -9 "${OLD_SERVER_PID}"
fi
echo 'old process checked'

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

for ((i = 1; i <= ${CONCURRENCY}; i++)); do
  (
    bash -c "${CLIENT_BINARY} 127.0.0.1 ${SERVER_PORT} >>${LOG_FILENAME} 2>&1"
    echo "communication finished..."
  ) &
done

(
  sleep 100
  echo 'shutdown' | telnet 127.0.0.1 ${SERVER_PORT}
  echo 'shutdown server because timeout exceeded'
  exit 1
) &

(
  sleep 125
  pkill -9 -f "${CLIENT_BINARY}"
  pkill -9 -f "${SERVER_BINARY}"
  echo 'terminate clients and server because timeout exceeded'
  exit 1
) &

#read -p -r "Press enter to continue..."
while true; do
  linesCount=$(cat "${LOG_FILENAME}" | grep -v '==' | wc -l)
  if [[ $linesCount -eq $CONCURRENCY ]]; then
    break
  fi
  sleep 1
done

echo 'stopping server...'
pkill -9 -f "${SERVER_BINARY}"
