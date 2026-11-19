#include "speaker.h"

const CLSID CLSID_SpVoice = 
    {0x96749377, 0x3391, 0x11d2, {0x9e, 0xe3, 0x00, 0xc0, 0x4f, 0x79, 0x73, 0x96}};

const IID IID_ISpVoice = 
    {0x6c44df74, 0x72b9, 0x4992, {0xa1, 0xec, 0xef, 0x99, 0x6e, 0x04, 0x22, 0xd4}};

static ISpVoice *speaker_voice = NULL;
static int speaker_com_initted = 0;
static int speaker_voice_failed = 0;

static char speaker_accum[SPEAK_ACCUM_LIMIT] = "";
static size_t speaker_accum_len = 0;

static int SpeakW(const wchar_t *text);


int SpeakInit() {
    if (speaker_voice != NULL)
        return TRUE;
    
    if (speaker_voice_failed)
        return FALSE;

    HRESULT hres = CoInitialize(NULL);
    if (FAILED(hres) && hres != S_FALSE) {
        speaker_voice_failed = 1;
        return FALSE;
    }
    speaker_com_initted = 1;

    hres = CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void **)&speaker_voice);
    if (FAILED(hres) || speaker_voice == NULL) {
        speaker_voice = NULL;
        speaker_voice_failed = 1;
        return FALSE;
    }

    return TRUE;
}

void SpeakCleanup() {
    SpeakFlush();

    if (speaker_voice) {
        speaker_voice->lpVtbl->Release(speaker_voice);
        speaker_voice = NULL;
    }

    if (speaker_com_initted) {
        CoUninitialize();
        speaker_com_initted = 0;
    }
}

static int SpeakW(const wchar_t *text) {
    if (text == NULL)
        return FALSE;

    if (speaker_voice == NULL && !SpeakInit())
        return FALSE;

    HRESULT hres = speaker_voice->lpVtbl->Speak(speaker_voice, text, SPF_DEFAULT, NULL);

    return SUCCEEDED(hres) ? TRUE : FALSE;
}

int Speak(const wchar_t *text) {
    if (text == NULL)
        return FALSE;
    
    SpeakFlush();

    return SpeakW(text);
}

int SpeakChar(const char *text) {
    if (text == NULL)
        return FALSE;
    
    SpeakFlush();

    wchar_t wbuf[SPEAK_ACCUM_LIMIT];
    if (swprintf(wbuf, SPEAK_ACCUM_LIMIT, L"%hs", text) < 0)
        return FALSE;
    
    return SpeakW(wbuf);
}

void SpeakAppend(const char *text) {
    if (text == NULL || *text == '\0') return;

    size_t len = strlen(text);
    size_t free_space = SPEAK_ACCUM_LIMIT - speaker_accum_len - 1;

    if (len > free_space) {
        SpeakFlush();
        free_space = SPEAK_ACCUM_LIMIT - 1;

        if (len > free_space) {
            SpeakChar(text);
            return;
        }
    }

    memcpy(speaker_accum + speaker_accum_len, text, len);
    speaker_accum_len += len;
    speaker_accum[speaker_accum_len] = '\0';

    if (text[len - 1] == '\n')
        SpeakFlush();
}

void SpeakFlush() {
    if (speaker_accum_len == 0) return;

    wchar_t wbuf[SPEAK_ACCUM_LIMIT];
    int n = swprintf(wbuf, SPEAK_ACCUM_LIMIT, L"%hs", speaker_accum);

    speaker_accum_len = 0;
    speaker_accum[0] = '\0';

    if (n < 0) return;
    SpeakW(wbuf);
}