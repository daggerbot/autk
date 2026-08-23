// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_MAIN_H_
#define AUTK_MAIN_H_

#ifdef _WIN32
# include <wtypes.h>
#endif

#include "types.h"

#ifdef _WIN32
# ifdef UNICODE
#  define AUTK_MAIN(ARGC, ARGV) \
      static int _autk_main_(int, wchar_t **); \
      int WINAPI wWinMain(AUTK_UNUSED HINSTANCE hInstance, AUTK_UNUSED HINSTANCE hPrevInstance, \
                          AUTK_UNUSED LPWSTR lpCmdLine, AUTK_UNUSED int nShowCmd) \
      { \
          return _autk_main_(__argc, __wargv); \
      } \
      static int _autk_main_(AUTK_MAYBE_UNUSED int ARGC, AUTK_MAYBE_UNUSED wchar_t **ARGV)
# else
#  define AUTK_MAIN(ARGC, ARGV) \
      static int _autk_main_(int, char **); \
      int WINAPI WinMain(AUTK_UNUSED HINSTANCE hInstance, AUTK_UNUSED HINSTANCE hPrevInstance, \
                         AUTK_UNUSED LPSTR lpCmdLine, AUTK_UNUSED int nShowCmd) \
      { \
          return _autk_main_(__argc, __argv); \
      } \
      static int _autk_main_(AUTK_MAYBE_UNUSED int ARGC, AUTK_MAYBE_UNUSED char **ARGV)
# endif
#else
# define AUTK_MAIN(ARGC, ARGV) int main(AUTK_MAYBE_UNUSED int ARGC, AUTK_MAYBE_UNUSED char **ARGV)
#endif

#endif // AUTK_MAIN_H_
