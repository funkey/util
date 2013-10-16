option(ENABLE_DEBUG_LOGGING "Enable the 'debug' and 'all' log-channels" TRUE)

define_module(util LIBRARY LINKS boost curl INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/..)
