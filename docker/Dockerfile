#
#    This file is part of the RealtimeSanitizer (RTSan) project, under the
#    Apache v2.0 license.
#
#    https://github.com/realtime-sanitizer/rtsan
#

FROM ubuntu:24.04 AS build
ARG LLVM_PROJECT_PATH
RUN test -n "${LLVM_PROJECT_PATH}" # Must be defined as build arg
COPY ${LLVM_PROJECT_PATH} llvm-project
RUN apt-get update && apt-get install -y clang cmake
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
