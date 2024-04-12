# RealtimeSanitizer (RADSan)

## Document Purpose

This doc is mostly intended for LLVM project code reviewers, but it's also for
anyone interested in how RealtimeSanitizer (RADSan) works and how it will be
integrated upstream. Its aim is to add higher-level context as a supplement to
RADSan's upstream pull requests.

## Background

Algorithms with "real-time" constraints must guarantee completion within a
specified time, before their "deadline". Failure to deliver a response before
this deadline is a failure of the system, the severity of which determines the
"hardness" of the system. A more detailed description can be found
[here](https://en.wikipedia.org/wiki/Real-time_computing).

To guarantee an algorithm's completion in a deterministic time, it must never
call any function that takes a non-deterministic time to complete, otherwise
the deadline could be missed and the real-time system would fail. Functions
with non-deterministic execution times are commonly found in C and C++
programs: dynamic memory (de)allocation and (un)locking mutexes are ubiquitous
examples, because they are not implemented with
[_non-blocking_](https://en.wikipedia.org/wiki/Non-blocking_algorithm#:~:text=A%20non%2Dblocking%20algorithm%20is,of%20obstruction%2Dfreedom%20in%202003.)
algorithms. An important part of the real-time programming challenge is
therefore to:

1. satisfy the logical requirements of the system, while
2. avoiding calling any functions that could block.

Implementing an algorithm without blocking function calls in C++ is a
non-trivial task. Dynamic memory allocations, for example, can be hidden at a
level below the API of the C++ STL (consider the construction of a capturing
`std::function`). Mutex locks can appear in surprising places (consider
`std::atomic<std::array<int, 4>>::load()`). Small changes in one part of a
codebase can introduce devastating effects on the real-time safety of a system,
and it may not be obvious why.

RealtimeSanitizer (RADSan) provides a means of detecting calls to blocking
functions in algorithms that should be non-blocking. Such calls are considered
_real-time violations_ and result in a RADSan error. Like other sanitizers,
RADSan instruments the user's binary with interceptors for blocking functions
in libc like `malloc`, `free` and `pthread_mutex_lock`. Non-blocking contexts
are defined by the user simply by marking functions with the
`[[clang::nonblocking]]` attribute.

### Comparison with other saniziters

## System Design

### Components

### Algorithm

### Interception strategy

## Integration Roadmap
