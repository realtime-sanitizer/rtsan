# Realtime Sanitizer

RealtimeSanitizer (a.k.a. RADSan) is a real-time safety testing tool for C and
C++ projects. RADSan can be used to detect *real-time violations*, i.e. calls
to methods that are not safe for use in functions with deterministic runtime
requirements.

RADSan is a customised version of `clang` and, like other popular sanitizers, a
lightweight dynamic library that can detect real-time unsafe system calls.
Calls to functions such as `malloc`, `free` and `pthread_mutex_lock` (along
with anything else RADSan believes may have a nondeterministic execution time)
will cause RADSan to error, but only if they are called within a real-time
context, as defined by the user. Real-time contexts are defined by the user
simply by marking functions with the `[[clang::realtime]]` attribute.

# Table Of Contents

1. [Getting RADSan](#getting-radsan)
    a. [Docker](#docker)
    b. [Building From Source](#building-from-source)
2. [Usage](#usage)
    a. [CMake](#cmake)
3. [Configuration](#configuration)
    a. [Error Modes](#error-modes)
    b. [Disabling RADSan](#disabling-radsan)
    c. [Choice of symbolizer](#choice-of-symbolizer)
4. [How it works](#how-it-works)
5. [Development](#developement)
    a. [Building the Docker Image](#building-the-docker-image)
    a. [Running the tests](#running-the-tests)
6. [Contact](#contact)

# Getting RADSan

## Docker

The fastest way to try RealtimeSanitizer is to pull the [pre-built docker
image](https://hub.docker.com/repository/docker/realtimesanitizer/radsan-clang/), which has `clang` (and other `llvm` tooling) with RADSan readily installed.

```sh
docker pull realtimesanitizer/radsan-clang
```

You can quickly experiment in your own repository using a shared-volume:

```sh
docker run -v $(pwd):/my_repo -it realtimesanitizer/radsan-clang /bin/bash
```

which mounts the host's current working directory at path `/my_repo` in the
container. Alternatively, you may prefer to use the RADSan Docker image as a
parent image for your own development or CI environment:

```Dockerfile
FROM realtimesanitizer/radsan-clang:latest
RUN apt-get update && apt-get install -y git cmake vim
```

## Building from source on Linux & macOS


Building RADSan-enabled clang is performed entirely within the fork of the
llvm-project submodule. The llvm-project is a CMake project, and takes a bit of
time to build. 

To minimise this build time, we recommend using the `ninja`
build system, and configuring with a few specific CMake settings:

```sh
git clone https://github.com/realtime-sanitizer/radsan && cd radsan
git submodule update --init --recursive && cd llvm-project
mkdir build && cd build
cmake -G Ninja \
   -DCMAKE_BUILD_TYPE=Release \
   -DBUILD_SHARED_LIBS=ON \
   -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" \
   -DLLVM_TARGETS_TO_BUILD=Native \
   ../llvm
ninja -j8 clang compiler-rt llvm-symbolizer
```

If built successfully, `clang` should have appeared inside the `bin/` folder,
and the radsan dynamic library should be in `lib/`:

```sh
> find $(pwd)/bin | grep clang
$RADSAN_ROOT/llvm-project/build/bin/clang
$RADSAN_ROOT/llvm-project/build/bin/clang-tblgen
$RADSAN_ROOT/llvm-project/build/bin/clang-cl
$RADSAN_ROOT/llvm-project/build/bin/clang++
$RADSAN_ROOT/llvm-project/build/bin/clang-cpp
$RADSAN_ROOT/llvm-project/build/bin/clang-18
> find $(pwd)/lib | grep radsan
$RADSAN_ROOT/llvm-project/build/lib/clang/18/lib/darwin/libclang_rt.radsan_osx_dynamic.dylib
```

These absolute paths will be used as your compiler in the [Usage](#usage) section.

## Windows

Apologies, RealtimeSanitizer does not yet support Windows. We very much welcome
contributions, so please [contact us](#contact) if you're interested.

# Usage

Using RealtimeSanitizer requires only two actions:

1. Mark a real-time function with the `[[clang::realtime]]` attribute:

```cpp
[[clang::realtime]] void process (processing_data const & data)
{
    auto x = std::vector<float> (16); // oops!
}
```

2. Add `-fsanitize=realtime` to your compile and link flags directly (or see [CMake](#cmake) below). 

With RADSan docker:
```sh
clang -fsanitize=realtime main.cpp
```

With compiled RADSan:
```
$RADSAN_ROOT/llvm-project/build/bin/clang -fsanitize=realtime main.cpp
```

At run-time, RADSan presents detected real-time violations with a helpful stack trace:

```sh
./a.out
Real-time violation: intercepted call to real-time unsafe function `malloc` in real-time context! Stack trace:
    #0 0x5644f383d78a in radsan::printStackTrace() /llvm-project/compiler-rt/lib/radsan/radsan_stack.cpp:36:5
    #1 0x5644f383d630 in radsan::Context::printDiagnostics(char const*) /llvm-project/compiler-rt/lib/radsan/radsan_context.cpp:37:3
    #2 0x5644f383d5d5 in radsan::Context::exitIfRealtime(char const*) /llvm-project/compiler-rt/lib/radsan/radsan_context.cpp:24:5
    #3 0x5644f383e067 in exitIfRealtime /llvm-project/compiler-rt/lib/radsan/radsan_interceptors.cpp:29:29
    #4 0x5644f383e067 in malloc /llvm-project/compiler-rt/lib/radsan/radsan_interceptors.cpp:221:3
    #5 0x7f7a072b798b in operator new(unsigned long) (/lib/x86_64-linux-gnu/libstdc++.so.6+0xae98b) (BuildId: e37fe1a879783838de78cbc8c80621fa685d58a2)
    #6 0x5644f3861d7f in std::__new_allocator<float>::allocate(unsigned long, void const*) (/root/test/a.out+0x2ad7f)
    #7 0x5644f3861d10 in std::allocator_traits<std::allocator<float>>::allocate(std::allocator<float>&, unsigned long) (/root/test/a.out+0x2ad10)
    #8 0x5644f3861ccf in std::_Vector_base<float, std::allocator<float>>::_M_allocate(unsigned long) (/root/test/a.out+0x2accf)
    #9 0x5644f3861c20 in std::_Vector_base<float, std::allocator<float>>::_M_create_storage(unsigned long) (/root/test/a.out+0x2ac20)
    #10 0x5644f38619b1 in std::_Vector_base<float, std::allocator<float>>::_Vector_base(unsigned long, std::allocator<float> const&) (/root/test/a.out+0x2a9b1)
    #11 0x5644f3861838 in std::vector<float, std::allocator<float>>::vector(unsigned long, std::allocator<float> const&) (/root/test/a.out+0x2a838)
    #12 0x5644f3861767 in process(processing_data const&) (/root/test/a.out+0x2a767)
    #13 0x5644f38617d0 in main (/root/test/a.out+0x2a7d0)
    #14 0x7f7a06eefd8f  (/lib/x86_64-linux-gnu/libc.so.6+0x29d8f) (BuildId: a43bfc8428df6623cd498c9c0caeb91aec9be4f9)
    #15 0x7f7a06eefe3f in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x29e3f) (BuildId: a43bfc8428df6623cd498c9c0caeb91aec9be4f9)
    #16 0x5644f383d4a4 in _start (/root/test/a.out+0x64a4)
```




## CMake

RADSan-enabled clang is installed to `/usr/local` in the RADSan Docker image,
and CMake will automatically detect it. However, if you've built RADSan from
source, you'll need to instruct CMake to use it by either:

1. setting the `CC` and `CXX` environment variables,

```sh
CC=/path/to/built/clang CXX=/path/to/built/clang++ cmake ..
```

2. passing the `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` options to cmake

```sh
cmake -DCMAKE_C_COMPILER=/path/to/built/clang -DCMAKE_CXX_COMPILER=/path/to/built/clang++ 
```

3. or using `set(CMAKE_CXX_COMPILER ...)` in your CMake project file.


```cmake
set(CMAKE_CXX_COMPILER /path/to/built/clang++)
set(CMAKE_C_COMPILER   /path/to/built/clang)
```

Adding the compiler flag `-fsanitize=realtime` can be done however works best
for your project. Individual targets can be configured with:

```cmake
target_compile_options(MyTarget PUBLIC -fsanitize=realtime)
target_link_options(MyTarget PUBLIC -fsanitize=realtime)
```

or add the following to configure all targets in the project:

```cmake
add_compile_options(-fsanitize=realtime)
add_link_options(-fsanitize=realtime)
```

# Configuration

## Error modes

RADSan's behaviour during real-time violations can be configured using the `RADSAN_ERROR_MODE`
environment variable. RADSan recognises any of the following values for `RADSAN_ERROR_MODE`:

1. `exit` (RADSan will immediately exit with a failure code on the first violation),
2. `continue` (RADSan will print errors and continue), or
3. `interactive` (RADSan will wait for your instruction at the command line):
```sh
> RADSAN_ERROR_MODE=interactive ./my_executable

Real-time violation: intercepted call to real-time unsafe function `malloc` in real-time context! Stack trace:
    #0 0x5644f383d78a in radsan::printStackTrace() /llvm-project/compiler-rt/lib/radsan/radsan_stack.cpp:36:5
    #1 0x5644f383d630 in radsan::Context::printDiagnostics(char const*) /llvm-project/compiler-rt/lib/radsan/radsan_context.cpp:37:3
    ... (etc)

Continue? (Y/n):
```

The default configuration is `RADSAN_ERROR_MODE=exit`.

## Disabling RADSan

You might find a case where you disagree with RADSan's assessment of real-time
safety. Consider the case of locking and unlocking a mutex; these operations
can block if the mutex is contested, and common advice is to avoid them in
real-time contexts. However, it can be argued that their use is safe under
certain special constraints, like if the mutex is never contested (you might be
re-using multi-threaded code in a single-threaded context), or if it's only
contested at times when a user is expecting a glitch anyway (if, say, an audio
device is disconnected). RADSan always assumes the worst, and this may not be
true for your particular use case. You can turn off RADSan's error detection
temporarily by wrapping code in `radsan_off()` and `radsan_on()` as follows:

```cpp
#include "radsan.h" // (found in llvm-project/compiler-rt/lib/radsan)

[[clang::realtime]] float process (float x)
{
    auto const y = 2.0f * x;

    radsan_off();
    i_know_this_method_is_realtime_safe_but_radsan_complains_about_it();
    radsan_on();
}
```

## Choice of symbolizer

By default, RADSan uses the installed `llvm-symbolizer` to symbolise the stack
trace. If you prefer a different symboliser, you can configure RADSan to use it
by setting the envionment variable `RADSAN_SYMBOLIZER_PATH` at run-time.


# How it works

Radsan contains a submodule with a fork of the `llvm-project`. This fork contains 2 main areas of new functionality:

- Changes to the clang compiler front end that:

    - check for `-fsanitize=realtime` as a compiler flag,
    - look for functions marked with the `[[clang::runtime]]` function attribute,
    - insert calls to `radsan_realtime_enter()` & `radsan_realtime_exit()` at
      the start and end of each of these realtime functions, which are used by the
      radsan library (described below), to keep track of whether we are inside
      a realtime context or not.

- `radsan` library

    - This library links to your application at run time and contains 2 main functional areas:
        - `radsan::Context`: which is used to maintain whether we are in a "real-time" context and handle any errors
        -  The `INTERCEPTOR`s, which are used to intercept calls to the system library at runtime.
            It uses interceptors from llvm's `compiler-rt` library and contains an interceptor
            for each function we want to intercept. They all follow the same pattern: check if we're in a real-time context, if not, then call the "real" function, if it is, then handle the error

      ```cpp
      INTERCEPTOR(void *, malloc, SIZE_T size) {
        radsan::expectNotRealtime("malloc");
        return REAL(malloc)(size);
      }
      ```

# Development


## Building the Docker image

Building the Docker image locally is straightforward:

```sh
docker build -t radsan -f docker/Dockerfile .
```

## Running the tests

RADSan follows the existing sanitizer testing patterns, and adds two new test
targets to the compiler-rt project for each architecture `arch`:

1. `TRadsan-${arch}-Test` (which is instrumented by RADSan), and
2. `TRadsan-${arch}-NoInstTest` (which are unit tests that do not need the RADSan instrumentation)

Here's an example script for running the RADSan tests in isolation on `arm64`
architecture. From your build folder in llvm-project:

```sh
ninja TRadsan-arm64-Test && ./projects/compiler-rt/lib/radsan/tests/Radsan-arm64-Test
ninja TRadsan-arm64-NoInstTest && ./projects/compiler-rt/lib/radsan/tests/Radsan-arm64-NoInstTest
```

# Contact

RealtimeSanitizer was developed by David Trevelyan and Ali Barker. We welcome
contributions to make this tool more helpful to a wider group of developers.
For all comments, suggestions and queries, please contact us by:

1. joining the [Discord server](https://discord.gg/DZqjbmSZzZ), or
2. sending an email to realtime.sanitizer@gmail.com.
