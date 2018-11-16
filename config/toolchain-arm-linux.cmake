
#
# Copyright © 2018 Moxa Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)    # This one is important
SET(CMAKE_SYSTEM_VERSION 1)     # This one not so much

# This is the location of the amd64 toolchain targeting the device
SET(CMAKE_C_COMPILER $ENV{DIR_ENV}/bin/arm-linux-gnueabihf-gcc)

# This is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH $ENV{DIR_ENV}/arm-linux-gnueabihf)

# Search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

