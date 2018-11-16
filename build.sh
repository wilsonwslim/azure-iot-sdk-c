#!/bin/bash
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


DIR_WRK=$(dirname $(readlink -e $0))

DST_SDK=$DIR_WRK/output/sdk_azure
export DIR_ENV=$DIR_WRK/output/env_azure

TOOLCHAIN_FILE=$DIR_WRK/config/toolchain-arm-linux.cmake
SYSROOT=$DIR_ENV/arm-linux-gnueabihf

function sdk_build()
{
    echo "-- Process: $FUNCNAME ..."
    $DST_SDK/build_all/linux/build.sh --toolchain-file $TOOLCHAIN_FILE -cl --sysroot=$SYSROOT
    echo "-- Process: $FUNCNAME DONE!!"
}

sdk_build

