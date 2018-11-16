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

# Default is right to left pow and log is natural log
# Build tinyExpr without closure support and with function_1 to function_3 support only
# (these ones cannot be disabled as predefined functions require them).
set(TE_DEFAULTS
  "-DTE_POW_FROM_RIGHT -DTE_NAT_LOG -DTE_WITHOUT_CLOSURES -DTE_WITHOUT_FUNCTION_0 -DTE_MAX_FUNCTION_ARITY=3")
# Settings to allow other code branches to be tested (cannot be the empty string)
set(TE_NON_DEFAULTS "-DTE_POW_FROM_LEFT -DTE_LOG_10") # Meaningless defines

ADD_LIBRARY(tinyexpr STATIC
  ${TINYEXPR_SOURCE_DIR}/tinyexpr.c ${TINYEXPR_SOURCE_DIR}/tinyexpr.h)
set_target_properties(tinyexpr PROPERTIES
  # Default is left to right pow and log is natural log
  # Build tinyExpr without closure support and with function_1 to function_3 only
  COMPILE_FLAGS ${TE_DEFAULTS}
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

  function(build_test _name _source _lib)
    add_executable(${_name}
	  ${TINYEXPR_SOURCE_DIR}/${_source}.c
	  ${TINYEXPR_SOURCE_DIR}/minctest.h)
	# additional args are compile flags
    set_target_properties(${_name} PROPERTIES COMPILE_FLAGS "${ARGN}")
    if(CMAKE_COMPILER_IS_GNUCC)
      set_target_properties(${_name} PROPERTIES COMPILE_FLAGS "-std=c99")
      set(MATH_LIB m)
    endif()
    target_link_libraries(${_name} ${_lib} ${MATH_LIB})
  endfunction()

  function(new_test _name _source _lib)
    build_test(${_name} ${_source} ${_lib} ${ARGN})
    add_test(${_name} ${_name})
  endfunction()
  
  # create tinyexpr lib variant to check all the stuff not checked by tinyexpr.lib
  ADD_LIBRARY(tinyexpr_others STATIC
    ${TINYEXPR_SOURCE_DIR}/tinyexpr.c ${TINYEXPR_SOURCE_DIR}/tinyexpr.h)
  set_target_properties(tinyexpr_others PROPERTIES
    VERSION ${VERSION}
    COMPILE_FLAGS ${TE_NON_DEFAULTS})
  if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(tinyexpr_others PROPERTIES COMPILE_FLAGS "-std=c99")
  endif()
  target_link_libraries(tinyexpr_others ${MATH_LIB})

  # tests
  new_test(example example tinyexpr ${TE_DEFAULTS})
  build_test(example2 example2 tinyexpr ${TE_DEFAULTS})
  add_test(example2_347 example2 "x+y" 3 4 7)
  add_test(example2_310 example2 "-5+x+2*y" 3 1 0)
  build_test(example2off example2off tinyexpr ${TE_DEFAULTS})
  add_test(example2off_347 example2off "x+y" 3 4 7)
  add_test(example2off_310 example2off "-5+x+2*y" 3 1 0)
  new_test(example3 example3 tinyexpr ${TE_DEFAULTS})
  build_test(benchmark benchmark tinyexpr ${TE_DEFAULTS})
  new_test(test_pow_defaults test tinyexpr ${TE_DEFAULTS})
  # Check all the stuff not checked by test_pow_defaults
  new_test(test_pow_others test tinyexpr_others ${TE_NON_DEFAULTS})
  add_test(benchmark_100 benchmark 100)

endif(NOT LIB_ONLY)
