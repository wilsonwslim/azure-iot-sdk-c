// Copyright (C) 2019 Moxa Inc. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>
#include <iothinx/iothinxio.h>
#include "serializer.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothubtransportmqtt.h"

/*String containing Hostname, Device Id & Device Key in the format:               */
/*    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "[device connection string]";

// Define the Model
BEGIN_NAMESPACE(MOXA_SAMPLE);
DECLARE_MODEL(IOTHINX_IO,
              WITH_DATA(ascii_char_ptr, DeviceId),
              WITH_DATA(int, DI_Values),
              WITH_DATA(int, DO_Values),
              WITH_ACTION(DO_SetValues, int, DO_Values)
             );
END_NAMESPACE(MOXA_SAMPLE);

EXECUTE_COMMAND_RESULT DO_SetValues(IOTHINX_IO *iothinxio, int DO_Values)
{
    int rc = 0;
    uint32_t slot = 1;

    rc = ioThinx_DO_SetValues(slot, DO_Values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_SetValues() = %d\r\n", rc);
        return EXECUTE_COMMAND_ERROR;
    }

    return EXECUTE_COMMAND_SUCCESS;
}

static void send_Callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *usercontextcallback)
{
    printf("%s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    return;
}

static IOTHUBMESSAGE_DISPOSITION_RESULT recv_Callback(IOTHUB_MESSAGE_HANDLE iothubmessagehandle, void *usercontextcallback)
{
    int rc = 0;
    const unsigned char *buf = NULL;
    unsigned char *tmp = NULL;
    size_t buf_size = 0;

    rc = IoTHubMessage_GetByteArray(iothubmessagehandle, &buf, &buf_size);
    if (rc != IOTHUB_MESSAGE_OK)
    {
        printf("IoTHubMessage_GetByteArray() = %d\r\n", rc);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    }

    /* buf is not zero terminated */
    tmp = calloc(buf_size + 1, sizeof(char));
    if (tmp == NULL)
    {
        printf("calloc() = %s\r\n", "NULL");
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    }

    memcpy(tmp, buf, buf_size);

    rc = EXECUTE_COMMAND(usercontextcallback, tmp);
    switch (rc)
    {
    case EXECUTE_COMMAND_FAILED:
        free(tmp);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_REJECTED));
        return IOTHUBMESSAGE_REJECTED;
    case EXECUTE_COMMAND_ERROR:
        free(tmp);
        printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED));
        return IOTHUBMESSAGE_ABANDONED;
    default:
        break;
    }

    free(tmp);
    printf("%s\r\n", ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED));
    return IOTHUBMESSAGE_ACCEPTED;
}

static int send_Message(IOTHUB_CLIENT_HANDLE iothubclienthandle, IOTHINX_IO *iothinxio)
{
    int rc = 0;
    unsigned char *buf = NULL;
    size_t buf_size = 0;

    IOTHUB_MESSAGE_HANDLE iothubmessagehandle = NULL;

    rc = SERIALIZE(&buf, &buf_size, iothinxio->DeviceId, iothinxio->DI_Values, iothinxio->DO_Values);
    if (rc != CODEFIRST_OK)
    {
        printf("SERIALIZE() = %d\r\n", rc);
        return -1;
    }

    iothubmessagehandle = IoTHubMessage_CreateFromByteArray(buf, buf_size);
    if (iothubmessagehandle == NULL)
    {
        free(buf);
        printf("IoTHubMessage_CreateFromByteArray() = %s\r\n", "NULL");
        return -1;
    }

    rc = IoTHubClient_SendEventAsync(iothubclienthandle, iothubmessagehandle, send_Callback, NULL);
    if (rc != IOTHUB_CLIENT_OK)
    {
        IoTHubMessage_Destroy(iothubmessagehandle);
        free(buf);
        printf("IoTHubClient_SendEventAsync() = %d\r\n", rc);
        return -1;
    }

    IoTHubMessage_Destroy(iothubmessagehandle);
    free(buf);

    return 0;
}

