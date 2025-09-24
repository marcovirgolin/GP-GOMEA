add_definitions(-DGPGOMEA)

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wno-unused-parameter -Wno-redundant-move"
)

add_subdirectory(Evolution)
add_subdirectory(Fitness)
add_subdirectory(Genotype)
add_subdirectory(GOMEA)
add_subdirectory(Operators)
add_subdirectory(RunHandling)
add_subdirectory(Selection)
add_subdirectory(Semantics)
add_subdirectory(Utils)
add_subdirectory(Variation)

set(GPGOMEA_LINK_LIBS ${ARMADILLO_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${OpenMP_CXX_FLAGS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ALL_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(ALL_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
  set(ALL_COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# Build Python package
add_library(gpgomea_python MODULE gpgomea_python.cpp ${ALL_OBJECT_FILES})
target_link_libraries(gpgomea_python ${GPGOMEA_LINK_LIBS})
target_include_directories(
  gpgomea_python PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
                        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Create C++ executable
add_executable(main main.cpp ${ALL_OBJECT_FILES})
target_link_libraries(main ${GPGOMEA_LINK_LIBS})
target_include_directories(
  main PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
              $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Construct RCPP Bindings
find_program(R_EXECUTABLE R)
if(NOT R_EXECUTABLE)
  message(FATAL_ERROR "R executable not found in PATH; cannot build R binding.")
else()
  # R/RCPP flags
  execute_process(COMMAND ${R_EXECUTABLE} CMD config --cppflags
                  OUTPUT_VARIABLE R_CPPFLAGS
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${R_EXECUTABLE} CMD config --ldflags
                  OUTPUT_VARIABLE R_LDFLAGS
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${R_EXECUTABLE} -s -e "Rcpp:::CxxFlags()"
                  OUTPUT_VARIABLE RCPP_CXXFLAGS
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${R_EXECUTABLE} -s -e "Rcpp:::LdFlags()"
                  OUTPUT_VARIABLE RCPP_LDFLAGS
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Fallback Rcpp files
  execute_process(COMMAND ${R_EXECUTABLE} RHOME
          OUTPUT_VARIABLE R_HOME
          OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${R_EXECUTABLE} -s -e "cat(system.file('include', package='Rcpp'))"
          OUTPUT_VARIABLE RCPP_INCLUDE_DIR
          OUTPUT_STRIP_TRAILING_WHITESPACE)

  message(STATUS "R_CPPFLAGS=${R_CPPFLAGS}")
  message(STATUS "R_LDFLAGS=${R_LDFLAGS}")
  message(STATUS "RCPP_CXXFLAGS=${RCPP_CXXFLAGS}")
  message(STATUS "RCPP_LDFLAGS=${RCPP_LDFLAGS}")

  add_library(gpgomea_rcpp MODULE gpgomea_rcpp.cpp ${ALL_OBJECT_FILES})
  target_link_libraries(gpgomea_rcpp ${ARMADILLO_LIBRARIES} ${OpenMP_CXX_FLAGS})
  target_include_directories(
    gpgomea_rcpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
                        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                       "${R_HOME}/include"                # R
                       "${RCPP_INCLUDE_DIR}")             # Rcpp (Rcpp.h)


  # apply R/Rcpp flags
  separate_arguments(R_CPPFLAGS_LIST NATIVE_COMMAND ${R_CPPFLAGS})
  separate_arguments(RCPP_CXXFLAGS_LIST NATIVE_COMMAND ${RCPP_CXXFLAGS})
  target_compile_options(gpgomea_rcpp PRIVATE ${R_CPPFLAGS_LIST} ${RCPP_CXXFLAGS_LIST})

  # String flags
  set_target_properties(gpgomea_rcpp PROPERTIES LINK_FLAGS "${R_LDFLAGS} ${RCPP_LDFLAGS}")

  # Standard Lib Name
  set_target_properties(gpgomea_rcpp PROPERTIES PREFIX "" SUFFIX ".so")
endif()
