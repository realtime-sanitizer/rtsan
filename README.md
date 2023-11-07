# Realtime Sanitizer

RealtimeSanitizer (a.k.a. RADSan) is a realtime-safety testing tool for C and
C++ projects. RADSan can be used to detect *realtime violations*, i.e. calls to
any method that is not safe for use with deterministic runtime requirements.

RADSan is a customised version of `clang` and, like other popular sanitizers, a
lightweight dynamic library that can detect real-time unsafe system calls.
Calls to functions such as `malloc`, `free` and `pthread_mutex_lock` (along
with anything else RADSan believes may have a nondeterministic execution time)
will cause RADSan to error, but only if they are called within a realtime
context, as defined by the user. Realtime contexts are defined by the user
simply by marking functions with the `[[clang::realtime]]` attribute.

# Getting it

## Docker

The fastest way to try RADSan is to pull the pre-built docker image, which has
RADSan-enabled `clang` (and other `llvm` tooling) readily installed.

```sh
$ docker pull realtime-sanitizer/radsan
```

You can experiment in your own repository simply by using Docker's
shared-volumes feature:

```sh
(/path/to/my_repo) $ docker run -v $(pwd):/my_repo -it realtime-sanitizer/radsan /bin/bash
root@abcdef012345:/# cd /my_repo
root@abcdef012345:/# mkdir build_radsan // etc.
```

Alternatively, you may prefer to use the RADSan docker image as a parent image for your own:

```Dockerfile
FROM realtime-sanitizer/radsan:latest
RUN apt-get update && apt-get install -y git cmake vim // etc.
```

## Linux

Sorry, pre-built binaries for Linux are not yet available for download. Please
see the "Building from source" section below for instructions.

## macOS

Sorry, pre-built binaries for macOS are not yet available for download. Please
see the "Building from source" section below for instructions.

## Windows

Apologies, RealtimeSanitizer does not yet support Windows. We very much welcome
contributions, so please contact us (details at the bottom of this README) if
you're interested.

# Using it

RADSan is a forked version of `clang` and a lightweight dynamic library, that
together allow instrumentation of realtime algorithms at a granularity chosen
by the user. Using it is very simple, requiring only two actions:

1. mark a real-time function with the `[[clang::realtime]]` attribute, and
2. add `-fsanitize=realtime` to your compile and link flags.

## Real-time function attribute

Any function can be marked as having real-time
requirements with the `[[clang::realtime]]` attribute:

```cpp
[[clang::realtime]] void process (processing_data const & data)
{
    auto x = std::vector<float> (16); // oops!
}
```

## Compile and link flags

To activate RADSan, pass the flag `-fsanitize=realtime` to clang for compiling
and linking:

```sh
$ clang -fsanitize=realtime main.cpp
```

## Results

At run-time, RADSan presents detected realtime violations with a helpful stack trace:

```
$ ./a.out
Intercepted call to realtime-unsafe function `malloc` from realtime context! Stack trace
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

If you're using the RADSan Docker image, CMake will automatically detect
RADSan-enabled clang as the C/C++ compiler because it is installed in the
system path. If you've built it from source, you'll need to instruct CMake to
use it by either:

1. setting the `CC` and `CXX` environment variables,

```sh
$ CC=/path/to/built/clang CXX=/path/to/built/clang++ cmake ..
```

2. passing the `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` options to cmake

```sh
$ cmake -DCMAKE_CXX_COMPILER=/path/to/built/clang++ ..
```

3. or using `set(CMAKE_CXX_COMPILER ...)` in your CMake project file.


```cmake
set(CMAKE_CXX_COMPILER /path/to/built/clang++)
```

Adding the compiler flag `-fsanitize=realtime` can be done however works best
for your project. Individual targets can be configured with:

```cmake
target_compile_options(MyTarget PUBLIC -fsanitize=realtime)
target_link_options(MyTarget PUBLIC -fsanitize=realtime)
```

For your entire project globally, add the following early in your CMake project
file:

```cmake
add_compile_options(-fsanitize=realtime)
add_link_options(-fsanitize=realtime)
```

## Configuration

### Error modes

On detection of realtime violations, RADSan can be configured to either:

1. Exit (with a failure return code),
2. Pause and wait for user instruction,
3. Print the error and continue, or
4. Log the error to file.

TODO details...

### Choice of symbolizer

By default, RADSan uses the installed `llvm-symbolizer` to symbolise the stack
trace. If you prefer a different symboliser, you can configure RADSan to use it
by setting the envionment variable `RADSAN_SYMBOLIZER_PATH` at run-time.

# How it works

TODO

# Building from source

## Clang with RealtimeSanitizer

Building RADSan-enabled clang is performed entirely within the fork of the
llvm-project submodule. The llvm-project is a CMake project, and takes a bit of
time to build. To minimise this build time, we recommend using the `ninja`
build system, and configuring with the following CMake settings:

```sh
$ cd llvm-project
$ mkdir build && cd build
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
     -DBUILD_SHARED_LIBS=ON \
     -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" \
     -DLLVM_TARGETS_TO_BUILD=Native \
     ../llvm
$ ninja -j8 clang compiler-rt llvm-symbolizer
```

If built successfully, `clang` should have appeared inside the `bin/` folder,
and the radsan dynamic library should be in `lib/`:

```sh
$ find lib | grep radsan
lib/clang/18/lib/darwin/libclang_rt.radsan_osx_dynamic.dylib
$ find bin | grep clang
bin/clang
bin/clang-tblgen
bin/clang-cl
bin/clang++
bin/clang-cpp
bin/clang-18
```

## RADSan docker image

Building the docker image locally is straightforward:

```sh
(radsan) $ docker build -t radsan -f docker/Dockerfile .
```

# Contact

RealtimeSanitizer was developed by David Trevelyan and Ali Barker. We welcome
contributions to make this tool more helpful to a wider group of developers.
For all comments, suggestions and queries, please contact us by:

1. joining the [Discord server](https://discord.gg/UMkhAxU9), or
2. sending an email to realtime.sanitizer@gmail.com.

# Stuff

A realtime context is established automatically at the entrypoint of a
`[[clang::realtime]]` function, and torn down at the corresponding exit.
