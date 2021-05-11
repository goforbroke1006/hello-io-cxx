# hello-io-cxx

I/O frameworks comparison project.

### Server implementation types

* **01-ss-1th** - single-thread socket server
* **02-ss-5th** - multi-thread socket server
* **03-ss-select** - with select() sys-call
* **04-ss-poll** - with poll() sys-call

### How to run demos

```bash
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make
cd ..

# bash ./run-demo.sh <server_app> <client_app> <concurrency>

bash ./run-demo.sh \
  ./cmake-build-debug/01-ss-1th/01-ss-1th \
  ./cmake-build-debug/client/client \
  10000

bash ./run-demo.sh \
  ./cmake-build-debug/02-ss-5th/02-ss-5th \
  ./cmake-build-debug/client/client \
  10000

bash ./run-demo.sh \
  ./cmake-build-debug/03-ss-select/03-ss-select \
  ./cmake-build-debug/client/client \
  10000

bash ./run-demo.sh \
  ./cmake-build-debug/04-ss-poll/04-ss-poll \
  ./cmake-build-debug/client/client \
  10000

```