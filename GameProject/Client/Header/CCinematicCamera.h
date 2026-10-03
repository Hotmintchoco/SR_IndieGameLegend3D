#pragma once

#include "CCamera.h"

struct CINEMATIC_DESC
{
	_vec3 vEyeFrom{ 0.f, 0.f, 0.f };    // 시작 위치
	_vec3 vEyeTo{ 0.f, 0.f, 0.f };      // 종료 위치
	_vec3 vLookAt{ 0.f, 0.f, 1.f };     // 시선 위치 (대상의 위치)

    _float fDuration = 2.f;             // 연출 지속 시간

    // 라디안 단위
    _float fFovFrom = D3DXToRadian(60.f);
    _float fFovTo = D3DXToRadian(60.f);
};

class CCinematicCamera : public Engine::CCamera
{
protected:
    explicit CCinematicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CCinematicCamera();

public:
    HRESULT Ready_GameObject() override;

    _int Update_GameObject(_float fTimeDelta) override;
    void LateUpdate_GameObject(_float fTimeDelta) override;
    void Render_GameObject() override {}

public:
    HRESULT Play(const CINEMATIC_DESC& tDesc);
    void Stop();
    void Skip();

    _bool Is_Playing() const { return m_bPlaying; }
    _bool Is_Finished() const { return m_bFinished; }

private:
    void Evaluate(_float fRatio);

private:
    CINEMATIC_DESC  m_tDesc{};

    _float          m_fElapsedTime;
    _bool           m_bPlaying;
    _bool           m_bFinished;

public:
    static CCinematicCamera* Create(
        LPDIRECT3DDEVICE9 pGraphicDev);

private:
    void Free() override;
};