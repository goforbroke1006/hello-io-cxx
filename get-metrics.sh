#!/bin/bash

FILENAME=$1

TARGET_FILENAME="${FILENAME}.metrics.txt"
DURATIONS_LOG="${FILENAME}-durations.log"

rm -f "${TARGET_FILENAME}" "${DURATIONS_LOG}"

concurrency=$(cat "${FILENAME}" | grep '========== Concurrency: ' | awk '{print $3}')

processedClientsMetric=$(cat "${FILENAME}" | grep -c 'SPEND TIME')
rejectedClientsMetric=$(cat "${FILENAME}" | grep -c 'failed')

(
  echo "Concurrency: ${concurrency}"
  echo "Processed:   ${processedClientsMetric}"
  echo "Rejected:    ${rejectedClientsMetric}"
) >> "${TARGET_FILENAME}"



rm -f ./durations.log
while read -r line; do
  duration=$(echo "${line}" | awk '{print $3}')
  echo "${duration}" >> "${DURATIONS_LOG}"
done<<<$(cat "${FILENAME}" | grep 'SPEND TIME')

(
  datamash --header-out mean 1 median 1 perc:90 1 perc:95 1 perc:99 1 < "${DURATIONS_LOG}"
) >> "${TARGET_FILENAME}"