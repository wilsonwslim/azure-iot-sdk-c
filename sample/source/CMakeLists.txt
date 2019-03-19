# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

compileAsC99()

if(NOT ${use_mqtt})
    message(FATAL_ERROR "moxa_sample_mqtt being generated without mqtt support")
endif()

set(
    moxa_sample_mqtt_c_files
    moxa_sample_mqtt.c
)

#Conditionally use the SDK trusted certs in the samples
if(${use_sample_trusted_cert})
    add_definitions(-DSET_TRUSTED_CERT_IN_SAMPLES)
    include_directories(${PROJECT_SOURCE_DIR}/certs)
    set(
        moxa_sample_mqtt_c_files
        ${moxa_sample_mqtt_c_files}
        ${PROJECT_SOURCE_DIR}/certs/certs.c
    )
endif()

set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,/usr/lib/iothinx"
)

find_library(
    IOTHINXIO_LIB
    NAMES iothinxio
    HINTS ${IOTHINXIO_LIB_DIR}
)

include_directories(
    ${IOTHUB_CLIENT_INC_FOLDER}
    ${SHARED_UTIL_INC_FOLDER}
    ${IOTHINXIO_INC_DIR}
)

add_executable(
    moxa_sample_mqtt
    ${moxa_sample_mqtt_c_files}
)

target_link_libraries(
    moxa_sample_mqtt
    iothub_client
    iothub_client_mqtt_transport
    serializer
    ${IOTHINXIO_LIB}
)
