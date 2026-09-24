#include "CSoundMgr.h"

#include <io.h>
#include "fmod.h"

#ifdef _DEBUG
#pragma comment(lib, "fmodL_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

using namespace Engine;

IMPLEMENT_SINGLETON(CSoundMgr)

namespace
{
	// 핸들 구조: [상위 24비트 = 세대 번호][하위 8비트 = 채널 인덱스]
	constexpr unsigned int HANDLE_INDEX_BITS = 8;
	constexpr unsigned int HANDLE_INDEX_MASK = (1u << HANDLE_INDEX_BITS) - 1;
	constexpr unsigned int HANDLE_GEN_MAX = (1u << (32 - HANDLE_INDEX_BITS)) - 1;

	// FMOD Core의 파일 경로는 UTF-8 char* 로 받는다 -> 한글 경로 대응
	string ToUTF8(const wstring& wstr)
	{
		if (wstr.empty())
			return string();

		int iLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		string str(iLen, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], iLen, nullptr, nullptr);
		str.pop_back(); // 끝의 null 문자 제거

		return str;
	}
}

CSoundMgr::CSoundMgr()
{
	// 슬롯 상수가 private이라 클래스 밖(익명 네임스페이스)에서는 검사할 수 없어서 여기에 둔다
	static_assert(MAX_SLOT <= (1 << HANDLE_INDEX_BITS), "채널 수가 핸들 인덱스 비트를 넘음");
	static_assert(EFFECT_BEGIN < MAX_SLOT, "이펙트 채널이 최소 1개는 있어야 함");
}

CSoundMgr::~CSoundMgr()
{
	Free();
}

HRESULT CSoundMgr::Ready()
{
	if (m_pSystem)
		return E_FAIL;

	// 사운드를 담당하는 대표객체 생성
	if (FMOD_OK != FMOD_System_Create(&m_pSystem, FMOD_VERSION))
		return E_FAIL;

	// 동시에 실제로 믹싱될 최대 보이스 수 (초과분은 우선순위에 따라 가상 보이스로 처리)
	if (FMOD_OK != FMOD_System_Init(m_pSystem, 64, FMOD_INIT_NORMAL, nullptr))
		return E_FAIL;

	// 그룹은 생성 시 자동으로 Master 그룹 아래에 붙는다
	FMOD_System_CreateChannelGroup(m_pSystem, "BGM", &m_pBGMGroup);
	FMOD_System_CreateChannelGroup(m_pSystem, "SFX", &m_pSFXGroup);

	FMOD_ChannelGroup_SetVolume(m_pBGMGroup, m_fBGMVolume);
	FMOD_ChannelGroup_SetVolume(m_pSFXGroup, m_fSFXVolume);

	return S_OK;
}

void CSoundMgr::Update()
{
	if (m_pSystem)
		FMOD_System_Update(m_pSystem);
}

CSoundMgr::SOUND_HANDLE CSoundMgr::PlaySFX(const wstring& pSoundKey, bool bLoop)
{
	FMOD_SOUND* pSound = FindSound(pSoundKey);
	if (!pSound)
		return INVALID_SOUND;

	return PlayOnChannel(pSound, GetNextEffectChannel(), bLoop);
}

void CSoundMgr::PlayBGM(const wstring& pSoundKey)
{
	FMOD_SOUND* pSound = FindSound(pSoundKey);
	if (!pSound)
		return;

	PlayOnChannel(pSound, BGM_SLOT, true);
}

void CSoundMgr::StopSFX(SOUND_HANDLE hSound)
{
	int iIdx = ResolveHandle(hSound);
	if (iIdx < 0)
		return;

	StopChannel(iIdx);
}

void CSoundMgr::StopBGM()
{
	StopChannel(BGM_SLOT);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAX_SLOT; ++i)
		StopChannel(i);
}

bool CSoundMgr::IsPlaying(SOUND_HANDLE hSound)
{
	int iIdx = ResolveHandle(hSound);
	if (iIdx < 0)
		return false;

	FMOD_BOOL bPlaying = FALSE;
	if (FMOD_OK != FMOD_Channel_IsPlaying(m_pChannelArr[iIdx], &bPlaying))
		return false;

	return bPlaying != FALSE;
}

