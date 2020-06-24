# Network Table
A communication hub (basically database) that runs on central controller on Ada 2.0.  
Receives updates on sensor data (from GPS, wind sensors, etc), allows other modules to connect
to the network table using pub/sub or request/reply.

[Network Table Communication Protocol](https://confluence.ubcsailbot.org/display/ADA2/Network+Table+Communication+Protocol)

## Install Dependencies
Clone all submodules:  
```git submodule update --init --recursive```  

Install these dependencies (all should be available via apt-get):  
```build-essential cppcheck cmake clang libzmq3-dev libboost-all-dev pkg-config```

The following dependencies are needed to build protobuf.
They're pretty common anyways, you probably have a lot of them:  
```autoconf automake libtool curl make g++ unzip```

Run this script to compile protobuf. Protobuf
will be installed locally in the lib folder.
WARNING, this script takes about 1 hour to run. Compiling protobuf
takes a long time.
```./scripts/install_protobuf.sh```

### ROS (Robot Operating System)
If ENABLE_ROS is set to ON in the top
level CMakeLists.txt, this builds an
extra executable which is ran on the
Intel NUC. Obviously you will need to have
ROS and catkin installed. Refer to the ROS
website to find out how to install it on your
computer.

## Compiling
In the root directory of the repository run these commands:
```bash
mkdir build
cd build
cmake ..
make
```

## Running
Executables can be found in `build/bin/`

## Style Guide, Linting, and Code Check
We follow [Google's C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

Run cpplint to check the code for style guideline violations.
```bash
./scripts/run_cpplint.sh
```

Run cppcheck to check code for possible bugs.
```bash
./scripts/run_cppcheck.sh
```

## Testing
##### Creating & Running Tests
Whenever you add new tests, you will need to add the required `.cpp` and `.h` files to the `TEST_FILES` parameter in `test/basic_tests/CMakelists.txt`.

For example:
```cmake
set(TEST_FILES
    example_tests/NewTest.cpp
    example_tests/NewTest.h)
```

To run the tests, use CMake to compile the project, then run `basic_tests` in `build/bin`.
It will notify you if tests pass or fail.

## Structure of the code
The directories `src` and `test/basic_tests` should mirror each other. That is, any unit testing code for the file `src/a/b.cpp` should be in `test/basic_tests/a/bTest.cpp`.

-   **src/** - Source code.
-   **test/** - Unit tests.
-   **projects/** - Target specific code. For more details, check out the README file in this directory.
