target_sources(daq
    PRIVATE
    Q2Usb.cpp
    Q8Usb.cpp
    QPid.cpp
    QuanserAI.cpp
    QuanserAO.cpp
    QuanserDaq.cpp
    QuanserDI.cpp
    QuanserDIO.cpp
    QuanserDO.cpp
    QuanserEncoder.cpp
    QuanserPwm.cpp
    QuanserOptions.cpp
    QuanserWatchdog.cpp
)

target_link_libraries(daq
PRIVATE
    HIL::HIL                 # HIL SDK
    HIL::quanser_runtime     # HIL quanser_runtime
    HIL::quanser_common      # HIL quanser_common
    legacy_stdio_definitions # needed for _vsnprintf in quanser_common
)
target_compile_definitions(daq PUBLIC MAHI_QUANSER)