# mahi::daq CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Updated: 2/2020

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MAHI_DAQ_EXAMPLES "Turn ON to build example executable(s)" OFF)

#===============================================================================
# FRONT MATTER
#===============================================================================

# create the project
project(mahi-daq VERSION 1.0.0 LANGUAGES C CXX)

# tell user they can't have shared version if they try
if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "MAHI does not yet support shared libaries")
endif()

# add ./cmake to CMake module path so our .cmake files can be found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Macros) # macros
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
# CREATE COMMON LIBRARY
#===============================================================================

# create core mahi::daq library
set(MAHI_DAQ_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/ChannelBase.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/DaqBase.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Encoder.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Input.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/InputOutput.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Output.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Registry.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/VirtualDaq.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Watchdog.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/ChannelBase.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/Input.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/InputOutput.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/Module.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/Output.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Detail/Registry.inl"
)

set(MAHI_DAQ_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/DaqBase.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Encoder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Registry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/VirtualDaq.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Watchdog.cpp"
)

mahi_daq_add_library(daq SOURCES ${MAHI_DAQ_SRC} ${MAHI_DAQ_HEADERS})

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

            set(MAHI_QAUNSER_HEADERS 
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/Q2Usb.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/Q8Usb.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QPid.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserAI.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserAO.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserDaq.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserDI.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserDIO.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserDO.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserEncoder.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserPwm.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserOptions.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Quanser/QuanserWatchdog.hpp"
            )

            set(MAHI_QUANSER_SRC
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/Q2Usb.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/Q8Usb.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QPid.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserAI.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserAO.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserDaq.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserDI.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserDIO.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserDO.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserEncoder.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserPwm.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserOptions.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Quanser/QuanserWatchdog.cpp"        
            )

            # create Quanser library module
            mahi_daq_add_library(quanser SOURCES ${MAHI_QUANSER_SRC} ${MAHI_QUANSER_HEADERS})
            target_link_libraries(quanser
                PUBLIC
                    mahi::daq
                PRIVATE
                    HIL::HIL                 # HIL SDK
                    HIL::quanser_runtime     # HIL quanser_runtime
                    HIL::quanser_common      # HIL quanser_common
                    legacy_stdio_definitions # needed for _vsnprintf in quanser_common
            )
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
            message("mahi::daq does not yet support NI DAQmx. This option serves as placeholder.")
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

            set(MAHI_SENSORAY_HEADERS
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826AI.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826AO.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826DIO.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826Encoder.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/Sensoray/S826Watchdog.hpp"
            )

            set(MAHI_SENSORAY_SRC
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826AI.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826AO.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826DIO.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826Encoder.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/Sensoray/S826Watchdog.cpp"
            )

            mahi_daq_add_library(sensoray SOURCES ${MAHI_SENSORAY_SRC} ${MAHI_SENSORAY_HEADERS})
            target_link_libraries(sensoray PUBLIC mahi::daq PRIVATE Sensoray::Sensoray826)
        endif()

    endif(MSVC)

endif(WIN32)

#===============================================================================
# NI LINUX REAL-TIME ONLY
#===============================================================================

# detect incompatible options
mahi_incompatible(NI_LRT MAHI_QUANSER)
mahi_incompatible(NI_LRT MAHI_SENSORAY826)
mahi_incompatible(NI_LRT MAHI_NIDAQMX)

if (NI_LRT)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    target_link_libraries(daq
        PRIVATE
            dl # Dynamic Linking Library
            rt # Realtime Extensions library
    )
    if (CMAKE_SYSTEM_PROCESSOR MATCHES ARM)

        set(MAHI_MYRIO_HEADERS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRio.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioAI.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioAO.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioConnector.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioDIO.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioEncoder.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/mahi/daq/NI/myRIO/MyRioI2C.hpp"
        )

        set(MAHI_MYRIO_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRio.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioAI.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioAO.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioConnector.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioDIO.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioEncoder.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/MyRioI2C.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioUtil.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioUtil.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/IRQConfigure.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/IRQConfigure.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/libvisa.so"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/MyRio.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/MyRio.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/MyRio1900.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/MyRio1950.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/NiFpga.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/NiFpga.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/NiFpga_MyRio1900Fpga60.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/NiFpga_MyRio1950Fpga60.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/visa.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/mahi/daq/NI/myRIO/Detail/MyRioFpga60/visatype.h"        
        )

        mahi_daq_add_library(myrio SOURCES ${MAHI_MYRIO_SRC} ${MAHI_MYRIO_HEADERS})
        target_link_libraries(myrio PUBLIC daq)
        target_compile_definitions(myrio PRIVATE MyRio_1900) # needed by NI src
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

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MAHI_INSTALL_POSTFIX "") # 64 bit Windows is default
    else()
        set(MAHI_INSTALL_POSTFIX "-x86")
    endif()
elseif(NI_LRT AND CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
    set(MAHI_INSTALL_POSTFIX "-nilrt-arm")
elseif(NI_LRT AND CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindNIDAQmx.cmake
    DESTINATION ${INSTALL_CONFIGDIR}/Modules
)

# export from the build tree
export(EXPORT mahi-daq-targets
	   NAMESPACE mahi::
	   FILE ${CMAKE_CURRENT_BINARY_DIR}/mahi-daq-targets.cmake)

