

```bash
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make
cd ..

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

```