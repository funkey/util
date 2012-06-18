# find all files constituting this library
file(GLOB utilfiles
  *.cpp)

# this is a library
add_library(util ${utilfiles})
