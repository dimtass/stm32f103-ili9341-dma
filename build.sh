#!/bin/bash -e

echo "Building the project in Linux environment"

# select architecture
: ${ARCHITECTURE:=stm32}

# select to create eclipse project files
: ${ECLIPSE_IDE:=false}

# select to clean previous builds
: ${CLEANBUILD:=false}

# Compile objects in parallel, the -jN flag in make
: ${PARALLEL:=$(expr $(getconf _NPROCESSORS_ONLN) + 1)}

# Current working directory
: ${WORKING_DIR:=$(pwd)}

# default generator
: ${IDE_GENERATOR:="Unix Makefiles"}

# cmake scripts folder
: ${SCRIPTS_CMAKE:=${WORKING_DIR}/cmake}

# select cmake toolchain
: ${CMAKE_TOOLCHAIN:=TOOLCHAIN_arm_none_eabi_cortex_m3.cmake}

if [ "${ECLIPSE}" == "true" ]; then
	IDE_GENERATOR="Eclipse CDT4 - Unix Makefiles" 
fi

BUILD_ARCH_DIR=${WORKING_DIR}/build-${ARCHITECTURE}

if [ "${ARCHITECTURE}" == "stm32" ]; then
    CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_TOOLCHAIN_FILE=${SCRIPTS_CMAKE}/${CMAKE_TOOLCHAIN}"

else
    >&2 echo "*** Error: Architecture '${ARCHITECTURE}' unknown."
    exit 1
fi

if [ "${CLEANBUILD}" == "true" ]; then
    echo "- removing build directory: ${BUILD_ARCH_DIR}"
    rm -rf ${BUILD_ARCH_DIR}
fi

echo "--- Pre-cmake ---"
echo "architecture      : ${ARCHITECTURE}"
echo "distclean         : ${CLEANBUILD}"
echo "parallel          : ${PARALLEL}"
echo "cmake flags       : ${CMAKE_FLAGS}"
echo "cmake scripts     : ${SCRIPTS_CMAKE}"
echo "IDE generator     : ${IDE_GENERATOR}"
echo "Threads           : ${PARALLEL}"

cd source

# setup cmake
cmake -G"${IDE_GENERATOR}" -H. -B${BUILD_ARCH_DIR} ${CMAKE_FLAGS}

# build
make -C ${BUILD_ARCH_DIR} -j${PARALLEL} --no-print-directory
