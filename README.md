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

```txt
Processor type: x86_64
OS: GNU/Linux

CPU: Intel(R) Core(TM) i5-6300HQ CPU @ 2.30GHz
RAM: 32GiB
```

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
mean(field-1)   median(field-1) perc:90(field-1)        perc:95(field-1)        perc:99(field-1)
7923,2314781992 434     21797   55226   106364,79
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
mean(field-1)   median(field-1) perc:90(field-1)        perc:95(field-1)        perc:99(field-1)
3867,8244205766 313     9560    27757,6 62035,12
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
Duration Avg: 231 millis
Duration Max: 1129 millis
mean(field-1)   median(field-1) perc:90(field-1)        perc:95(field-1)        perc:99(field-1)
231,7049        19      913     939     1031,01
```

#### 04-ss-poll

```bash
bash get-metrics.sh 04-ss-poll-output.log
```

```txt
Concurrency: 10000
Logged:    10000
Processed: 10000
Rejected:  0
Loose:     0
Duration Avg: 389 millis
Duration Max: 1629 millis
mean(field-1)   median(field-1) perc:90(field-1)        perc:95(field-1)        perc:99(field-1)
389,9287        65      1185    1298    1407
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
Duration Avg: 331 millis
Duration Max: 1452 millis
mean(field-1)   median(field-1) perc:90(field-1)        perc:95(field-1)        perc:99(field-1)
331,97  21      1178    1328,05 1380
```

### Useful links

* [select vs poll vs epoll](https://habr.com/ru/company/infopulse/blog/415259/)
* [linux multiplexing select vs poll vs epoll](https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/#.YJ8nOnX7RKs)
