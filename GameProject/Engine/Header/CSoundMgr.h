#pragma once

#include "CBase.h"
#include "Engine_Define.h"

// FMOD 타입은 포인터로만 쓰므로 전방 선언으로 충분하다.
// 실제 fmod.h는 CSoundMgr.cpp에서만 include -> 클라이언트는 FMOD 헤더가 필요 없음
struct FMOD_SYSTEM;
struct FMOD_SOUND;
struct FMOD_CHANNEL;
struct FMOD_CHANNELGROUP;

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

public:
	// PlaySFX가 돌려주는 핸들. 0은 항상 무효값.
	// 소리가 끝나거나 다른 소리에 채널을 뺏기면 핸들은 자동으로 무효가 된다.
	using SOUND_HANDLE = unsigned int;
	static constexpr SOUND_HANDLE INVALID_SOUND = 0;

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr();

public:
	// wstrFolderPath 아래의 모든 폴더/파일을 재귀 탐색해서 로드 (예: L"../Resource/Sound/")
	HRESULT Ready();
	HRESULT LoadSound(const wstring& szFolderPath);

	// 매 프레임 1회 호출 (FMOD 내부 상태 갱신)
	void Update();

public:
	// 빈 이펙트 채널 자동 할당. 나중에 제어할 필요가 없으면 반환값은 무시해도 된다.
	SOUND_HANDLE PlaySFX(const wstring& pSoundKey, bool bLoop = false);
	void PlayBGM(const wstring& pSoundKey);

	void StopSFX(SOUND_HANDLE hSound);   // 무효 핸들이면 아무 일도 안 함
	void StopBGM();
	void StopAll();

	bool IsPlaying(SOUND_HANDLE hSound);
	void SetSFXChannelVolume(SOUND_HANDLE hSound, float fVolume); // 개별 소리 (그룹 볼륨과 곱해짐)
	void SetBGMVolume(float fVolume);                               // BGM 그룹 전체
	void SetSFXVolume(float fVolume);                               // SFX 그룹 전체

private:
	HRESULT RecursiveLoadSound(const wstring& szFolderPath);
	int  GetNextEffectChannel();

	FMOD_SOUND* FindSound(const wstring& pSoundKey);
	SOUND_HANDLE PlayOnChannel(FMOD_SOUND* pSound, int iIdx, bool bLoop);
	int  ResolveHandle(SOUND_HANDLE hSound);   // 유효하면 채널 인덱스, 아니면 -1
	void StopChannel(int iIdx);

private:
	static constexpr int BGM_SLOT = 0;
	static constexpr int EFFECT_BEGIN = 1;
	static constexpr int MAX_SLOT = 32;

	// 키: 파일명 (예: L"Hit.wav")
	map<wstring, FMOD_SOUND*> m_mapSound;

	// 채널별 현재 재생 핸들
	FMOD_CHANNEL* m_pChannelArr[MAX_SLOT] = {};

	// 채널에 새 소리가 할당될 때마다 증가 -> 예전 핸들을 무효로 만든다
	unsigned int m_iGeneration[MAX_SLOT] = {};

	int m_iEffectCursor = EFFECT_BEGIN;

	FMOD_SYSTEM* m_pSystem = nullptr;
	FMOD_CHANNELGROUP* m_pBGMGroup = nullptr;
	FMOD_CHANNELGROUP* m_pSFXGroup = nullptr;

	float m_fBGMVolume = 1.f;
	float m_fSFXVolume = 1.f;

private:
	virtual void Free() override;
};

END