void CSoundMgr::SetSFXChannelVolume(SOUND_HANDLE hSound, float fVolume)
{
	int iIdx = ResolveHandle(hSound);
	if (iIdx < 0)
		return;

	FMOD_Channel_SetVolume(m_pChannelArr[iIdx], fVolume);
}

void CSoundMgr::SetSFXVolume(float fVolume)
{
	m_fSFXVolume = fVolume;
	if (m_pSFXGroup)
		FMOD_ChannelGroup_SetVolume(m_pSFXGroup, fVolume);
}

void CSoundMgr::SetBGMVolume(float fVolume)
{
	m_fBGMVolume = fVolume;
	if (m_pBGMGroup)
		FMOD_ChannelGroup_SetVolume(m_pBGMGroup, fVolume);
}

HRESULT CSoundMgr::LoadSound(const wstring& szFolderPath)
{
	if (szFolderPath.empty())
		return E_FAIL;

	// 경로 끝에 '/'가 없으면 붙여준다
	wstring wstrPath = szFolderPath;
	if (wstrPath.back() != L'/' && wstrPath.back() != L'\\')
		wstrPath += L'/';

	return RecursiveLoadSound(wstrPath);
}

// szFolderPath 아래의 파일을 로드하고, 하위 폴더가 있으면 재귀적으로 들어간다.
// szFolderPath는 반드시 '/'로 끝나야 한다. (예: L"../Sound/")
HRESULT CSoundMgr::RecursiveLoadSound(const wstring& szFolderPath)
{
	_wfinddata_t fd{};

	// _wfindfirst : 폴더 내 첫 번째 항목을 찾는 함수
	intptr_t hFind = _wfindfirst((szFolderPath + L"*.*").c_str(), &fd);
	if (hFind == -1)
		return E_FAIL; // 폴더가 없거나 경로가 잘못됨

	do
	{
		// "." ".." 가상 항목은 스킵 (안 하면 무한 재귀)
		if (0 == wcscmp(fd.name, L".") || 0 == wcscmp(fd.name, L".."))
			continue;

		wstring wstrFullPath = szFolderPath + fd.name;

		// 하위 폴더 -> 재귀 (빈 폴더 등은 실패해도 계속 진행)
		if (fd.attrib & _A_SUBDIR)
		{
			RecursiveLoadSound(wstrFullPath + L'/');
			continue;
		}

		// 키는 파일명만 사용 -> 다른 폴더에 같은 파일명이 있으면 충돌
		if (m_mapSound.count(fd.name))
		{
			OutputDebugStringW((L"[SoundMgr] 중복 사운드 키 무시: " + wstrFullPath + L"\n").c_str());
			continue;
		}

		// 오디오가 아닌 파일(txt, ini 등)은 CreateSound가 실패하므로 자연스럽게 걸러진다
		FMOD_SOUND* pSound = nullptr;
		if (FMOD_OK != FMOD_System_CreateSound(m_pSystem, ToUTF8(wstrFullPath).c_str(),
			FMOD_DEFAULT, nullptr, &pSound))
			continue;

		m_mapSound.emplace(fd.name, pSound);

		// _wfindnext : 다음 항목을 찾는 함수, 더 이상 없으면 0이 아닌 값 리턴
	} while (0 == _wfindnext(hFind, &fd));

	_findclose(hFind);

	return S_OK;
}

int CSoundMgr::GetNextEffectChannel()
{
	const int iBegin = EFFECT_BEGIN;
	const int iCount = MAX_SLOT - iBegin;

	// 커서 위치부터 한 바퀴 돌면서 쉬고 있는 채널을 찾는다
	for (int i = 0; i < iCount; ++i)
	{
		int idx = iBegin + (m_iEffectCursor - iBegin + i) % iCount;

		FMOD_BOOL bPlaying = FALSE;
		if (!m_pChannelArr[idx]
			|| FMOD_OK != FMOD_Channel_IsPlaying(m_pChannelArr[idx], &bPlaying)
			|| !bPlaying)
		{
			m_iEffectCursor = iBegin + (idx - iBegin + 1) % iCount;
			return idx;
		}
	}

	// 전부 사용 중이면 가장 오래전에 할당한 채널(커서 위치)을 덮어쓴다
	// -> 그 채널의 기존 핸들은 세대 번호가 바뀌면서 자동으로 무효가 됨
	int idx = m_iEffectCursor;
	m_iEffectCursor = iBegin + (idx - iBegin + 1) % iCount;
	return idx;
}

