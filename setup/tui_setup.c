#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tee_client_api.h>
#include <tui_ta.h>
#include <tui_setup.h>

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;

    if (argc >= 3) {
        prepare_tee_session(&ctx, &sess);
        if(tui_setup(&sess, argv[1], atoi(argv[2])) != 0) {
            printf("tui_setup failed");
            terminate_tee_session(&ctx, &sess);
            return 1;
        }
        terminate_tee_session(&ctx, &sess);
    } else {
        errx(1, "Setup requires two arguments.\nPlease run: tui_setup <ip_address> <port>\n");
    }
    return 0;
}

void prepare_tee_session(TEEC_Context *ctx, TEEC_Session *sess) {
    TEEC_UUID uuid = TA_TUI_UUID;
    uint32_t origin;
    TEEC_Result res;

    res = TEEC_InitializeContext(NULL, ctx);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
    }

    res = TEEC_OpenSession(ctx, sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, origin);
    }
}

void terminate_tee_session(TEEC_Context *ctx, TEEC_Session *sess) {
    TEEC_CloseSession(sess);
    TEEC_FinalizeContext(ctx);
}

int tui_setup(TEEC_Session *sess, char *ip, int port) {
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);

    op.params[0].tmpref.buffer = ip;
    op.params[0].tmpref.size = strlen(ip) + 1;

    op.params[1].value.a = port;

    res = TEEC_InvokeCommand(sess, TA_TUI_CMD_SETUP, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
        return 1;
    }
    return 0;
}