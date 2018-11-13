# Copyright (C) 2016-2018 |Meso|Star>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0)
project(tinyexpr C)
enable_testing()

option(LIB_ONLY "Do not compile the test pograms nor the benchmark" OFF)

set(TINYEXPR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/..)

include_directories(${TINYEXPR_SOURCE_DIR})

################################################################################
# Configure and define targets
################################################################################
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

if(CMAKE_COMPILER_IS_GNUCC)
  set(MATH_LIB m)
endif()

if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
elseif(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wcast-align -Wmissing-declarations -Wmissing-prototypes -Wconversion -Wno-long-long -pedantic")
endif()

ADD_LIBRARY(tinyexpr STATIC
  ${TINYEXPR_SOURCE_DIR}/tinyexpr.c ${TINYEXPR_SOURCE_DIR}/tinyexpr.h)
set_target_properties(tinyexpr PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR})
if(CMAKE_COMPILER_IS_GNUCC)
  set_target_properties(tinyexpr PROPERTIES COMPILE_FLAGS "-std=c99")
endif()
target_link_libraries(tinyexpr ${MATH_LIB})

################################################################################
# Define tests
################################################################################
if(NOT LIB_ONLY)

  function(build_test _name)
    add_executable(${_name} ${TINYEXPR_SOURCE_DIR}/${_name}.c)
    if(CMAKE_COMPILER_IS_GNUCC)
      set_target_properties(${_name} PROPERTIES COMPILE_FLAGS "-std=c99")
      set(MATH_LIB m)
    endif()
    target_link_libraries(${_name} ${MATH_LIB})
    set(_libraries ${ARGN})
    foreach(_lib ${_libraries})
      target_link_libraries(${_name} ${_lib})
    endforeach()
  endfunction()

  function(new_test _name)
    build_test(${_name} ${ARGN})
    add_test(${_name} ${_name})
  endfunction()
  
  # create files variants
  file(READ ${TINYEXPR_SOURCE_DIR}/tinyexpr.c tinyexpr_c_file_content)
  file(WRITE ${PROJECT_BINARY_DIR}/tinyexpr_pow_right.c
"#define TE_POW_FROM_RIGHT 1
#define TE_NAT_LOG 1
${tinyexpr_c_file_content}")

  file(READ ${TINYEXPR_SOURCE_DIR}/test.c test_c_file_content)
  file(WRITE ${TINYEXPR_SOURCE_DIR}/test_pow_left.c "${test_c_file_content}")
  file(WRITE ${TINYEXPR_SOURCE_DIR}/test_pow_right.c
"#define TE_POW_FROM_RIGHT 1
#define TE_NAT_LOG 1
${test_c_file_content}")

  # create tinyexpr lib variant
  ADD_LIBRARY(tinyexpr_pow_right STATIC
    ${PROJECT_BINARY_DIR}/tinyexpr_pow_right.c ${TINYEXPR_SOURCE_DIR}/tinyexpr.h)
  set_target_properties(tinyexpr_pow_right PROPERTIES VERSION ${VERSION})
  if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(tinyexpr_pow_right PROPERTIES COMPILE_FLAGS "-std=c99")
  endif()
  target_link_libraries(tinyexpr_pow_right ${MATH_LIB})

  # tests
  new_test(example tinyexpr)
  build_test(example2 tinyexpr)
  add_test(example2_347 example2 "x+y" 3 4 7)
  add_test(example2_310 example2 "-5+x+2*y" 3 1 0)
  build_test(example2off tinyexpr)
  add_test(example2off_347 example2 "x+y" 3 4 7)
  add_test(example2off_310 example2 "-5+x+2*y" 3 1 0)
  new_test(example3 tinyexpr)
  new_test(test_pow_left tinyexpr)
  new_test(test_pow_right tinyexpr_pow_right)
  build_test(benchmark tinyexpr)
  add_test(benchmark_100 benchmark 100)

endif(NOT LIB_ONLY)
