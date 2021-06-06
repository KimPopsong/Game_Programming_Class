//#define XAUDIO2_USE

#include <vector>
#include <windows.h>
#ifdef XAUDIO2_USE
#include <xaudio2.h>
#endif
#include <strsafe.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <conio.h>
#include "SDKwavefile.h"

//--------------------------------------------------------------------------------------
// Helper macros
//--------------------------------------------------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef XAUDIO2_USE

typedef struct SoundInfo {
	BYTE* pbWaveData;
	IXAudio2SourceVoice* pSourceVoice;
	WCHAR szFilename[100];
} SoundInfo;

#endif


class CSoundManager
{
protected:
private:

#ifdef XAUDIO2_USE
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	std::vector <SoundInfo> m_soundVector;

#endif

public:
	CSoundManager();
	~CSoundManager();

	int init();
	void reset(int id);
	void volume(float vol);
	HRESULT play(int id);
	HRESULT replay(int id);
	HRESULT stop(int id);
	HRESULT add(WCHAR *szFilename, int id);
	HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);
};