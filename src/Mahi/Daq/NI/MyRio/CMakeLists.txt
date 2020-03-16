target_sources(daq
    PRIVATE
    MyRio.cpp
    MyRioConnector.cpp
    MyRioAI.cpp
    MyRioAO.cpp
    MyRioDI.cpp
    MyRioDO.cpp
    MyRioEncoder.cpp
    # MyRioI2C.cpp
    MyRioUtils.cpp
    MyRioUtils.hpp
    Detail/MyRioFpga60/IRQConfigure.c
    Detail/MyRioFpga60/IRQConfigure.h
    Detail/MyRioFpga60/libvisa.so
    Detail/MyRioFpga60/MyRio.c
    Detail/MyRioFpga60/MyRio.h
    Detail/MyRioFpga60/MyRio1900.h
    Detail/MyRioFpga60/MyRio1950.h
    Detail/MyRioFpga60/NiFpga.c
    Detail/MyRioFpga60/NiFpga.h
    Detail/MyRioFpga60/NiFpga_MyRio1900Fpga60.h
    Detail/MyRioFpga60/NiFpga_MyRio1950Fpga60.h
    Detail/MyRioFpga60/visa.h
    Detail/MyRioFpga60/visatype.h
)

target_compile_definitions(daq PUBLIC MAHI_MYRIO PRIVATE MyRio_1900) # MyRio_1900 needed by NI src
