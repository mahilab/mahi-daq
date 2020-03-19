# mahi::daq CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Updated: 3/2020

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(MAHI_DAQ_EXAMPLES "Turn ON to build example executable(s)" ON)
else()
    option(MAHI_DAQ_EXAMPLES "Turn ON to build example executable(s)" OFF)
endif()

#===============================================================================
# FRONT MATTER
#===============================================================================

# create the project
project(mahi-daq VERSION 1.0.0 LANGUAGES C CXX)

# tell user they can't have shared version if they try
if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "mahi::daq does not support shared libaries")
endif()

# add ./cmake to CMake module path so our .cmake files can be found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs) # defines conventional GNU isntallation directories

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP")    # MULTICORE BUILDS
endif()

#===============================================================================
# MAHI UTIL
#===============================================================================

include(FetchContent) 
FetchContent_Declare(mahi-util GIT_REPOSITORY https://github.com/mahilab/mahi-util.git) 
FetchContent_MakeAvailable(mahi-util)

#===============================================================================
# CREATE LIBRARY
#===============================================================================

add_library(daq "")
add_library(mahi::daq ALIAS daq)
set_target_properties(daq PROPERTIES DEBUG_POSTFIX -d)
target_compile_features(daq PUBLIC cxx_std_11)
set_target_properties(daq PROPERTIES OUTPUT_NAME "mahi-daq")

# defines
target_compile_definitions(daq PUBLIC MAHI_DAQ) # for compatibility checks

# add source files
add_subdirectory(src/mahi/daq)

# add include files
file(GLOB_RECURSE MAHI_DAQ_HEADERS "include/*.hpp" "include/*.h" "include/*.inl")
target_sources(daq PRIVATE ${MAHI_DAQ_HEADERS}) # for intellisense
target_include_directories(daq
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# link libraries
target_link_libraries(daq PUBLIC mahi::util)

#===============================================================================
# WINDOWS ONLY
#===============================================================================

if(WIN32)

    target_compile_definitions(daq
        PUBLIC
            -D_CRT_SECURE_NO_WARNINGS          # remove secure warnings (e.g sprinft_s)
            -DNOMINMAX                         # remove min/max macros
            -D_WINSOCK_DEPRECATED_NO_WARNINGS  # remove winsock deprecated warnings
    )

    # the following only compile under MSVC
    if (MSVC)

        #=======================================================================
        # HIL / QUANSER
        #=======================================================================

        find_package(HIL)
        if (HIL_FOUND)
            option(MAHI_QUANSER "Turn ON to build Quanser support"  ON) 
        else()
            option(MAHI_QUANSER "Turn ON to build Quanser support"  OFF) 
        endif()
        if (MAHI_QUANSER)
            add_subdirectory(src/mahi/daq/Quanser)
        endif()

        #=======================================================================
        # SENSORAY 826
        #=======================================================================

        find_package(Sensoray826)
        if (Sensoray826_FOUND)
            option(MAHI_SENSORAY826 "Turn ON to build Sensoray 826 support" ON)  
        else()
            option(MAHI_SENSORAY826 "Turn ON to build Sensoray 826 support" OFF)
        endif() 
        if (MAHI_SENSORAY826)
            add_subdirectory(src/mahi/daq/Sensoray)
        endif()

        #=======================================================================
        # NI DAQMX
        #=======================================================================
        find_package(NIDAQmx)
        if (NIDAQmx_FOUND)
            option(MAHI_NIDAQMX "Turn ON to build NI DAQmx support" ON)  
        else()
            option(MAHI_NIDAQMX "Turn ON to build NI DAQmx support" OFF)
        endif() 
        if(MAHI_NIDAQMX)
            message("mahi::daq does not yet support NI DAQmx. This option serves as placeholder for future work.")
        endif()

    endif(MSVC)

endif(WIN32)

#===============================================================================
# NI LINUX REAL-TIME ONLY
#===============================================================================

if (NI_LRT)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    target_link_libraries(daq
        PRIVATE
            dl # Dynamic Linking Library
            rt # Realtime Extensions library
    )
    if (CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
        set(MAHI_MYRIO ON)
        add_subdirectory(src/mahi/daq/NI/MyRio)
    endif()
endif()

#===============================================================================
# EXAMPLE EXECUTABLES
#===============================================================================

if(MAHI_DAQ_EXAMPLES)
    message("Building mahi::daq examples")
    add_subdirectory(examples)
endif()

#===============================================================================
# INSTALL
#===============================================================================

install(TARGETS daq EXPORT mahi-daq-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MAHI_INSTALL_POSTFIX "") # 64 bit Windows is default
    else()
        set(MAHI_INSTALL_POSTFIX "-x86")
    endif()
elseif(NI_LRT AND CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
    set(MAHI_INSTALL_POSTFIX "-nilrt-arm")
elseif(NI_LRT AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MAHI_INSTALL_POSTFIX "-nilrt-x64")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}${MAHI_INSTALL_POSTFIX}")

# install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# set where we want to install our config
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/mahi-daq)

# export the targets to a script
install(EXPORT mahi-daq-targets
	FILE
		mahi-daq-targets.cmake
	NAMESPACE
		mahi::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

# include helper functions for creating config files that can be included by other projects to find and use a package
include(CMakePackageConfigHelpers)

# generate a package configuration file and a package version file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/mahi-daq-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-config.cmake
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

# install the config and configversion modules
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-config-version.cmake
	DESTINATION ${INSTALL_CONFIGDIR}
)

# install find modules
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindHIL.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindSensoray826.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindNIDAQmx.cmake
    DESTINATION ${INSTALL_CONFIGDIR}/Modules
)

# export from the build tree
export(EXPORT mahi-daq-targets
	   NAMESPACE mahi::
	   FILE ${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-targets.cmake)

