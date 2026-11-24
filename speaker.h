#pragma once

#include "base.h"
#include <ole2.h>
#include <sapi.h>

extern const CLSID CLSID_SpVoice;
extern const IID   IID_ISpVoice;

#define SPEAK_LIMIT 1024
#define SPEAK_ACCUM_LIMIT (SPEAK_LIMIT * 8)

/////////////////////////////////////////////////////////// ПРОТОТИПЫ ФУНКЦИЙ ///////////////////////////////////////////////////////////
int SpeakInit();

void SpeakCleanup();

int Speak(const wchar_t *text);

int SpeakChar(const char *text);

void SpeakAppend(const char *text);

void SpeakFlush();



#define SPEAK_AND_PRINT(fileout, ...) do {              \
    char buf[SPEAK_LIMIT] = "";                         \
    snprintf(buf, SPEAK_LIMIT, __VA_ARGS__);            \
    fprintf(fileout, "%s", buf);                        \
    SpeakAppend(buf);                                   \
} while(0)

#define SPEAK_AND_PRINT_BASE(...) SPEAK_AND_PRINT(stdout, __VA_ARGS__)