cmake_minimum_required(VERSION 2.6)

#######################
# project information #
#######################

project(util_exampes)

#####################
# required packages #
#####################

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
message(STATUS ${CMAKE_MODULE_PATH})

find_package(Boost 1.42 COMPONENTS thread signals program_options REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
	message(STATUS "Boost found.")
else()
	message(STATUS "Boost *NOT* found.")
endif()

############
# all libs #
############

set(CORELIBS
  ${Boost_LIBRARIES}
)

###############
# config file #
###############

configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
)

#######################
# project directories #
#######################

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(util)

#####################
# compiler settings #
#####################

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG   "-g")

###############
# main target #
###############

add_executable(program_options program_options.cpp)
target_link_libraries(program_options util ${CORELIBS})
