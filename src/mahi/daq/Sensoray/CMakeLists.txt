target_sources(daq
    PRIVATE
    S826.cpp
    S826AI.cpp
    S826AO.cpp
    S826DIO.cpp
    S826Encoder.cpp
    S826Watchdog.cpp
)

target_link_libraries(daq PRIVATE Sensoray::Sensoray826)
target_compile_definitions(daq PUBLIC MAHI_SENSORAY)