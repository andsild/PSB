#
#  File        : CMakeLists.txt
#                ( Configuration file for 'cmake' utility )
#
#  Description : CMakeLists.txt configuration file for compiling CImg-based code.
#                This file is a part of the CImg Library project.
#                ( http://cimg.sourceforge.net )
#
#  Copyright   : Antonio Albiol
#                ( http://personales.upv.es/~aalbiol/ )
#
#  License     : CeCILL v2.0
#                ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
#
#  This software is governed by the CeCILL  license under French law and
#  abiding by the rules of distribution of free software.  You can  use,
#  modify and/ or redistribute the software under the terms of the CeCILL
#  license as circulated by CEA, CNRS and INRIA at the following URL
#  "http://www.cecill.info".
#
#  As a counterpart to the access to the source code and  rights to copy,
#  modify and redistribute granted by the license, users are provided only
#  with a limited warranty  and the software's author,  the holder of the
#  economic rights,  and the successive licensors  have only  limited
#  liability.
#
#  In this respect, the user's attention is drawn to the risks associated
#  with loading,  using,  modifying and/or developing or reproducing the
#  software by the user in light of its specific status of free software,
#  that may mean  that it is complicated to manipulate,  and  that  also
#  therefore means  that it is reserved for developers  and  experienced
#  professionals having in-depth computer knowledge. Users are therefore
#  encouraged to load and test the software's suitability as regards their
#  requirements in conditions enabling the security of their systems and/or
#  data to be ensured and,  more generally, to use and operate it in the
#  same conditions as regards security.
#
#  The fact that you are presently reading this means that you have had
#  knowledge of the CeCILL license and that you accept its terms.
#

cmake_minimum_required(VERSION 2.6)

PROJECT(PSB)


# Prevent compilation in-source
if( ${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
  Message( " " )
  Message( FATAL_ERROR "Source and build  directories are the same.
 Create an empty build directory,
 change into it and re-invoke cmake")
endif()


set(PACKAGE_BUGREPORT "andsild@gmail.com")


# To use PKG_CHECK_MODULES to find some optional packages
find_package(PkgConfig)

#    Tell CMake where to leave executables 
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/)

#Path of CImg.h file relative to this file path
set(CIMG_H_PATH ${PROJECT_SOURCE_DIR}/../lib/CImg-1.5.8)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake-modules")

option(WITH_FFTW3     "Use FFTW3 library" ON)

if(WITH_FFTW3)
  find_package( FFTW3 REQUIRED )
  add_definitions( -Dcimg_use_fftw3 )
  include_directories( ${FFTW3_INCLUDE_DIRS} )
  list( APPEND DEPENDENCIES_LIBRARIES ${FFTW3_LIBRARIES} )
endif(WITH_FFTW3)



file(GLOB_RECURSE Foo_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE Foo_HEADERS "${PROJECT_INCLUDE_DIR}/*.hpp")

set (Foo_INCLUDE_DIRS "")
foreach (_headerFile ${Foo_HEADERS})
        get_filename_component(_dir ${_headerFile} PATH)
            list (APPEND Foo_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS)
include_directories(${Foo_INCLUDE_DIRS})

# add debug tools?
#    OPTION (USE_DEBUG_TOOLS
#       "Include primitive debug tools for creating test images" OFF)
# 

# include_directories( ${PROJECT_SOURCE_DIR}/debug_tools )
include_directories( ${CIMG_H_PATH} )

find_program(GNUPLOT_BIN gnuplot)
if(NOT GNUPLOT_BIN)
    message (FATAL_ERROR "Could not find gnuplot")
endif(NOT GNUPLOT_BIN)

if(NOT WIN32)
  FIND_PACKAGE(X11)
  FIND_PACKAGE(Threads REQUIRED)
endif()

if(NOT APPLE)
  if(NOT WIN32)
    if(X11_FOUND)
      SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_XSHM_CCFLAGS} ${CIMG_XRANDR_CCFLAGS}")
      SET(SYSTEM_LIBS ${SYSTEM_LIBS} Xext Xrandr "-lX11 -L/usr/X11/R6/lib")
    endif()
  endif(NOT WIN32)
endif(NOT APPLE)

if(X11_FOUND)
  link_directories(${X11_LIB_DIRS})
  include_directories(${X11_INCLUDE_DIR})
  SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${X11_LIBRARIES}  )
endif()

if (NOT WIN32)
  SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
endif()

if( WIN32)
  SET( SYSTEM_LIBS  ${SYSTEM_LIBS}  gdi32 " -lgdi32")
endif()

if (OpenCV_FOUND)
  message("OpenCV Found")
  SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_OPENCV_CCFLAGS}")
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIB_DIRS})
  SET( SYSTEM_LIBS  ${SYSTEM_LIBS}  ${OpenCV_LIBS} )
endif()



if(LAPACK_FOUND)
  SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_LAPACK_CCFLAGS}")
  link_directories( ${LAPACK_LIBRARY_DIRS} )
  include_directories( ${LAPACK_INCLUDE_DIRS} )
  SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${LAPACK_LIBRARIES} )
endif()

if(BLAS_FOUND)
  SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_BLAS_CCFLAGS}")
  link_directories( ${BLAS_LIBRARY_DIRS} )
  include_directories( ${BLAS_INCLUDE_DIRS} )
  SET( SYSTEM_LIBS ${SYSTEM_LIBS} ${BLAS_LIBRARIES} )
endif()

# Add CIMG Flags to Compilation Flags
#TODO: remove debug
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CIMG_CFLAGS} -L/usr/X11/R6/lib")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CIMG_CFLAGS} -g -DLOGGING -std=c++11 -I${PROJECT_INCLUDE_DIR} -I. -L/usr/X11R6/lib -lfftw3 -lfftw3f -lX11 -lpthread -lm -lfftw3_threads ")


IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
ENDIF()

SET(CIMG_FILES main 
    #make_test_image
  )

foreach(program ${CIMG_FILES})
    add_executable(${program}.out ${Foo_SOURCES} ${program}.cpp)
    target_link_libraries(${program}.out ${SYSTEM_LIBS})
endforeach(program)
