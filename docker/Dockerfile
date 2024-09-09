#
#    This file is part of the RealtimeSanitizer (RADSan) project.
#    https://github.com/realtime-sanitizer/radsan
#
#    Copyright 2023 David Trevelyan & Alistair Barker
#    Subject to GNU General Public License (GPL) v3.0
#
FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y clang cmake
COPY llvm-project llvm-project

WORKDIR llvm-project/build

RUN CC=clang CXX=clang++ cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON \
    -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" \
    -DLLVM_TARGETS_TO_BUILD=Native ../llvm

RUN make -j`nproc` compiler-rt clang llvm-symbolizer
RUN make DESTDIR=./clang-install -j`nproc` install

FROM ubuntu:24.04
COPY --from=build llvm-project/build/clang-install /
RUN apt-get update && apt-get install -y libstdc++-14-dev binutils
ENV CC=clang
ENV CXX=clang++
