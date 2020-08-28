#ifndef TSX_TUI_HELPERS_H
#define TSX_TUI_HELPERS_H

#include <tee_internal_api.h>
#include <tee_tcpsocket.h>

TEE_Result get_st_tcpSocket_Setup(TEE_tcpSocket_Setup *setup);
TEE_Result print_aux(const char *str);
TEE_Result input_aux(const char *str, char *input, size_t input_size);

#endif