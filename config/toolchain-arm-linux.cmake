# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)    # This one is important
SET(CMAKE_SYSTEM_VERSION 1)     # This one not so much

# This is the location of the amd64 toolchain targeting the device
SET(CMAKE_C_COMPILER $ENV{DIR_ENV}/bin/arm-linux-gnueabihf-gcc)

# This is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{DIR_ENV}/arm-linux-gnueabihf)

# This is the iothinxio include directory
SET(IOTHINXIO_INC_DIR $ENV{DIR_ENV}/arm-linux-gnueabihf/libc/usr/include)

# This is the iothinxio library directory
SET(IOTHINXIO_LIB_DIR $ENV{DIR_ENV}/arm-linux-gnueabihf/libc/usr/lib/iothinx)

# Search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
