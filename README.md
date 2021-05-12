# hello-io-cxx

I/O frameworks comparison project.

### Server implementation types

* **01-ss-1th** - single-thread socket server, sequential accept() calls, blocking socket usage
* **02-ss-5th** - multi-thread socket server, 5 threads, accept() wrapped with mutex, blocking socket usage
* **03-ss-select** - with select() sys-call, non-blocking socket usage
* **04-ss-poll** - with poll() sys-call, non-blocking socket usage
* **05-ss-epoll** - with epoll() sys-call, non-blocking socket usage

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
bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 10000

```

### Summary

Use **get-metrics.sh** script to analyze output logs.

#### 01-ss-1th

```bash
bash get-metrics.sh 01-ss-1th-output.log
```

```txt
Concurrency: 10000
Logged:    8031
Processed: 5642
Rejected:  2389
Loose:     1969
Duration Avg: 7923 millis
Duration Max: 106442 millis
```

#### 02-ss-5th

```bash
bash get-metrics.sh 02-ss-5th-output.log
```

```txt
Concurrency: 10000
Logged:    9376
Processed: 8845
Rejected:  531
Loose:     624
Duration Avg: 3867 millis
Duration Max: 102685 millis
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
Duration Avg: 326 millis
Duration Max: 9634 millis
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
Duration Avg: 1721 millis
Duration Max: 4609 millis
```


#### 05-ss-epoll

```bash
bash get-metrics.sh 05-ss-epoll-output.log
```

```txt
Concurrency: 10000
Logged:    10000
Processed: 10000
Rejected:  0
Loose:     0
Duration Avg: 1350 millis
Duration Max: 3027 millis
```
