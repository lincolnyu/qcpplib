#if !defined(_SYSTEM_H_)
#define _SYSTEM_H_

#undef _QTL_FUNC
#if defined(_MSC_VER) && (_MSC_VER >= 1020) // Visual C++
#   define _QTL_FUNC __FUNCSIG__
#elif (defined(__GNUC__) && (__GNUC__ >= 2)) // GCC
#   define _QTL_FUNC __PRETTY_FUNCTION__
#elif (defined(__INTEL_COMPILER)) // Intel C++
#   define _QTL_FUNC __PRETTY_FUNCTION__
#elif defined(__clang__) && (__clang__ == 1) // Clang++
#   define _QTL_FUNC __PRETTY_FUNCTION__
#else
#   if defined(__func__)
#      define _QTL_FUNC __func__
#   else
#      define _QTL_FUNC ""
#   endif // defined(__func__)
#endif // defined(_MSC_VER) && (_MSC_VER >= 1020)

// Recognising Compiler

#if defined(__GNUC__)
#   define _QTL_COMPILER_GCC 1
#   define _QTL_GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define _QTL_CXX0X 1
#   elif (_QTL_GCC_VERSION >= 40801)
#      define _QTL_CXX11 1
#   endif // defined(__GXX_EXPERIMENTAL_CXX0X__)
#endif // defined(__GNUC__)
// Visual C++
#if defined(_MSC_VER)
#   define _QTL_COMPILER_MSVC 1
#   define _QTL_CRT_DBG_WARNINGS 1
#   if (_MSC_VER == 1600)
#      define _QTL_CXX0X 1
#   elif (_MSC_VER >= 1700)
#      define _QTL_CXX11 1
#   endif // (_MSC_VER == 1600)
#else
#   define _QTL_CRT_DBG_WARNINGS 0
#endif // defined(_MSC_VER)
// Clang++
#if defined(__clang__) && (__clang__ == 1)
#   define _QTL_COMPILER_CLANG 1
#   define _QTL_CLANG_VERSION (__clang_major__ * 10000 \
                                + __clang_minor__ * 100 \
                                + __clang_patchlevel__)
#   if (_QTL_CLANG_VERSION >= 30300)
#      define _QTL_CXX11 1
#   endif // (_QTL_CLANG_VERSION >= 30300)
#endif // defined(__clang__) && (__clang__ == 1)
// MinGW
#if defined(__MINGW32__) || defined(__MINGW64__)
#   define _QTL_MINGW 1
#endif // defined(__MINGW32__) || defined(__MINGW64__)
// Cygwin
#if defined(__CYGWIN__) && (__CYGWIN__ == 1)
#   define _QTL_CYGWIN 1
#endif // defined(__CYGWIN__) && (__CYGWIN__ == 1)
// Intel C++
#if defined(__INTEL_COMPILER)
#   define _QTL_COMPILER_INTEL 1
#endif

// Recognising Operating System

// Windows
#if defined(_WIN32) || defined(_WIN64)
#   define _QTL_OS_WINDOWS 1
#else
#   define _QTL_OS_WINDOWS 0
#endif // defined(_WIN32) || defined(_WIN64)
// Linux
#if (defined(__linux) || defined(__linux__))
#   define _QTL_OS_LINUX 1
#else
#   define _QTL_OS_LINUX 0
#endif // (defined(__linux) || defined(__linux__))
// Unix
#if (defined(__unix) || defined(__unix__))
#	define _QTL_OS_UNIX 1
#else
#	define _QTL_OS_UNIX 0
#endif
// Mac
#if defined(__APPLE__)
#   define _QTL_OS_MAC 1
#	define _QTL_OS_UNIX 1
#else
#   define _QTL_OS_MAC 0
#endif // defined(__APPLE__)
// Unix
#define _QTL_OS_NIX ((_QTL_OS_LINUX || _QTL_OS_MAC) && (!_QTL_OS_WINDOWS))
// Android
#if defined(__ANDROID__)
#   define _QTL_OS_ANDROID 1
#else
#   define _QTL_OS_ANDROID 0
#endif // defined(__ANDROID__)
// Regard Cygwin as unix OS
#if (!_QTL_OS_NIX && !_QTL_OS_WINDOWS && _QTL_CYGWIN)
#   undef _QTL_OS_NIX
#   undef _QTL_OS_LINUX
#   define _QTL_OS_NIX 1
#   define _QTL_OS_LINUX 1
#endif

// Compiler specific support
// better not use STL for threading in case the OS is unix
#if _QTL_MINGW || _QTL_COMPILER_CLANG || _QTL_OS_UNIX
#   define _QTL_USE_STD_THREADING 0
#else
#   define _QTL_USE_STD_THREADING 1
#endif // _QTL_MINGW || _QTL_COMPILER_CLANG

#endif
