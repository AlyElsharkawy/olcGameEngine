#!/bin/bash
if [ "$1" == "clean" ]; then 
  rm -r ./lib/* > /dev/null 2>&1
  rm -r ./bin/* > /dev/null 2>&1
  rm -r ./build/* >/dev/null 2>&1
  cd ./build/ 
  make clean
  echo "Succesfully cleaned project"
  exit 0
fi

# make compilation multithreaded
export MAKEFLAGS="-j$(nproc)"

cmake -S . -B ./build -DCMAKE_BUILD_TYPE=debug
cd ./build
make debug
# $? is used to access exit code of previously executed command
if [ "$?" != 0 ]; then
  echo "ERROR: Could not build debug project"
  exit 1
fi
cd ..
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=release
cd ./build
make release
if [ "$?" != 0 ]; then
  echo "ERROR: Could not build release project"
  exit 2
fi
cd ..
echo "Succesfully built project!"
