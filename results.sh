#!/bin/bash

set -e

TARGET_FILENAME=./RESULTS.md
TMP_FILENAME=./RESULTS.md.tmp


rm -f ${TMP_FILENAME}

{
  echo "# RESULTS"
  echo ""

  echo "### 03-ss-select"

  echo '```txt'
  cat 03-ss-select-10-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 03-ss-select-1000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 03-ss-select-2500-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 03-ss-select-10000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 03-ss-select-25000-output.log.metrics.txt
  echo '```'

  echo "### 04-ss-poll"

  echo '```txt'
  cat 04-ss-poll-10-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 04-ss-poll-1000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 04-ss-poll-2500-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 04-ss-poll-10000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 04-ss-poll-25000-output.log.metrics.txt
  echo '```'

  echo "### 05-ss-epoll"

  echo '```txt'
  cat 05-ss-epoll-10-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 05-ss-epoll-1000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 05-ss-epoll-2500-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 05-ss-epoll-10000-output.log.metrics.txt
  echo '```'

  echo '```txt'
  cat 05-ss-epoll-25000-output.log.metrics.txt
  echo '```'

}>> ${TMP_FILENAME}

mv ${TMP_FILENAME} ${TARGET_FILENAME}
