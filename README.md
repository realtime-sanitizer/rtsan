# RealtimeSanitizer

RealtimeSanitizer (a.k.a. RTSan) is a real-time safety testing tool for C and
C++ projects. RTSan can be used to detect _real-time violations_, i.e. calls to
methods that are not safe for use in functions with deterministic runtime
requirements.

RTSan is part of the LLVM project, starting from v20.0.0. Like other popular
sanitizers, it uses a lightweight dynamic library that can detect real-time
unsafe system library calls. Calls to functions such as `malloc`, `free` and
`pthread_mutex_lock` (along with anything else RTSan believes may have a
nondeterministic execution time) will cause RTSan to error, but only if they
are called within a real-time context, as defined by the user. Real-time
contexts are defined by the user simply by marking functions with the
`[[clang::nonblocking]]` attribute.

## Usage

Using RealtimeSanitizer requires LLVM 20's `clang` and two actions:

1. Mark a real-time function with the `[[clang::nonblocking]]` attribute:

```cpp
void process(processing_data const & data) [[clang::nonblocking]]
{
    auto x = std::vector<float> (16); // oops!
}
```

2. Add `-fsanitize=realtime` to your compile and link flags (for CMake see [below](#cmake)):

```sh
clang -fsanitize=realtime main.cpp
```

At run-time, real-time violations are presented with a stack trace:

```sh
> ./a.out
Real-time violation: intercepted call to real-time unsafe function `malloc` in real-time context! Stack trace:
    #0 0x5644f383e067 in malloc /llvm-project/compiler-rt/lib/rtsan/rtsan_interceptors.cpp:221:3
    #1 0x7f7a072b798b in operator new(unsigned long) (/lib/x86_64-linux-gnu/libstdc++.so.6+0xae98b) (BuildId: e37fe1a879783838de78cbc8c80621fa685d58a2)
    #2 0x5644f3861d7f in std::__new_allocator<float>::allocate(unsigned long, void const*) (/root/test/a.out+0x2ad7f)
    #3 0x5644f3861d10 in std::allocator_traits<std::allocator<float>>::allocate(std::allocator<float>&, unsigned long) (/root/test/a.out+0x2ad10)
    #4 0x5644f3861ccf in std::_Vector_base<float, std::allocator<float>>::_M_allocate(unsigned long) (/root/test/a.out+0x2accf)
    #5 0x5644f3861c20 in std::_Vector_base<float, std::allocator<float>>::_M_create_storage(unsigned long) (/root/test/a.out+0x2ac20)
    #6 0x5644f38619b1 in std::_Vector_base<float, std::allocator<float>>::_Vector_base(unsigned long, std::allocator<float> const&) (/root/test/a.out+0x2a9b1)
    #7 0x5644f3861838 in std::vector<float, std::allocator<float>>::vector(unsigned long, std::allocator<float> const&) (/root/test/a.out+0x2a838)
    #8 0x5644f3861767 in process(processing_data const&) (/root/test/a.out+0x2a767)
    #9 0x5644f38617d0 in main (/root/test/a.out+0x2a7d0)
    #10 0x7f7a06eefd8f  (/lib/x86_64-linux-gnu/libc.so.6+0x29d8f) (BuildId: a43bfc8428df6623cd498c9c0caeb91aec9be4f9)
    #11 0x7f7a06eefe3f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x29e3f) (BuildId: a43bfc8428df6623cd498c9c0caeb91aec9be4f9)
    #12 0x5644f383d4a4 in _start (/root/test/a.out+0x64a4)
```

# Table Of Contents

1. [Getting RTSan](#getting-rtsan)
   1. [Docker](#docker)
   2. [Building LLVM 20 from source](#building-from-source)
   3. [Windows](#windows)
2. [Documentation](#documentation)
3. [CMake](#cmake)
4. [Using RTSan with other compilers](#using-rtsan-with-other-compilers)
5. [How it works](#how-it-works)
6. [Upstream integration roadmap](#roadmap)
7. [Contact](#contact)

# Getting RTSan

Until LLVM 20 is shipped with your OS, you'll need to download or build it
yourself to use RealtimeSanitizer. The easiest way to experiment with RTSan is
with our Docker image, for which more details can be found below.

## Docker

The fastest way to try RealtimeSanitizer is to pull the [pre-built docker
image](https://hub.docker.com/repository/docker/realtimesanitizer/rtsan-clang/),
which has `clang` (and other `llvm` tooling) with RTSan readily installed.

```sh
docker pull realtimesanitizer/rtsan-clang
```

You can quickly experiment in your own repository using a shared-volume:

```sh
docker run -v $(pwd):/my_repo -w /my_repo -it realtimesanitizer/rtsan-clang /bin/bash
```

which mounts the host's current working directory at path `/my_repo` in the
container, and initialises the working directory to it. Alternatively, you may
prefer to use the RTSan Docker image as a parent image for your own development
or CI environment:

```Dockerfile
FROM realtimesanitizer/rtsan-clang:latest
RUN apt-get update && apt-get install -y git cmake vim
```

## Building LLVM 20 from source

RTSan is now part of LLVM 20; please see the build instructions
[here](https://llvm.org/docs/CMake.html) for more information.

## Windows

Apologies, RealtimeSanitizer does not yet support Windows. We very much welcome
contributions, so please [contact us](#contact) if you're interested.

# Documentation

A full list of features and instructions for configuring RealtimeSanitizer can
be found in the official docs [here](https://clang.llvm.org/docs/RealtimeSanitizer.html).

# CMake

LLVM 20 `clang` is installed to `/usr/local` in the [RTSan Docker
image](https://hub.docker.com/r/realtimesanitizer/rtsan-clang), and CMake will
automatically detect it. However, if you've built the latest LLVM from source,
you'll need to instruct CMake to use the right version of `clang` by either i)
setting the `CC` and `CXX` environment variables or ii) passing the
`CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` options to CMake as arguments.

Adding the compile and link flag `-fsanitize=realtime` can be done however
works best for your project. One unintrusive option is to pass them as
arguments to CMake:

```sh
cmake \
  -DCMAKE_CXX_FLAGS="-fsanitize=realtime" \
  -DCMAKE_C_FLAGS="-fsanitize=realtime" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=realtime" ...
```

# How it works

RTSan's algorithm consists of two parts that work together:

- A compilation step that
  - signals real-time context entry and exit for the duration of `[[clang::nonblocking]]` functions, and
- A lightweight runtime library that
  - keeps track of the real-time context,
  - detects calls to system library functions that are known to block, and
  - raises an error if a blocking library function is called in a real-time context.

# Using RTSan with other compilers


The recommended way to use RTSan is to use it with LLVM 20 directly, as described elsewhere in this document. The rest of this section describes a hack which may or may not continue to work in the future.

If you are in a position where you cannot use this compiler and instead rely on AppleClang or GCC, you can still use RTSan by directly linking in the runtime in directly.

First, build the RTSan runtime by following the instructions in the [official docs](https://clang.llvm.org/docs/RealtimeSanitizer.html).

From there, find the RTSan runtime library and link it to your binary. This differs based on system:
```
> cd $BUILD_DIR_MAC
> find . -name "*rtsan_osx*dylib"
./lib/clang/20/lib/darwin/libclang_rt.rtsan_osx_dynamic.dylib

...
> cat your_proj/CMakeLists.txt
...
target_link_libraries(helloWorld PRIVATE
  libclang_rt.rtsan_osx_dynamic.dylib
)
```

```
> cd $BUILD_DIR_LINUX
> find . -name "libclang_rt.rtsan.a"
./lib/clang/20/lib/aarch64-unknown-linux-gnu/libclang_rt.rtsan.a

...
> cat your_proj/CMakeLists.txt
...
target_link_libraries(helloWorld PRIVATE
  libclang_rt.rtsan.a
  pthread
  dl
)
```
In your code, you must `#include "include/rtsan_standalone/rtsan_standalone.h"`, provided in this repo. Initialize RTSan, and put `__rtsan::ScopedSanitizeRealtime()` in places where you would normally use `[[clang::nonblocking]]` (in the top level of your real-time callback).

```cpp
#include "rtsan_standalone/rtsan_standalone.h"

int main() {
    __rtsan::Initialize();
    ...
}

void my_real_time_function() {
    __rtsan::ScopedSanitizeRealtime ssr;
    ...
}
```

To "enable" the sanitizer, you must compile your code defining `__SANITIZE_REALTIME`, e.g.

```
clang++ main.cpp -D__SANITIZE_REALTIME
```

Without this flag, each of these aforementioned constructs will compile to a no-op, and the sanitizer will be disabled.

This header also defines `__rtsan::ScopedDisabler()`, which allows for disabling the sanitizer in a specific scope. Please see the official docs for more information.

# Upstream Integration Roadmap

- [x] Minimum viable functionality: raise an error if a blocking system library function is called in a real-time context (e.g. `malloc`)
- [x] Ability to disable error reporting in a specified scope (`__rtsan::ScopedDisabler`)
- [x] Raise an error if a function attributed with `[[clang::blocking]]` is called in a real-time context
- [x] `Continue` error mode - using `halt_on_error` runtime flag
- [ ] Suppressing specific errors
    - In progress [here](https://github.com/llvm/llvm-project/pull/111608)

# Contact

RealtimeSanitizer was invented by David Trevelyan and Ali Barker, and the
upstream integration was authored by Chris Apple and David Trevelyan, who
continue its maintenance. We welcome further contributions to make this tool
more helpful to a wider group of developers. For all comments, suggestions and
queries, please contact us by:

1. joining the [Discord server](https://discord.gg/DZqjbmSZzZ),
2. sending an email to realtime.sanitizer@gmail.com, or
3. raising an issue in this GitHub repository.
