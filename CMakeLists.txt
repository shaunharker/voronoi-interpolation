cmake_minimum_required(VERSION 3.18...3.26)
project(voronoi_interpolation)

if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build'. Running
  it directly will almost certainly not produce the desired result. If
  you are a user trying to install this package, please use the command
  below, which will install all necessary build dependencies, compile
  the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ python setup.py install
  =====================================================================")
endif()

# project
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_library(GMP_LIBRARY NAMES gmp libgmp)
if (NOT GMP_LIBRARY)
  message(FATAL_ERROR "GMP library not found")
endif()

# Perform a release build by default
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Create CMake targets for Python components needed by nanobind
find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

# Determine the nanobind CMake include path and register it
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")

# Import nanobind through CMake's find_package mechanism
find_package(nanobind CONFIG REQUIRED)

# Compile project code with current optimization mode configured in CMake
add_library(external_lib STATIC src/external.cpp)

# Need position independent code (-fPIC) to link into 'example_ext' below
set_target_properties(external_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Compile extension module with size optimization and add 'example_lib'
nanobind_add_module(_voronoi_interpolation src/voronoi_interpolation.cpp)
target_link_libraries(_voronoi_interpolation PRIVATE external_lib ${GMP_LIBRARY})

# install
install(TARGETS _voronoi_interpolation LIBRARY DESTINATION .)
