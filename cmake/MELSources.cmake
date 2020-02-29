#===============================================================================
# HEADER FILES
#===============================================================================

# headers directory
set(ROOT_HEADERS_DIR "${CMAKE_SOURCE_DIR}/include/mahi")

# collections of common subdirectory headers
set(MEL_DAQ_HEADERS "")

# collections of platform/hardware specific headers
set(MEL_MYRIO_HEADERS "")
set(MEL_QUANSER_HEADERS "")
set(MEL_SENSORAY_HEADERS "")

# MEL DAQ
set(MEL_DAQ_HEADERS_DIR "${ROOT_HEADERS_DIR}/daq")
list(APPEND MEL_DAQ_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/ChannelBase.hpp"
    "${MEL_DAQ_HEADERS_DIR}/DaqBase.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Encoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Input.hpp"
    "${MEL_DAQ_HEADERS_DIR}/InputOutput.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Module.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Output.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Registry.hpp"
    "${MEL_DAQ_HEADERS_DIR}/VirtualDaq.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Watchdog.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Detail/ChannelBase.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Input.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/InputOutput.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Module.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Output.inl"
    "${MEL_DAQ_HEADERS_DIR}/Detail/Registry.inl"
)

list(APPEND MEL_MYRIO_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRio.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioAI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioAO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioConnector.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioDIO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioEncoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/NI/myRIO/MyRioI2C.hpp"
)

list(APPEND MEL_QUANSER_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/Quanser/Q2Usb.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/Q8Usb.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QPid.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserAI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserAO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDaq.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDIO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserDO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserEncoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserPwm.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserOptions.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Quanser/QuanserWatchdog.hpp"
)

list(APPEND MEL_SENSORAY_HEADERS
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826AI.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826AO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826DIO.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826Encoder.hpp"
    "${MEL_DAQ_HEADERS_DIR}/Sensoray/S826Watchdog.hpp"
)

#===============================================================================
# SOURCE FILES
#===============================================================================

# source directory
set(ROOT_SRC_DIR "${CMAKE_SOURCE_DIR}/src/mahi")

# collections of subdirectory sources
set(MEL_DAQ_SRC "")

# collections of platform/hardware specific sources
set(MEL_QUANSER_SRC "")
set(MEL_SENSORAY_SRC "")
set(MEL_MYRIO_SRC "")

# MEL Base
# (no source files yet)

# MEL DAQ
set(MEL_DAQ_SRC_DIR "${ROOT_SRC_DIR}/daq")
list(APPEND MEL_DAQ_SRC
    "${MEL_DAQ_SRC_DIR}/DaqBase.cpp"
    "${MEL_DAQ_SRC_DIR}/Encoder.cpp"
    "${MEL_DAQ_SRC_DIR}/Module.cpp"
    "${MEL_DAQ_SRC_DIR}/Registry.cpp"
    "${MEL_DAQ_SRC_DIR}/VirtualDaq.cpp"
    "${MEL_DAQ_SRC_DIR}/Watchdog.cpp"
)

list(APPEND MEL_QUANSER_SRC
    "${MEL_DAQ_SRC_DIR}/Quanser/Q2Usb.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/Q8Usb.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QPid.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserAI.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserAO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDaq.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDI.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDIO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserDO.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserEncoder.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserPwm.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserOptions.cpp"
    "${MEL_DAQ_SRC_DIR}/Quanser/QuanserWatchdog.cpp"
)

list(APPEND MEL_SENSORAY_SRC
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826.cpp"
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826AI.cpp"
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826AO.cpp"
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826DIO.cpp"
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826Encoder.cpp"
    "${MEL_DAQ_SRC_DIR}/Sensoray/S826Watchdog.cpp"
)

list(APPEND MEL_MYO_SRC
    "${MEL_DEVICES_SRC_DIR}/Myo/MyoBand.cpp"
)