FMOD_SOUND* CSoundMgr::FindSound(const wstring& pSoundKey)
{
	auto iter = m_mapSound.find(pSoundKey);
	if (iter == m_mapSound.end())
	{
		assert(!"사운드 키를 찾을 수 없음");
		return nullptr;
	}

	return iter->second;
}

CSoundMgr::SOUND_HANDLE CSoundMgr::PlayOnChannel(FMOD_SOUND* pSound, int iIdx, bool bLoop)
{
	// 해당 슬롯에서 재생 중이던 소리는 정리
	StopChannel(iIdx);

	// 새 소리 할당 -> 세대 증가 (0은 무효 핸들용으로 비워둔다)
	unsigned int& iGen = m_iGeneration[iIdx];
	iGen = (iGen >= HANDLE_GEN_MAX) ? 1 : iGen + 1;

	const bool bBGM = (iIdx == BGM_SLOT);
	FMOD_CHANNELGROUP* pGroup = bBGM ? m_pBGMGroup : m_pSFXGroup;

	// 일시정지 상태로 시작 -> 설정을 다 끝낸 뒤 재생 (첫 프레임 튀는 소리 방지)
	FMOD_CHANNEL* pChannel = nullptr;
	if (FMOD_OK != FMOD_System_PlaySound(m_pSystem, pSound, pGroup, TRUE, &pChannel))
		return INVALID_SOUND;

	if (bLoop)
	{
		FMOD_Channel_SetMode(pChannel, FMOD_LOOP_NORMAL);
		FMOD_Channel_SetLoopCount(pChannel, -1);
	}

	// BGM은 보이스가 모자라도 밀려나지 않게 최고 우선순위(0)
	if (bBGM)
		FMOD_Channel_SetPriority(pChannel, 0);

	FMOD_Channel_SetPaused(pChannel, FALSE);

	m_pChannelArr[iIdx] = pChannel;

	return (iGen << HANDLE_INDEX_BITS) | (unsigned int)iIdx;
}

int CSoundMgr::ResolveHandle(SOUND_HANDLE hSound)
{
	if (hSound == INVALID_SOUND)
		return -1;

	int iIdx = (int)(hSound & HANDLE_INDEX_MASK);
	unsigned int iGen = hSound >> HANDLE_INDEX_BITS;

	// 이펙트 채널 범위 밖이거나(BGM 등), 이미 다른 소리가 할당된 채널이면 무효
	if (iIdx < EFFECT_BEGIN || iIdx >= MAX_SLOT)
		return -1;

	if (m_iGeneration[iIdx] != iGen || !m_pChannelArr[iIdx])
		return -1;

	return iIdx;
}

void CSoundMgr::StopChannel(int iIdx)
{
	if (m_pChannelArr[iIdx])
	{
		FMOD_Channel_Stop(m_pChannelArr[iIdx]);
		m_pChannelArr[iIdx] = nullptr;
	}
}

void CSoundMgr::Free()
{
	StopAll();

	for (auto& Pair : m_mapSound)
		FMOD_Sound_Release(Pair.second);
	m_mapSound.clear();

	if (m_pBGMGroup) { FMOD_ChannelGroup_Release(m_pBGMGroup); m_pBGMGroup = nullptr; }
	if (m_pSFXGroup) { FMOD_ChannelGroup_Release(m_pSFXGroup); m_pSFXGroup = nullptr; }

	// Close -> Release 순서
	if (m_pSystem)
	{
		FMOD_System_Close(m_pSystem);
		FMOD_System_Release(m_pSystem);
		m_pSystem = nullptr;
	}
}