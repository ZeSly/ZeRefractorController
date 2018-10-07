#!/bin/sh
if [ ! -d build ]; then
  mkdir build
fi
cd build
cmake -G "Unix Makefiles" -D "Qt5_DIR=~/Qt/5.5/gcc_64/lib/cmake/Qt5" ..
make
make package

# increase build number
cd ..
string=`cat build_number.h`
set -- $string
num=$3
num=$((num + 1))
echo "#define BUILD_NUMBER $num" > build_number.h
