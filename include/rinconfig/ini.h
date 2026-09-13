/* SPDX-License-Identifier: MIT */
#ifndef RINCONFIG_INI_H
#define RINCONFIG_INI_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIN_CONFIG_INI_MAX_BYTES (64u * 1024u)
#define RIN_CONFIG_INI_SECTION_MAX 64u

typedef int (*RinConfigIniCallback)(const char* section,
                                    const char* key,
                                    const char* value,
                                    void* context);

/* Parse a bounded, NUL-terminated INI-like buffer in place.  The parser owns
 * syntax only; authorization and key policy remain in the callback owner. */
int rin_config_ini_parse(char* text, size_t text_size,
                         RinConfigIniCallback callback, void* context);

#ifdef __cplusplus
}
#endif

#endif /* RINCONFIG_INI_H */
