macro(mahi_daq_example target)
    # create executable
    add_executable(${target} "ex_${target}.cpp")
    # set dependencies
    target_link_libraries(${target} mahi::daq)
    # add install rule
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    set_target_properties(${target} PROPERTIES FOLDER "Examples")
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
endmacro(mahi_daq_example)

# generic examples
mahi_daq_example(control)
mahi_daq_example(custom)
mahi_daq_example(perf)
mahi_daq_example(handles)

# quanser examples
if (MAHI_QUANSER)
    mahi_daq_example(q2usb)
    mahi_daq_example(q8usb)
    mahi_daq_example(qpid)
endif()

# sensoray examples
if (MAHI_SENSORAY826)
    mahi_daq_example(s826)
endif()

# myrio examples
if (MAHI_MYRIO)
    mahi_daq_example(myrio)
endif()