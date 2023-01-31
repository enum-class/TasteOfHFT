#! /bin/bash

for run in {1..2}
do
  git clang-format HEAD^ -- include/*.h src/*.cpp tests/*.cpp libs/*.cpp libs/*.h -f
done
