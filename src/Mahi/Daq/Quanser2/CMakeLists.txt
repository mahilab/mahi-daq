target_sources(daq
    PRIVATE
    QuanserDaq.cpp
    QuanserAI.cpp
    QuanserAO.cpp
    QuanserDI.cpp
    QuanserDO.cpp
    QuanserEncoder.cpp
    QuanserOther.cpp
    QuanserWatchdog.cpp
    QuanserOptions.cpp
    QuanserUtils.cpp
    Q8Usb.cpp
)

target_link_libraries(daq
PUBLIC
    HIL::HIL                 # HIL SDK
    HIL::quanser_runtime     # HIL quanser_runtime
    HIL::quanser_common      # HIL quanser_common
    legacy_stdio_definitions # needed for _vsnprintf in quanser_common
)
target_compile_definitions(daq PUBLIC MAHI_QUANSER)