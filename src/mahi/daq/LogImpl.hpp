#pragma once

struct SimpleLog {

};

#if MAHI_DAQ_OUTPUT_LOGS
    #ifdef MAHI_LOG
        #include <mahi/log/Log.hpp>
    #else
        #include <iostream>
        #define LOG(severity) std::cout << std::endl << #severity << ": "
    #endif
#else
    #include <iostream>
    #define LOG(severity) if (true) { } else std::cout 
#endif