target_sources(mahi-daq
    PRIVATE
    S826.cpp
    S826AI.cpp
    S826AO.cpp
    S826DIO.cpp
    S826Encoder.cpp
    S826Watchdog.cpp
)

target_link_libraries(mahi-daq PRIVATE Sensoray::Sensoray826)
target_compile_definitions(mahi-daq PUBLIC MAHI_SENSORAY)