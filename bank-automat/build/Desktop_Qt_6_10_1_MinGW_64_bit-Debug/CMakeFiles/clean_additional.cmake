# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\bank-automat_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\bank-automat_autogen.dir\\ParseCache.txt"
  "bank-automat_autogen"
  )
endif()