static int Azure_IoT_Init(IOTHUB_CLIENT_HANDLE *iothubclienthandle, IOTHINX_IO **iothinxio)
{
    int rc = 0;

    rc = platform_init();
    if (rc != 0)
    {
        printf("platform_init() = %d\r\n", rc);
        return -1;
    }

    rc = serializer_init(NULL);
    if (rc != SERIALIZER_OK)
    {
        printf("serializer_init() = %d\r\n", rc);
        platform_deinit();
        return -1;
    }

    *iothubclienthandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (*iothubclienthandle == NULL)
    {
        printf("IoTHubClient_CreateFromConnectionString() = %s\r\n", "NULL");
        serializer_deinit();
        platform_deinit();
        return -1;
    }

    *iothinxio = CREATE_MODEL_INSTANCE(MOXA_SAMPLE, IOTHINX_IO);
    if (*iothinxio == NULL)
    {
        printf("CREATE_MODEL_INSTANCE() = %s\r\n", "NULL");
        IoTHubClient_Destroy(*iothubclienthandle);
        serializer_deinit();
        platform_deinit();
        return -1;
    }

    rc = IoTHubClient_SetMessageCallback(*iothubclienthandle, recv_Callback, *iothinxio);
    if (rc != IOTHUB_CLIENT_OK)
    {
        printf("IoTHubClient_SetMessageCallback() = %d\r\n", rc);
        DESTROY_MODEL_INSTANCE(*iothinxio);
        IoTHubClient_Destroy(*iothubclienthandle);
        serializer_deinit();
        platform_deinit();
        return -1;
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    int rc = 0;
    uint32_t slot = 1;
    uint32_t di_values = 0;
    uint32_t do_values = 0;

    IOTHUB_CLIENT_HANDLE iothubclienthandle = NULL;
    IOTHINX_IO *iothinxio = NULL;

    rc = ioThinx_IO_Client_Init();
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_IO_Client_Init() = %d\r\n", rc);
        return -1;
    }

    rc = Azure_IoT_Init(&iothubclienthandle, &iothinxio);
    if (rc != 0)
    {
        return -1;
    }

    rc = ioThinx_DI_GetValues(slot, &di_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DI_GetValues() = %d\r\n", rc);
        return -1;
    }

    rc = ioThinx_DO_GetValues(slot, &do_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_GetValues() = %d\r\n", rc);
        return -1;
    }

    iothinxio->DeviceId = "ioThinx";
    iothinxio->DI_Values = di_values;
    iothinxio->DO_Values = do_values;

    rc = send_Message(iothubclienthandle, iothinxio);
    if (rc != 0)
    {
        printf("send_Message() = %d\r\n", rc);
        DESTROY_MODEL_INSTANCE(iothinxio);
        IoTHubClient_Destroy(iothubclienthandle);
        serializer_deinit();
        platform_deinit();
        return -1;
    }

    while (true)
    {
        rc = ioThinx_DI_GetValues(slot, &di_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DI_GetValues() = %d\r\n", rc);
            return -1;
        }

        rc = ioThinx_DO_GetValues(slot, &do_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DO_GetValues() = %d\r\n", rc);
            return -1;
        }

        if (iothinxio->DI_Values != di_values || iothinxio->DO_Values != do_values)
        {
            iothinxio->DI_Values = di_values;
            iothinxio->DO_Values = do_values;

            rc = send_Message(iothubclienthandle, iothinxio);
            if (rc != 0)
            {
                printf("send_Message() = %d\r\n", rc);
                DESTROY_MODEL_INSTANCE(iothinxio);
                IoTHubClient_Destroy(iothubclienthandle);
                serializer_deinit();
                platform_deinit();
                return -1;
            }
        }

        sleep(1);
    }

    DESTROY_MODEL_INSTANCE(iothinxio);
    IoTHubClient_Destroy(iothubclienthandle);
    serializer_deinit();
    platform_deinit();

    return 0;
}
