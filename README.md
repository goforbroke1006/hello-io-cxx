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
bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 20000
bash ./run-demo.sh ./cmake-build-debug/03-ss-select/03-ss-select ./cmake-build-debug/client/client 100000

bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 10000
bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 20000
bash ./run-demo.sh ./cmake-build-debug/04-ss-poll/04-ss-poll ./cmake-build-debug/client/client 100000

bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 10000
bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 20000
bash ./run-demo.sh ./cmake-build-debug/05-ss-epoll/05-ss-epoll ./cmake-build-debug/client/client 100000

```

### Summary


### Useful links

* [select vs poll vs epoll](https://habr.com/ru/company/infopulse/blog/415259/)
* [linux multiplexing select vs poll vs epoll](https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/#.YJ8nOnX7RKs)
