#!/usr/bin/env bash
set -e

#
# This script takes care of installing all the dependencies required to
#  build the code. Read the comments below for more
#  specific details on what it does.
#
# REQUIREMENTS:
#  - This script only works on Debian-based flavors of Linux, e.g. Ubuntu.
#  - It's assumed that you're starting from a freshly cloned copy of
#     the repository.
#  - A working internet connection is needed to install any missing
# 	  packages as well as the git submodules.
#
#  If you're having problems cloning git repos, check that ssh-agent
#   has your private key loaded. For reference, see e.g.
#   https://help.github.com/articles/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent/
#

# Clone submodules
git submodule update --init --recursive

# Update package manager indices to latest available.
sudo apt-get update

sudo apt-get install build-essential cppcheck -y

# CMake is needed to actually build the system
sudo apt-get install cmake -y

# Clang compiler is needed
sudo apt-get install clang

# ZeroMQ is needed for communications between processes
sudo apt-get install libzmq3-dev

# Boost is a general purpose c++ library
sudo apt-get install libboost-all-dev

# Needed by cmake to find packages
sudo apt-get install pkg-config

# install ROS melodic
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
sudo apt update
sudo apt install ros-melodic-ros-base

INSTALL_DEPS_DIRECTORY=${BASH_SOURCE%/*}
LIB_DIRECTORY=${INSTALL_DEPS_DIRECTORY}/../lib

# Install protobuf locally
# inside the folder lib/protobuf/cmake/build
sudo apt-get install autoconf automake libtool curl make g++ unzip
SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
pushd $SCRIPTDIR/../lib/protobuf
./autogen.sh
./configure --prefix="$(pwd)/cmake/build"
make
if make check ; then
    make install
fi
popd
