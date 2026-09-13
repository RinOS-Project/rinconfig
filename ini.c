/* SPDX-License-Identifier: MIT */
#include "include/rinconfig/ini.h"

#include <stddef.h>

static int config_ini_space(char character)
{
    return character == ' ' || character == '\t' || character == '\r' ||
           character == '\n';
}

static char* config_ini_trim_left(char* text)
{
    while (text != NULL && *text != '\0' && config_ini_space(*text)) ++text;
    return text;
}

static void config_ini_trim_right(char* text)
{
    char* end;
    if (text == NULL) return;
    end = text;
    while (*end != '\0') ++end;
    while (end != text && config_ini_space(end[-1])) --end;
    *end = '\0';
}

static char* config_ini_trim(char* text)
{
    text = config_ini_trim_left(text);
    config_ini_trim_right(text);
    return text;
}

static int config_ini_unquote_value(char* value)
{
    size_t length = 0u;
    size_t read = 1u;
    size_t write = 0u;

    if (value == NULL || value[0] != '"') return 1;
    while (value[length] != '\0') ++length;
    if (length < 2u || value[length - 1u] != '"') return 0;

    while (read < length - 1u) {
        if (value[read] == '"') return 0;
        if (value[read] == '\\') {
            if (read + 1u >= length - 1u ||
                (value[read + 1u] != '"' && value[read + 1u] != '\\'))
                return 0;
            value[write++] = value[read + 1u];
            read += 2u;
            continue;
        }
        value[write++] = value[read++];
    }
    value[write] = '\0';
    return 1;
}

static int config_ini_strip_comment(char* line)
{
    int in_quote = 0;
    int escaped = 0;
    char* cursor;
    if (line == NULL) return 0;
    for (cursor = line; *cursor != '\0'; ++cursor) {
        if (escaped != 0) {
            escaped = 0;
            continue;
        }
        if (*cursor == '\\' && in_quote != 0) {
            escaped = 1;
            continue;
        }
        if (*cursor == '"') {
            in_quote = !in_quote;
            continue;
        }
        if (in_quote == 0 && (*cursor == ';' || *cursor == '#')) {
            *cursor = '\0';
            break;
        }
    }
    return in_quote == 0 && escaped == 0;
}

static int config_ini_copy_section(char section[RIN_CONFIG_INI_SECTION_MAX],
                                   const char* value)
{
    size_t length = 0u;
    if (section == NULL || value == NULL) return 0;
    while (value[length] != '\0') {
        if (length + 1u >= RIN_CONFIG_INI_SECTION_MAX) return 0;
        section[length] = value[length];
        ++length;
    }
    section[length] = '\0';
    return length != 0u;
}

int rin_config_ini_parse(char* text, size_t text_size,
                         RinConfigIniCallback callback, void* context)
{
    char section[RIN_CONFIG_INI_SECTION_MAX] = {0};
    size_t cursor = 0u;

    if (text == NULL || callback == NULL || text_size == 0u ||
        text_size > RIN_CONFIG_INI_MAX_BYTES)
        return -1;

    while (cursor < text_size && text[cursor] != '\0') {
        size_t line_end = cursor;
        char* line;
        char* separator;

        while (line_end < text_size && text[line_end] != '\0' &&
               text[line_end] != '\n')
            ++line_end;
        if (line_end == text_size) return -2;
        if (text[line_end] == '\n') {
            text[line_end] = '\0';
            ++line_end;
        }
        line = config_ini_trim(text + cursor);
        cursor = line_end;
        if (*line == '\0') continue;
        if (!config_ini_strip_comment(line)) return -2;
        line = config_ini_trim(line);
        if (*line == '\0') continue;

        if (*line == '[') {
            char* closing = line + 1u;
            char* trailing;
            while (*closing != '\0' && *closing != ']') ++closing;
            if (*closing != ']') return -2;
            *closing = '\0';
            trailing = config_ini_trim(closing + 1u);
            if (*trailing != '\0') return -2;
            if (!config_ini_copy_section(section,
                                         config_ini_trim(line + 1u)))
                return -2;
            continue;
        }

        separator = line;
        while (*separator != '\0' && *separator != '=') ++separator;
        if (*separator != '=') return -2;
        *separator = '\0';
        line = config_ini_trim(line);
        if (*line == '\0') return -2;
        separator = config_ini_trim(separator + 1u);
        if (!config_ini_unquote_value(separator)) return -2;
        if (callback(section, line, separator, context) != 0)
            return -3;
    }
    if (cursor == text_size && text[text_size - 1u] != '\0') return -2;
    return 0;
}
