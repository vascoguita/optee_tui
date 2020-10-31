#include <tee_internal_api.h>
#include <tee_tcpsocket.h>
#include <string.h>

#include <tui_ta.h>
#include "st.h"
#include "tui_helpers.h"
#include "tui_ta_defines.h"

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("has been called");

	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("has been called");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    uint32_t exp_param_types;
    
    (void)&params;
	(void)&sess_ctx;
    
    DMSG("has been called");

    exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
                                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	if(param_types != exp_param_types) {
		return TEE_ERROR_BAD_PARAMETERS;
    }

	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)&sess_ctx;

    DMSG("has been called");
}

static TEE_Result setup(void *sess_ctx, uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types, flags;
    st ST;
    TEE_ObjectHandle obj;
    TEE_Result res;
    char *ip;
    size_t ip_size;
    char input[2];

    (void)&sess_ctx;

    DMSG("has been called");

    exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_VALUE_INPUT,
                                      TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    if(param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    ip = (char *)params[0].memref.buffer;
    ip_size = params[0].memref.size;
    if(ip[ip_size - 1] != '\0' || ip_size < MIN_IPV4_SIZE || ip_size > MAX_IPV4_SIZE) {
        DMSG("Invalid Security Token IPv4");
        return TEE_ERROR_GENERIC;
    }
    TEE_MemMove(ST.ip, ip, ip_size);
    IMSG("Security Token IPv4 Received: \"%s\"", ST.ip);

    if((int)params[1].value.a < PORT_MIN || (int)params[1].value.a > PORT_MAX) {
        DMSG("Invalid Security Token Port");
        return TEE_ERROR_GENERIC;
    }
    ST.port = params[1].value.a;
    IMSG("Security Token Port Received: %d", ST.port);


    flags = TEE_DATA_FLAG_ACCESS_READ;
    //flags = TEE_DATA_FLAG_ACCESS_READ |		/* we can later read the oject */
    //        TEE_DATA_FLAG_ACCESS_WRITE |		/* we can later write into the object */
    //        TEE_DATA_FLAG_ACCESS_WRITE_META |	/* we can later destroy or rename the object */
    //        TEE_DATA_FLAG_OVERWRITE;		    /* destroy existing object of same ID */

    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, ST_OBJ_ID, strlen(ST_OBJ_ID), flags, TEE_HANDLE_NULL, &ST, sizeof(struct _st), &obj);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_CreatePersistentObject failed 0x%08x", res);
        return res;
    }
    TEE_CloseObject(obj);

    IMSG("Security Token UI setup stored in persistent object \"%s\"", ST_OBJ_ID);

    res = input_aux(ST_SETUP_HS, input, 2);
    if(res != TEE_SUCCESS || strcmp(input, YES) != 0) {
        EMSG("Security Token setup failed to perform handshake 0x%08x", res);
        IMSG("Deleting Security Token UI setup persistent object \"%s\"", ST_OBJ_ID);
        if(TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, ST_OBJ_ID, strlen(ST_OBJ_ID), flags, &obj) != TEE_SUCCESS) {
            EMSG("Failed to open persistent object, res=0x%08x", res);
            EMSG("Failed to delete Security Token UI setup persistent object \"%s\"", ST_OBJ_ID);
        } else {
            TEE_CloseAndDeletePersistentObject1(obj);
        }
    }
    return res;
}

static TEE_Result print(void *sess_ctx, uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types;
    TEE_Result res;
    char *str;
    size_t str_size;

    (void)&sess_ctx;

    DMSG("has been called");

    exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE,
                                      TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    if(param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    str = (char *)params[0].memref.buffer;
    str_size = params[0].memref.size;
    if(str[str_size - 1] != '\0') {
        DMSG("Invalid string:\"%s\"", str);
        return TEE_ERROR_GENERIC;
    }

    res = print_aux(str);
    if(res != TEE_SUCCESS) {
        EMSG("TUI failed to print: \"%s\", res=0x%08x" , str, res);
    }

    return res;
}

static TEE_Result input(void *sess_ctx, uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types;
    TEE_Result res;
    char *str;
    size_t str_size;

	(void)&sess_ctx;

	DMSG("has been called");

    exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	if(param_types != exp_param_types) {
		return TEE_ERROR_BAD_PARAMETERS;
    }

    str = (char *)params[0].memref.buffer;
    str_size = params[0].memref.size;

    if(str[str_size - 1] != '\0') {
        DMSG("Invalid string:\"%s\"", str);
        return TEE_ERROR_GENERIC;
    }


    res = input_aux(str, params[1].memref.buffer, params[1].memref.size);
    if(res != TEE_SUCCESS) {
        EMSG("TUI failed to input: \"%s\", res=0x%08x" , str, res);
    }

    return res;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd, uint32_t param_types, TEE_Param params[4]) {
    TEE_Result res;
    TEE_Identity identity;

    res = TEE_GetPropertyAsIdentity(TEE_PROPSET_CURRENT_CLIENT,  "gpd.client.identity", &identity);
    if(res != TEE_SUCCESS) {
        EMSG("TUI failed to retrieve client identity, res=0x%08x" , res);
        return res;
    }
    if(identity.login != TEE_LOGIN_TRUSTED_APP && cmd != TA_TUI_CMD_SETUP) {
        EMSG("Access Denied: Only TAs are allowed to use TUI");
        return TEE_ERROR_GENERIC;
    }

    switch(cmd) {
        case TA_TUI_CMD_SETUP:
            return setup(sess_ctx, param_types, params);
        case TA_TUI_CMD_PRINT:
            return print(sess_ctx, param_types, params);
        case TA_TUI_CMD_INPUT:
            return input(sess_ctx, param_types, params);
        default:
            return TEE_ERROR_NOT_SUPPORTED;
    }
}