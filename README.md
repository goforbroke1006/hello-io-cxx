# hello-io-cxx

I/O frameworks comparison project.

### Server implementation types

* **01-ss-1th** - single-thread socket server, sequential accept() calls, blocking socket usage
* **02-ss-5th** - multi-thread socket server, 5 threads, accept() wrapped with mutex, blocking socket usage
* **03-ss-select** - with select() sys-call, non-blocking socket usage
* **04-ss-poll** - with poll() sys-call, non-blocking socket usage

### How to run demos

```bash
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make
cd ..

# bash ./run-demo.sh <socket_server_app> <socket_client_app> <concurrency>

bash ./run-demo.sh ./cmake-build-debug/01-ss-1th/01-ss-1th ./cmake-build-debug/client/client 10000
bash ./run-demo.sh ./cmake-build-debug/02-ss-5th/02-ss-5th ./cmake-build-debug/client/client 10000
bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 10000
bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 10000

```

### Summary

Use **get-metrics.sh** script to analyze output logs.

#### 01-ss-1th

```bash
bash get-metrics.sh 01-ss-1th-output.log
```

```txt
Concurrency: 10000
Logged:    6880
Processed: 3460
Rejected:  3420
Loose:     3120
Duration Avg: 400 millis
Duration Max: 14700 millis
```

#### 02-ss-5th

```bash
bash get-metrics.sh 02-ss-5th-output.log
```

```txt
Concurrency: 10000
Logged:    8855
Processed: 7271
Rejected:  1584
Loose:     1145
Duration Avg: 1695 millis
Duration Max: 107322 millis
```

#### 03-ss-select

```bash
bash get-metrics.sh 03-ss-select-output.log
```

```txt
Concurrency: 10000
Logged:    10000
Processed: 10000
Rejected:  0
Loose:     0
Duration Avg: 184 millis
Duration Max: 14880 millis
```

#### 04-ss-poll

```bash
bash get-metrics.sh 04-ss-poll-output.log
```

```txt
Concurrency: 10000
Logged:    9999
Processed: 9999
Rejected:  0
Loose:     1
Duration Avg: 592 millis
Duration Max: 15973 millis
```
