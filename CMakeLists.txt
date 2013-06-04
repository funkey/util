# find all files constituting this library
file(GLOB utilfiles
  *.cpp)

# this is a library
add_library(util ${utilfiles})

option(ENABLE_DEBUG_LOGGING "Enable the 'debug' and 'all' log-channels" TRUE)
