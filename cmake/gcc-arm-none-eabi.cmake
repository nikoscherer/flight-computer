set(CMAKE_SYSTEM_NAME          Generic)
set(CMAKE_SYSTEM_PROCESSOR       arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Prevent CMake from trying to run a full system compiler test
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# --- CROSS-PLATFORM LOCAL TOOLCHAIN PATH CONFIGURATION ---
get_filename_component(TOOLCHAIN_BIN_DIR "${CMAKE_CURRENT_LIST_DIR}/../tools/gnu-arm-toolchain/bin" ABSOLUTE)

set(TOOLCHAIN_PREFIX "arm-none-eabi-")

# Automatically append .exe if building on Windows, leave blank for Linux/macOS
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(TOOLCHAIN_EXT ".exe")
else()
    set(TOOLCHAIN_EXT "")
endif()

set(CMAKE_C_COMPILER     "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_EXT}")
set(CMAKE_ASM_COMPILER   "${CMAKE_C_COMPILER}")
set(CMAKE_CXX_COMPILER   "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_EXT}")
set(CMAKE_LINKER         "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_EXT}")
set(CMAKE_OBJCOPY        "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_EXT}")
set(CMAKE_SIZE           "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN_PREFIX}size${TOOLCHAIN_EXT}")
# --------------------------------------------------

set(CMAKE_EXECUTABLE_SUFFIX_ASM    ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C      ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX    ".elf")

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32F446xx_FLASH.ld\"")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(TOOLCHAIN_LINK_LIBRARIES "m")

# Restrict file searches to the target toolchain directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)