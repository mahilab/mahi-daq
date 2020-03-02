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

if (MAHI_QUANSER)
    mahi_daq_example(q8usb)
endif()