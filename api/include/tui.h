#ifndef TSX_TUI_H
#define TSX_TUI_H

#include <tee_internal_api.h>

typedef const struct _tui {
    TEE_Result (*setup)(const char *ip, int port);
    TEE_Result (*print)(const char *str);
    TEE_Result (*input)(const char *str, char *input, size_t input_size);
} tui;

extern tui *const TUI;

#endif //TSX_TUI_H
