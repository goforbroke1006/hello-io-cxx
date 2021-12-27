FROM debian:bullseye

RUN apt-get update
RUN apt-get install -y build-essential cmake git

RUN \
    git clone --depth 1 --branch v0.12.3 https://github.com/jupp0r/prometheus-cpp.git && \
    cd prometheus-cpp && \
    git submodule init && \
    git submodule update && \
    mkdir _build && \
    cd _build && \
    cmake .. -DBUILD_SHARED_LIBS=ON -DENABLE_PUSH=OFF -DENABLE_COMPRESSION=OFF && \
    cmake --build . --parallel 4 && \
    ctest -V && \
    cmake --install . && \
    ldconfig
