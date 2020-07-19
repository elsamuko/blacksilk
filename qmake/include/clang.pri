QMAKE_CC  = /usr/bin/clang
QMAKE_CXX = /usr/bin/clang++

QMAKE_LINK       = $$QMAKE_CXX
QMAKE_LINK_SHLIB = $$QMAKE_CXX

CONFIG += clang_pch_style
QMAKE_PCH_OUTPUT_EXT = .pch

QMAKE_CFLAGS_PRECOMPILE          = -x c-header -c ${QMAKE_PCH_INPUT} -o ${QMAKE_PCH_OUTPUT}
QMAKE_CFLAGS_USE_PRECOMPILE      = -Xclang -include-pch -Xclang ${QMAKE_PCH_OUTPUT}
QMAKE_CXXFLAGS_PRECOMPILE        = -x c++-header -c ${QMAKE_PCH_INPUT} -o ${QMAKE_PCH_OUTPUT}
QMAKE_CXXFLAGS_USE_PRECOMPILE    = $$QMAKE_CFLAGS_USE_PRECOMPILE

QMAKE_CXXFLAGS_RELEASE += -msse2 -O3 -finline
QMAKE_CXXFLAGS_DEBUG += -std=c++0x
QMAKE_LFLAGS_DEBUG +=