#include <tui.h>
#include <tui_ta.h>
#include <string.h>

static TEE_Result prepare_session(TEE_TASessionHandle* sess) {
    TEE_UUID uuid = TA_TUI_UUID;
    TEE_Result res;
    uint32_t origin;
    uint32_t param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    res = TEE_OpenTASession(&uuid, TEE_TIMEOUT_INFINITE, param_types, NULL, sess, &origin);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_OpenTASession failed with code 0x%x origin 0x%x", res, origin);
    }

    return res;
}

static TEE_Result setup(const char *ip, int port) {
    TEE_Result res;
    uint32_t param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    TEE_Param params[4];
    TEE_TASessionHandle sess;
    uint32_t origin;

    IMSG("has been called");

    params[0].memref.buffer = (char *)ip;
    params[0].memref.size = strlen(ip) + 1;
    params[1].value.a = port;

    res = prepare_session(&sess);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_InvokeTACommand(sess, TEE_TIMEOUT_INFINITE, TA_TUI_CMD_SETUP, param_types, params, &origin);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_InvokeTACommand failed with code 0x%x origin 0x%x", res, origin);
    }

    TEE_CloseTASession(sess);
    return res;
}

static TEE_Result print(const char *str) {
    TEE_Result res;
    uint32_t param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    TEE_Param params[4];
    TEE_TASessionHandle sess;
    uint32_t origin;

    IMSG("has been called");

    params[0].memref.buffer = (char *)str;
    params[0].memref.size = strlen(str) + 1;

    res = prepare_session(&sess);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_InvokeTACommand(sess, TEE_TIMEOUT_INFINITE, TA_TUI_CMD_PRINT, param_types, params, &origin);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_InvokeTACommand failed with code 0x%x origin 0x%x", res, origin);
    }

    TEE_CloseTASession(sess);
    return res;
}

static TEE_Result input(const char *str, char *input, size_t input_size) {
    TEE_Result res;
    uint32_t param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    TEE_Param params[4];
    TEE_TASessionHandle sess;
    uint32_t origin;

    params[0].memref.buffer = (char *)str;
    params[0].memref.size = strlen(str) + 1;

    params[1].memref.buffer = input;
    params[1].memref.size = input_size;

    res = prepare_session(&sess);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_InvokeTACommand(sess, TEE_TIMEOUT_INFINITE, TA_TUI_CMD_INPUT, param_types, params, &origin);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_InvokeTACommand failed with code 0x%x origin 0x%x", res, origin);
    }

    TEE_CloseTASession(sess);
    return res;
}

static tui tui_instance = {
        .setup = &setup,
        .print = &print,
        .input = &input,
};

tui *const TUI = &tui_instance;