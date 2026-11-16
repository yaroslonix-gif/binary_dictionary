#include "speaker.h"

const CLSID CLSID_SpVoice = 
    {0x96749377, 0x3391, 0x11d2, {0x9e, 0xe3, 0x00, 0xc0, 0x4f, 0x79, 0x73, 0x96}};

const IID IID_ISpVoice = 
    {0x6c44df74, 0x72b9, 0x4992, {0xa1, 0xec, 0xef, 0x99, 0x6e, 0x04, 0x22, 0xd4}};


int Speak(const wchar_t *text) {
    if (text == NULL)
        return FALSE;

    HRESULT hres = S_OK;
    ISpVoice *pVoice = NULL;

    hres = CoInitialize(NULL);
    if (FAILED(hres))
        return FALSE;

    hres = CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void **)&pVoice);
    if (SUCCEEDED(hres) && pVoice != NULL) {
        hres = pVoice->lpVtbl->Speak(pVoice, text, SPF_DEFAULT, NULL);

        pVoice->lpVtbl->Release(pVoice);
    }

    CoUninitialize();

    return SUCCEEDED(hres) ? TRUE : FALSE;
}

int SpeakChar(const char *text) {
    if (text == NULL)
        return FALSE;

    wchar_t buf[SPEAK_LIMIT];
    int n = swprintf(buf, SPEAK_LIMIT, L"%hs", text);
    if (n < 0) return FALSE;

    return Speak(buf);
}