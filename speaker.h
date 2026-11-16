#pragma once

#include "base.h"
#include <ole2.h>
#include <sapi.h>

extern const CLSID CLSID_SpVoice;
extern const IID IID_ISpVoice;
#define SPEAK_LIMIT 1024

#define SPEAK_AND_PRINT(fileout, ...) { \
    char buf[SPEAK_LIMIT] = ""; \
    snprintf(buf, SPEAK_LIMIT, __VA_ARGS__); \
    fprintf(fileout, "%s", buf); \
    \
    if (!SpeakChar(buf)) { \
        LOGG("Warning: can't speak\n"); \
    } \
}

#define SPEAK_AND_PRINT_BASE(...) SPEAK_AND_PRINT(stdout, __VA_ARGS__)


int Speak(const wchar_t *text);

int SpeakChar(const char *text);