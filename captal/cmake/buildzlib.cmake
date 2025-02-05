
captal_download_submodule(captal/external/zlib TRUE)

set(BUILD_SHARED_LIBS               OFF CACHE INTERNAL "")
set(WITH_GZFILEOP                   ON  CACHE INTERNAL "Compile with support for gzFile related functions")
set(ZLIB_COMPAT                     ON  CACHE INTERNAL "Compile with zlib compatible API")
set(ZLIB_ENABLE_TESTS               OFF CACHE INTERNAL "Build test binaries")
set(ZLIB_DUAL_LINK                  OFF CACHE INTERNAL "Dual link tests against system zlib")
set(WITH_SANITIZERS                 OFF CACHE INTERNAL "Build with address sanitizer and all supported sanitizers other than memory sanitizer")
set(WITH_MSAN                       OFF CACHE INTERNAL "Build with memory sanitizer")
set(WITH_FUZZERS                    OFF CACHE INTERNAL "Build test/fuzz")
set(WITH_OPTIM                      ON  CACHE INTERNAL "Build with optimisation")
set(WITH_NEW_STRATEGIES             ON  CACHE INTERNAL "Use new strategies")
set(WITH_NATIVE_INSTRUCTIONS        OFF CACHE INTERNAL "Instruct the compiler to use the full instruction set on this host (gcc/clang -march=native)")
set(WITH_MAINTAINER_WARNINGS        OFF CACHE INTERNAL "Build with project maintainer warnings")
set(WITH_CODE_COVERAGE              OFF CACHE INTERNAL "Enable code coverage reporting")
set(WITH_INFLATE_STRICT             OFF CACHE INTERNAL "Build with strict inflate distance checking")
set(WITH_INFLATE_ALLOW_INVALID_DIST OFF CACHE INTERNAL "Build with zero fill for inflate invalid distances")
set(WITH_UNALIGNED                  OFF CACHE INTERNAL "Support unaligned reads on platforms that support it")
set(WITH_ACLE                       ON  CACHE INTERNAL "Build with ACLE")
set(WITH_NEON                       ON  CACHE INTERNAL "Build with NEON intrinsics")
set(WITH_POWER8                     ON  CACHE INTERNAL "Build with optimisations for POWER8")
set(WITH_DFLTCC_DEFLATE             OFF CACHE INTERNAL "Use DEFLATE CONVERSION CALL instruction for compression on IBM Z")
set(WITH_DFLTCC_INFLATE             OFF CACHE INTERNAL "Use DEFLATE CONVERSION CALL instruction for decompression on IBM Z")
set(WITH_AVX2                       ON  CACHE INTERNAL "Build with AVX2")
set(WITH_SSE2                       ON  CACHE INTERNAL "Build with SSE2")
set(WITH_SSSE3                      ON  CACHE INTERNAL "Build with SSSE3")
set(WITH_SSE4                       ON  CACHE INTERNAL "Build with SSE4")
set(WITH_PCLMULQDQ                  ON  CACHE INTERNAL "Build with PCLMULQDQ")
set(INSTALL_UTILS                   OFF CACHE INTERNAL "Copy minigzip and minideflate during install")

add_compile_definitions(ZLIB_CONST)

add_subdirectory(external/zlib EXCLUDE_FROM_ALL)

unset(BUILD_SHARED_LIBS                CACHE)
unset(WITH_GZFILEOP                    CACHE)
unset(ZLIB_COMPAT                      CACHE)
unset(ZLIB_ENABLE_TESTS                CACHE)
unset(ZLIB_DUAL_LINK                   CACHE)
unset(WITH_SANITIZERS                  CACHE)
unset(WITH_MSAN                        CACHE)
unset(WITH_FUZZERS                     CACHE)
unset(WITH_OPTIM                       CACHE)
unset(WITH_NEW_STRATEGIES              CACHE)
unset(WITH_NATIVE_INSTRUCTIONS         CACHE)
unset(WITH_MAINTAINER_WARNINGS         CACHE)
unset(WITH_CODE_COVERAGE               CACHE)
unset(WITH_INFLATE_STRICT              CACHE)
unset(WITH_INFLATE_ALLOW_INVALID_DIST  CACHE)
unset(WITH_UNALIGNED                   CACHE)
unset(WITH_ACLE                        CACHE)
unset(WITH_NEON                        CACHE)
unset(WITH_POWER8                      CACHE)
unset(WITH_DFLTCC_DEFLATE              CACHE)
unset(WITH_DFLTCC_INFLATE              CACHE)
unset(WITH_AVX2                        CACHE)
unset(WITH_SSE2                        CACHE)
unset(WITH_SSSE3                       CACHE)
unset(WITH_SSE4                        CACHE)
unset(WITH_PCLMULQDQ                   CACHE)
unset(INSTALL_UTILS                    CACHE)
