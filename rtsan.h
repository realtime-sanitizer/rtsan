//===-- rtsan.h -------------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is a part of RealtimeSanitizer.
//
// Allows for users not using LLVM 20 to forward declare rtsan functions
// and helpers.
//===----------------------------------------------------------------------===//

#ifndef SANITIZER_RTSAN_INTERFACE_H
#define SANITIZER_RTSAN_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Enter real-time context.
// When in a real-time context, RTSan interceptors will error if realtime
// violations are detected. Calls to this method are injected at the code
// generation stage when RTSan is enabled.
// corresponds to a [[clang::nonblocking]] attribute.
void __rtsan_realtime_enter(void);

// Exit the real-time context.
// When not in a real-time context, RTSan interceptors will simply forward
// intercepted method calls to the real methods.
void __rtsan_realtime_exit(void);

// Disable all RTSan error reporting in an otherwise real-time context.
// Must be paired with a call to `__rtsan_enable`
void __rtsan_disable(void);

// Re-enable all RTSan error reporting.
// Must follow a call to `__rtsan_disable`.
void __rtsan_enable(void);

// Initializes rtsan if it has not been initialized yet.
// Used by the RTSan runtime to ensure that rtsan is initialized before any
// other rtsan functions are called.
void __rtsan_ensure_initialized(void);

bool __rtsan_is_initialized(void);

#ifdef __cplusplus
} // extern "C"

namespace __rtsan {
#if defined(__SANITIZE_REALTIME)

inline void Initialize() { __rtsan_ensure_initialized(); }

class ScopedRealtime {
public:
  ScopedRealtime() { __rtsan_realtime_enter(); }
  ~ScopedRealtime() { __rtsan_realtime_exit(); }

#if __cplusplus >= 201103L
  ScopedRealtime(const ScopedRealtime &) = delete;
  ScopedRealtime &operator=(const ScopedRealtime &) = delete;
  ScopedRealtime(ScopedRealtime &&) = delete;
  ScopedRealtime &operator=(ScopedRealtime &&) = delete;
#else
private:
  ScopedRealtime(const ScopedRealtime &);
  ScopedRealtime &operator=(const ScopedRealtime &);
#endif // __cplusplus >= 201103L
};

class ScopedDisabler {
public:
  ScopedDisabler() { __rtsan_disable(); }
  ~ScopedDisabler() { __rtsan_enable(); }

#if __cplusplus >= 201103L
  ScopedDisabler(const ScopedDisabler &) = delete;
  ScopedDisabler &operator=(const ScopedDisabler &) = delete;
  ScopedDisabler(ScopedDisabler &&) = delete;
  ScopedDisabler &operator=(ScopedDisabler &&) = delete;
#else
private:
  ScopedDisabler(const ScopedDisabler &);
  ScopedDisabler &operator=(const ScopedDisabler &);
#endif // __cplusplus >= 201103L
};

#else

inline void Initialize() {}

class ScopedRealtime {
public:
  ScopedRealtime() {}
#if __cplusplus >= 201103L
  ScopedRealtime(const ScopedRealtime &) = delete;
  ScopedRealtime &operator=(const ScopedRealtime &) = delete;
  ScopedRealtime(ScopedRealtime &&) = delete;
  ScopedRealtime &operator=(ScopedRealtime &&) = delete;
#else
private:
  ScopedRealtime(const ScopedRealtime &);
  ScopedRealtime &operator=(const ScopedRealtime &);
#endif // __cplusplus >= 201103L
};

class ScopedDisabler {
public:
  ScopedDisabler() {}
#if __cplusplus >= 201103L
  ScopedDisabler(const ScopedDisabler &) = delete;
  ScopedDisabler &operator=(const ScopedDisabler &) = delete;
  ScopedDisabler(ScopedDisabler &&) = delete;
  ScopedDisabler &operator=(ScopedDisabler &&) = delete;
#else
private:
  ScopedDisabler(const ScopedDisabler &);
  ScopedDisabler &operator=(const ScopedDisabler &);
#endif // __cplusplus >= 201103L
};

#endif // defined(__SANITIZE_REALTIME)
} // namespace __rtsan
#endif // __cplusplus

#endif // SANITIZER_RTSAN_INTERFACE_H
