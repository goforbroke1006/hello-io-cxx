#!/bin/bash

FILENAME=$1

concurrency=$(cat "${FILENAME}" | grep '========== Concurrency: ' | awk '{print $3}')
echo "Concurrency: ${concurrency}"

processedClientsMetric=$(cat "${FILENAME}" | grep 'SPEND TIME' | wc -l)
rejectedClientsMetric=$(cat "${FILENAME}" | grep 'connect: Connection timed out' | wc -l)

totalClientsMetrics=$(cat "${FILENAME}" | grep -v "==" | wc -l)
looseClientsMetrics=$((concurrency - totalClientsMetrics))

echo "Logged:    ${totalClientsMetrics}"
echo "Processed: ${processedClientsMetric}"
echo "Rejected:  ${rejectedClientsMetric}"
echo "Loose:     ${looseClientsMetrics}"

durationCount=0
durationSum=0
durationMax=0

rm -f ./durations.log
while read -r line; do
  duration=$(echo "${line}" | awk '{print $3}')
  if [ "$duration" -gt "$durationMax" ]; then
    durationMax=$duration
  fi

  durationCount=$((durationCount + 1))
  durationSum=$((durationSum + duration))

  echo $duration >> ./durations.log
done<<<$(cat "${FILENAME}" | grep 'SPEND TIME')

durationAvg=$((durationSum / durationCount))

echo "Duration Avg: ${durationAvg} millis"
echo "Duration Max: ${durationMax} millis"

datamash --header-out mean 1 median 1 perc:90 1 perc:95 1 perc:99 1 < ./durations.log
