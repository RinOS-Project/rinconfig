# RinConfig

Parses bounded INI-like text in place and reports syntax records through a caller callback.

## Public API contract

| Requirement | Contract |
| --- | --- |
| Purpose | Parses bounded INI-like text in place and reports syntax records through a caller callback. |
| Supported API | include/rinconfig/ini.h and rin_config_ini_parse. |
| Unsupported API | No file I/O, authorization, application policy, or general configuration language. |
| ownership | Caller owns a mutable NUL-terminated input buffer; callback strings are borrowed for the call. |
| thread-safety | Stateless parser; separate buffers and callback contexts may run concurrently. |
| limits | Input is capped at 64 KiB and sections at 64. |
| errors | Nonzero parse/callback result is failure; do not consume partial values as success. |
| ABI stability | Public C source interface; no separate binary ABI version. |
| security | Syntax only. Callback owner enforces key policy, authorization, and safe persistence. |
| build | No standalone build file; compile ini.c through a consumer build. |
| test | No standalone test target; parent CI owns consumer contracts. No tests/builds run for this README update. |
