// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_BASE_H_
#define AUTK_BASE_H_

#define AUTK_LENGTHOF(a) (sizeof(a) / sizeof((a)[0]))

#ifdef __cplusplus
# define AUTK_TOPLEVEL ::
#else
# define AUTK_TOPLEVEL
#endif

#ifdef __cplusplus
# define AUTK_BEGIN_DECLS extern "C" {
# define AUTK_END_DECLS }
#else
# define AUTK_BEGIN_DECLS
# define AUTK_END_DECLS
#endif

#ifdef __GNUC__
# define AUTK_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
# define AUTK_ALWAYS_INLINE __forceinline
#else
# define AUTK_ALWAYS_INLINE inline
#endif

#ifndef AUTK_API
# if AUTK_DLL && defined(_WIN32)
#  define AUTK_API __declspec(dllimport)
# else
#  define AUTK_API
# endif
#endif

#if (defined(__STDC__) && __STDC_VERSION__ >= 202311L) \
    || (defined(__cplusplus) && __cplusplus >= 201703L)
# define AUTK_FALLTHROUGH [[fallthrough]];
#elif defined(__GNUC__)
# define AUTK_FALLTHROUGH __attribute__((fallthrough));
#else
# define AUTK_FALLTHROUGH
#endif

#if defined(__GNUC__) && !defined(_WIN32)
# define AUTK_HIDDEN __attribute__((visibility("hidden")))
#else
# define AUTK_HIDDEN
#endif

#ifdef _MSC_VER
# define AUTK_FMTSTR _Printf_format_string_
#else
# define AUTK_FMTSTR
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) \
    || (defined(__cplusplus) && __cplusplus >= 201703L)
# define AUTK_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__)
# define AUTK_MAYBE_UNUSED __attribute__((unused))
#else
# define AUTK_MAYBE_UNUSED
#endif

#if (defined(__STDC__) && __STDC_VERSION__ >= 202311L) \
    || (defined(__cplusplus) && __cplusplus >= 201103L)
# define AUTK_NORETURN [[noreturn]]
#elif defined(__STDC__) && __STDC_VERSION__ >= 201112L
# define AUTK_NORETURN _Noreturn
#elif defined(__GNUC__)
# define AUTK_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
# define AUTK_NORETURN __declspec(noreturn)
#else
# define AUTK_NORETURN
#endif

#ifdef __GNUC__
# define AUTK_PRINTFLIKE(f, v) __attribute__((format(printf, f, v)))
#else
# define AUTK_PRINTFLIKE(f, v)
#endif

#if defined(__STDC__) && __STDC_VERSION__ >= 199901L
# define AUTK_RESTRICT restrict
#elif defined(__GNUC__) || defined(_MSC_VER)
# define AUTK_RESTRICT __restrict
#else
# define AUTK_RESTRICT
#endif

#define AUTK_UNUSED AUTK_MAYBE_UNUSED

#endif // AUTK_BASE_H_
