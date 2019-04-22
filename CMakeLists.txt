cmake_minimum_required(VERSION 3.14)

project(LibDMG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CEREAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/externals/cereal-1.2.2/include)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

###############################################################################
# Library core                                                                #
###############################################################################
set(LIBDMG_CORE_NAME "core")
set(LIBDMG_CORE_SRC_DIR ${CMAKE_SOURCE_DIR}/src/core)
set(LIBDMG_CORE_SRCS ${LIBDMG_CORE_SRC_DIR}/emulator.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cart/cart.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cpu/cpu.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cpu/cpu_instr.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cpu/cpu_instr_ld8.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cpu/cpu_instr_ld16.cpp
                     ${LIBDMG_CORE_SRC_DIR}/cpu/cpu_instr_alu8.cpp
                     ${LIBDMG_CORE_SRC_DIR}/mem/mem_controller_rom_only.cpp
                     ${LIBDMG_CORE_SRC_DIR}/mem/boot_rom.cpp
                     ${LIBDMG_CORE_SRC_DIR}/peripherals/peripherals.cpp
                     ${LIBDMG_CORE_SRC_DIR}/peripherals/lcd_controller.cpp
                     ${LIBDMG_CORE_SRC_DIR}/peripherals/timer.cpp
                     ${LIBDMG_CORE_SRC_DIR}/logger.cpp)
add_library("${LIBDMG_CORE_NAME}" STATIC ${LIBDMG_CORE_SRCS})
target_include_directories(${LIBDMG_CORE_NAME} PRIVATE ${CEREAL_INCLUDE_DIR} 
                                                       ${LIBDMG_CORE_SRC_DIR})

###############################################################################
# Tests                                                                       #
###############################################################################
set(LIBDMG_TESTS_NAME "run_tests")
set(LIBDMG_TESTS_SRC_DIR ${CMAKE_SOURCE_DIR}/src/tests)
set(LIBDMG_TESTS_SRCS ${LIBDMG_TESTS_SRC_DIR}/run_all_tests.cpp
                      ${LIBDMG_TESTS_SRC_DIR}/test_emulator.cpp)
add_executable("${LIBDMG_TESTS_NAME}" ${LIBDMG_TESTS_SRCS})
target_include_directories(${LIBDMG_TESTS_NAME} PRIVATE ${LIBDMG_CORE_SRC_DIR} ${CEREAL_INCLUDE_DIR})
target_link_libraries(${LIBDMG_TESTS_NAME} ${LIBDMG_CORE_NAME} gtest_main)