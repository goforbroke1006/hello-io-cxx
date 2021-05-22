#!/bin/bash

set -e

sudo apt-get install -y cmake
sudo apt-get install -y datamash

(
  mkdir -p ~/projects/
  cd ~/projects/ || exit 1

  rm -rf ./prometheus-cpp/
  git clone --depth 1 --branch v0.12.3 https://github.com/jupp0r/prometheus-cpp.git

  (
    cd ./prometheus-cpp/ || exit 1

    # fetch third-party dependencies
    git submodule init
    git submodule update

    mkdir -p _build
    cd _build || exit 1

    # run cmake
    cmake .. -DBUILD_SHARED_LIBS=ON # or OFF for static libraries

    # build
    make -j 4

    # run tests
    ctest -V

    # install the libraries and headers
    sudo make install
  )

  sudo ldconfig

  ldconfig -p | grep prometheus
)
