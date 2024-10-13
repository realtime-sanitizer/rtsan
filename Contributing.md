# Contributing to RealtimeSanitizer

This document describes some helpful tips and tricks for contributing to RealtimeSanitizer. 

RealtimeSanitizer is a part of LLVM, meaning that when in conflict, the two following guides take precedence over this one:

1. [Contributing to LLVM](https://llvm.org/docs/Contributing.html)
2. [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)

We highly recommend reading these two guides, as they have a lot of things not covered in this document. 

The goal of this document is to be a minimal addition to the above, providing specific advice for developing RealtimeSanitizer.

# Building and running tests
It is highly recommended you use [ninja](https://ninja-build.org/) and [ccache](https://ccache.dev/) to build LLVM. This codebase is **LARGE** so any time saved compiling is worth it.

## Generating 

```sh
cd $LLVM_ROOT
mkdir build && cd build
cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DCOMPILER_RT_BUILD_SANITIZERS=ON \
      -DLLVM_ENABLE_PROJECTS="clang;compiler-rt;llvm" \
      -DLLVM_TARGETS_TO_BUILD=Native \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
      -DCMAKE_C_COMPILER_LAUNCHER=ccache \
      ../llvm
```

## Building

### Compiler-rt
The vast majority of the time, code changes will happen in the compiler-rt subproject at `$LLVM_ROOT/compiler-rt/lib/rtsan`. 

To build and run compiler-rt RTsan tests:
```
ninja -C $LLVM_ROOT/build check-rtsan
```

If you make changes to any gtests, be sure to delete the test binary before re-running as there is a build system bug that leads to crufty code (sometimes) being executed. A typical workflow is:

```
rm -rf $LLVM_ROOT/build/projects/compiler-rt/lib/rtsan/tests/
ninja -C $LLVM_ROOT/build check-rtsan
```

### Other subprojects (llvm, clang)

If you make changes in the `llvm` or `clang` subprojects, you can run our subset of tests with a LIT_OPTS filter. 

```
LIT_OPTS="--filter=.*rtsan.*" ninja -C $LLVM_ROOT/build check-llvm check-clang
```

Typically we try to include `rtsan` as a phrase in any new lit test file names, to allow this filter to be all you need.

More info on [lit](https://llvm.org/docs/CommandGuide/lit.html) and [FileCheck](https://llvm.org/docs/CommandGuide/FileCheck.html).

## Other OS's
It is recommended to try and test on both mac and ubuntu before putting up a PR.

If you are on mac you can run tests in a Docker container running a flavor of Linux.

TODO: More details if we need them??


# Submitting PRs
Top line advice:
* PRs should be absolutely atomic, doing one small thing at a time. PRs should almost never span two sub-projects.
* PRs should match the title style of LLVM.
* You should clang-format your code before putting up a PR


More info in [LLVM - How To Submit At Patch](https://llvm.org/docs/Contributing.html#how-to-submit-a-patch).


## Example PR Titles
```
[rtsan][subproject] Added foo and removed bar
...
[rtsan][compiler-rt] Added interceptors for megamalloc
[rtsan][llvm] Added new attribute waitfree
```

## Code formatting
Code should be formatted before submission, or else you will get a build failure. `git-clang-tidy` is great for this:

```
git-clang-tidy main
```

Changes should always include a unit test, except if submitting an NFC (no functional change) PR.


# Reviewers
To request reviewers if you're not an LLVM contributor add Chris ([@cjappl](https://github.com/cjappl)) and David ([@davidtrevelyan](https://github.com/davidtrevelyan)) to the review by tagging them in the comments.