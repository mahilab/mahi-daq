<img src="https://github.com/mahilab/mahi-daq/blob/master/mahi-daq.png" width="256"> 

## mahi-daq

This library provides an object oriented hardware abstraction layer for commercial DAQs from Quanser, Sensoray, and National Instruments. 

### Integration

The library is small and intended to be copied to your build tree, or better yet, added as a `git` [submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

Suppose you already have a CMake project under `git` source control:

```shell
> cd my_project
my_project> git submodule add https://github.com/mahilab/mahi-daq
```

The `mahi-daq` directory will be cloned to the top level of `my_project`. Now, just add the following to your `CMakeLists.txt`:

```cmake
set(MAHI_DAQ_EXAMPLES OFF CACHE BOOL "" FORCE) # optional, ON by default
add_subdirectory(mahi-daq)
add_executable(my_app "my_app.cpp")
target_link_libraries(my_app mahi::daq)
```

That's it!
