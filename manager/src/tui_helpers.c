#include <tui_ta.h>
#include <string.h>
#include <string_ext.h>

#include "tui_helpers.h"
#include "st.h"
#include "tui_ta_defines.h"

TEE_Result get_st_tcpSocket_Setup(TEE_tcpSocket_Setup *setup) {
    uint32_t flags, count;
    TEE_Result res;
    TEE_ObjectHandle obj;
    static st ST;

    flags = TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_SHARE_READ;
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, ST_OBJ_ID, strlen(ST_OBJ_ID), flags, &obj);
    if (res != TEE_SUCCESS) {
        EMSG("Failed to open persistent object, res=0x%08x", res);
        return res;
    }
    res = TEE_ReadObjectData(obj, &ST, sizeof(struct _st), &count);
    if (res != TEE_SUCCESS || count != sizeof(struct _st)) {
        EMSG("TEE_ReadObjectData failed 0x%08x, read %" PRIu32 " over %lu", res, count, sizeof(struct _st));
        TEE_CloseObject(obj);
        return res;
    }
    TEE_CloseObject(obj);

    setup->ipVersion = TEE_IP_VERSION_4;
    setup->server_addr = ST.ip;
    setup->server_port = ST.port;

    return TEE_SUCCESS;
}

TEE_Result print_aux(const char *str){
    uint32_t protocolError, count;
    TEE_tcpSocket_Setup setup;
    TEE_Result res;
    TEE_iSocketHandle ctx;
    char buffer_send[BUFFER_SIZE] = { };

    if(strlen(str) > (BUFFER_SIZE - 2* sizeof(char))) {
        DMSG("Send buffer too short for str:\"%s\"", str);
        return TEE_ERROR_GENERIC;
    }

    res = get_st_tcpSocket_Setup(&setup);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to get Security Token setup, res=0x%08x", res);
        return res;
    }

    res = TEE_tcpSocket->open(&ctx, &setup, &protocolError);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to open TCP socket, ip=\"%s\", port=%d, res=0x%08x, protocolError=0x%x", setup.server_addr, setup.server_port, res, protocolError);
        return res;
    }

    strncpy(buffer_send, OP_PRINT, 2*sizeof(char));
    strlcat(buffer_send, str, BUFFER_SIZE);
    count = BUFFER_SIZE;
    IMSG("Sending to %s:%d:\"%s\"", setup.server_addr, setup.server_port, buffer_send);
    res = TEE_tcpSocket->send(ctx, buffer_send, &count, TEE_TIMEOUT_INFINITE);
    if(res != TEE_SUCCESS || count != BUFFER_SIZE) {
        EMSG("TCP socket failed to send data, res=0x%08x, transmitted=%lu (bytes)" , res, (unsigned long)count);
        if(TEE_tcpSocket->close(ctx) != TEE_SUCCESS) {
            EMSG("Failed to close TCP socket");
        }
        return res;
    }

    res = TEE_tcpSocket->close(ctx);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to close TCP socket, res=0x%08x", res);
        return res;
    }
    return res;
}

TEE_Result input_aux(const char *str, char *input, size_t input_size){
    uint32_t protocolError, count;
    TEE_tcpSocket_Setup setup;
    TEE_Result res;
    TEE_iSocketHandle ctx;
    char buffer_send[BUFFER_SIZE] = { };
    char buffer_recv[BUFFER_SIZE] = { };

    if(strlen(str) > (BUFFER_SIZE - 2* sizeof(char))) {
        DMSG("Send buffer too short for str:\"%s\"", str);
        return TEE_ERROR_GENERIC;
    }

    res = get_st_tcpSocket_Setup(&setup);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to get Security Token setup, res=0x%08x", res);
        return res;
    }

    res = TEE_tcpSocket->open(&ctx, &setup, &protocolError);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to open TCP socket, ip=\"%s\", port=%d, res=0x%08x, protocolError=0x%x", setup.server_addr, setup.server_port, res, protocolError);
        return res;
    }

    strncpy(buffer_send, OP_INPUT, 2*sizeof(char));
    strlcat(buffer_send, str, BUFFER_SIZE);
    count = BUFFER_SIZE;
    IMSG("Sending to %s:%d:\"%s\"", setup.server_addr, setup.server_port, buffer_send);
    res = TEE_tcpSocket->send(ctx, buffer_send, &count, TEE_TIMEOUT_INFINITE);
    if(res != TEE_SUCCESS || count != BUFFER_SIZE) {
        EMSG("TCP socket failed to send data, res=0x%08x, transmitted=%lu (bytes)" , res, (unsigned long)count);
        if(TEE_tcpSocket->close(ctx) != TEE_SUCCESS) {
            EMSG("Failed to close TCP socket");
        }
        return res;
    }

    res = TEE_tcpSocket->recv(ctx, buffer_recv, &count, TEE_TIMEOUT_INFINITE);
    if (res != TEE_SUCCESS){
        EMSG("TCP socket failed to receive reply data, res=0x%08x, transmitted=%lu (bytes)" , res, (unsigned long)count);
        if(TEE_tcpSocket->close(ctx) != TEE_SUCCESS) {
            EMSG("Failed to close TCP socket");
        }
        return res;
    }
    IMSG("Received from %s:%d:\"%s\"", setup.server_addr, setup.server_port, buffer_recv);

    res = TEE_tcpSocket->close(ctx);
    if(res != TEE_SUCCESS) {
        EMSG("Failed to close TCP socket, res=0x%08x", res);
        return res;
    }


    strncpy(input, buffer_recv, input_size);

    return res;
}