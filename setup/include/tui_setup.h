#ifndef TUI_SETUP_H
#define TUI_SETUP_H

#include <stdio.h>
#include <tee_client_api.h>

void prepare_tee_session(TEEC_Context *ctx, TEEC_Session *sess);
void terminate_tee_session(TEEC_Context *ctx, TEEC_Session *sess);
int tui_setup(TEEC_Session *sess, char *ip, int port);

#endif