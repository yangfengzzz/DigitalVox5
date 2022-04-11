#!/bin/bash
set -x

rm -rf cmake-release
rm -rf cmake-debug

# PhysX #################################### 
cd third_party/physx/physx

cd compiler/linux-debug
make clean
cd ..

cd linux-release
make clean
cd ../../../../

# OZZ #####################################
# cd ozz
# rm -rf build_release
# rm -rf build_debug
# cd ..

set +x
