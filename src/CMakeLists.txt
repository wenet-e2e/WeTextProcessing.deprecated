cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(wenet_text_processing VERSION 0.1)

# This option makes it easy to use the wenet-text-processing runtime as a sub-project
# in a CMake build, with another project as the top-level project (i.e., wenet).
# When using wenet-text-processibng as a sub-project, we should set THRAX=OFF
# since libthrax is not a dependence to libtext_processor and we assume libfst
# has been built in top-level project.
option(THRAX "whether to build thrax" OFF)

if(THRAX)
  # include_directories() will be called in the root CMakeLists.txt if THRAX=OFF,
  # Otherwise, call it in current project.
  include_directories(
    ${PROJECT_SOURCE_DIR}
  )
  set(CMAKE_VERBOSE_MAKEFILE on)

  include(FetchContent)
  include(ExternalProject)
  set(FETCHCONTENT_QUIET off)
  get_filename_component(fc_base "fc_base" REALPATH BASE_DIR "${PROJECT_SOURCE_DIR}")
  set(FETCHCONTENT_BASE_DIR ${fc_base})

  # Keep the same with openfst, -fPIC or -fpic
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -fPIC")

  # third_party openfst
  set(openfst_SOURCE_DIR ${fc_base}/openfst-src)
  set(openfst_BINARY_DIR ${fc_base}/openfst-build)
  set(openfst_PREFIX_DIR ${fc_base}/openfst-subbuild/openfst-populate-prefix)
  ExternalProject_Add(openfst
    URL               https://github.com/mjansche/openfst/archive/1.6.7.zip
    URL_HASH          SHA256=6ad450057ca4958de6c09358c66e2a137c5a1cbd2cb365b47866bf905fca72e2
    SOURCE_DIR        ${openfst_SOURCE_DIR}
    BINARY_DIR        ${openfst_BINARY_DIR}
    CONFIGURE_COMMAND ${openfst_SOURCE_DIR}/configure --prefix=${openfst_PREFIX_DIR}
                       "--enable-compact-fsts=yes --enable-compress=yes --enable-const-fsts=yes --enable-far=yes "
                       "--enable-grm=yes --enable-linear-fsts=yes --enable-lookahead-fsts=yes --enable-mpdt=yes "
                       "--enable-ngram-fsts=yes --enable-pdt=yes "
                       "LIBS=-lpthread"
    BUILD_COMMAND     make -j 4
  )
  link_directories(${openfst_PREFIX_DIR}/lib)
  include_directories(${openfst_SOURCE_DIR}/src/include)

  # third_party thrax
  set(thrax_SOURCE_DIR ${fc_base}/thrax-src)
  set(thrax_BINARY_DIR ${fc_base}/thrax-build)
  set(thrax_PREFIX_DIR ${fc_base}/thrax-subbuild/thrax-populate-prefix)
  ExternalProject_Add(thrax
    URL               http://www.openfst.org/twiki/pub/GRM/ThraxDownload/thrax-1.2.9.tar.gz
    URL_HASH          SHA256=3cfbc003609d208d50d98659166086d4a6dd341ce6697e16370f5f7ae414f2b0
    SOURCE_DIR        ${thrax_SOURCE_DIR}
    BINARY_DIR        ${thrax_BINARY_DIR}
    CONFIGURE_COMMAND ${thrax_SOURCE_DIR}/configure --prefix=${thrax_PREFIX_DIR}
                       "LDFLAGS=-L${openfst_PREFIX_DIR}/lib"
                       "CXXFLAGS=-I${openfst_SOURCE_DIR}/src/include"
                       "LIBS=-lpthread"
    BUILD_COMMAND     make -j 4
  )
  add_dependencies(thrax openfst)
  link_directories(${thrax_PREFIX_DIR}/lib)
  include_directories(${thrax_SOURCE_DIR}/src/include)
endif(THRAX)

# text_processor
add_library(text_processor STATIC
  text_processor/text_processor.cc
)
# We assume target openfst has been built in (top-level) CMake projects (i.e., wenet),
# so it can be directly linked to text_processor.
add_dependencies(text_processor openfst)
target_link_libraries(text_processor PUBLIC fst dl)

# binary
add_executable(text_process_main bin/text_process_main.cc)
target_link_libraries(text_process_main PUBLIC text_processor)
