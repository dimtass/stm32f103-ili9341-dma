cmake_minimum_required(VERSION 3.4)
project(stm32f103-ili9341-dma)

# force stm builds to debug.  This is a hack as when release is set, it automatically 
# sets the invalid -O3 flag on the assembler.
set(CMAKE_BUILD_TYPE Debug)

# Resolve the issue with Eclipse's indexer and C/C++ mixed source files
include(../cmake/language_provider.cmake)
remove_from_SYSTEM_MACROS(CXX "__cplusplus;.*;")
create_language_provider("${CMAKE_BINARY_DIR}/.settings" "${GCC_PREFIX}-" "${CXX_STANDARD_FLAG}")

# add startup and linker file
set(LIBS libs)
set(STARTUP ${LIBS}/startup/)

# Add libraries
include_directories(
        ${LIBS}/StdPeriph_Driver/inc/
        ${LIBS}/StdPeriph_Driver/src/
        ${LIBS}/CMSIS/core/
        ${LIBS}/CMSIS/device/
        ${LIBS}/ILI9341/
        ${LIBS}/ILI9341/fonts
        inc/
)
set(PERIPH_LIB_SRC ${LIBS}/StdPeriph_Driver/src/)
set(ILI9341_SRC ${LIBS}/ILI9341/)

set(SRC_FILES
        # STM32 standard periph drivers
        ${PERIPH_LIB_SRC}/misc.c
        ${PERIPH_LIB_SRC}/stm32f10x_flash.c
        ${PERIPH_LIB_SRC}/stm32f10x_rcc.c
        ${PERIPH_LIB_SRC}/stm32f10x_gpio.c
        ${PERIPH_LIB_SRC}/stm32f10x_spi.c
        ${PERIPH_LIB_SRC}/stm32f10x_dma.c
        ${PERIPH_LIB_SRC}/stm32f10x_usart.c
        # ILI9341 lib
        ${ILI9341_SRC}/ili9341_core.c
        ${ILI9341_SRC}/ili9341_graph.c
        ${ILI9341_SRC}/ili9341_spi.c
        ${ILI9341_SRC}/ili9341_text.c
        # Rest source files
        syscalls.c
        main.c
        cortexm_delay.c
        dev_spi.c
        dev_uart.c
        overclock_stm32f103.c
        stm32f10x_it.c
        system_stm32f10x.c
)

# enable asm for stm startup.s file
enable_language(ASM)

set(PP_DEFINES " -DSTM32F1 -DSTM32F103C8Tx -DSTM32 -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD")
set(PP_DEFINES "${PP_DEFINES} -DIVECT_TAB_OFFSET=0x0")
set_source_files_properties(${SRC_FILES}
    PROPERTIES COMPILE_FLAGS ${PP_DEFINES}
)
# set compiler
set(COMPILER_OPTIONS "-Wall -Werror")
set(COMPILER_OPTIMISATION "-g -O2")
set(CMAKE_C_FLAGS   "${COMPILER_OPTIONS} ${COMPILER_OPTIMISATION} -std=c11")
set(STM32_FLAGS "-march=armv7-m -mcpu=cortex-m3 -mthumb -mlittle-endian -mthumb-interwork ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STM32_FLAGS} -lm -lc --specs=nano.specs -fmessage-length=0 -ffunction-sections")

# add startup file
set(STARTUP_ASM_FILE "${STARTUP}/startup_stm32f10x_md.s")
set_property(SOURCE ${STARTUP_ASM_FILE} PROPERTY LANGUAGE ASM)
# add linker file
set(LINKER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${LIBS}/LinkerScript.ld")
SET(CMAKE_EXE_LINKER_FLAGS "${STM32_FLAGS} ${COMPILER_OPTIONS} -Wl,-Map=linker.map -Wl,-cref -Wl,--gc-sections" CACHE INTERNAL "exe link flags")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_FILE}")

add_executable(${PROJECT_NAME}.elf ${STARTUP_ASM_FILE} ${SRC_FILES})

add_custom_target (${PROJECT_NAME}.hex ALL
    DEPENDS ${PROJECT_NAME}.elf 
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Generating ${PROJECT_NAME}.hex"
)
add_custom_target (${PROJECT_NAME}.bin ALL
    DEPENDS ${PROJECT_NAME}.elf 
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT "Generating ${PROJECT_NAME}.bin"
)

# print output files sizes
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=berkeley "${PROJECT_NAME}.elf")

add_custom_target(write st-flash --reset write ${BUILD_ARCH_DIR}/${PROJECT_NAME}.bin 0x8000000 DEPENDS ${PROJECT_NAME}.bin)

