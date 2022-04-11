#!/bin/bash
set -x

# thrid_party #####################################
mkdir cmake-release
cd cmake-release
cmake -DCMAKE_BUILD_TYPE=Release .. 
make -j5
cd ..

mkdir cmake-debug
cd cmake-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j5
cd ..

# PhysX #################################### 
cd ./third_party/physx/physx
./generate_projects.sh linux

cd ./compiler/linux-debug
make -j5
cd ..

cd ./linux-release
make -j5
cd ../../../../

# OZZ-Animation ###########################
# cd ozz
# mkdir build_release
# cd build_release
# cmake -DCMAKE_BUILD_TYPE=Release ..
# make -j5
# cd ..
# 
# mkdir build_debug
# cd build_debug
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# make -j5
# cd ../..

set +x
