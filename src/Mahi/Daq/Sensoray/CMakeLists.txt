target_sources(daq
    PRIVATE
    SensorayUtils.cpp
    S826.cpp
    S826AI.cpp
    S826AO.cpp
    S826Encoder.cpp
    S826Watchdog.cpp
)

target_link_libraries(daq PUBLIC Sensoray::Sensoray826)
target_compile_definitions(daq PUBLIC MAHI_SENSORAY)