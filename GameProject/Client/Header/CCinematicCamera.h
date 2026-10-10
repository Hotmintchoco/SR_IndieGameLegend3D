#pragma once

#include "CCamera.h"
#include <queue>

namespace Engine
{
    class CTransform;
}

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
    HRESULT     Play();
    void        Stop();
    void        Skip();

	void        Add_Shot(const CINEMATIC_DESC& tDesc);
    void        Replace_Shot(const CINEMATIC_DESC& tDesc);
	void        Set_StartFromCurrent(_bool bStartFromCurrent) { m_bStartFromCurrent = bStartFromCurrent; }
    void        Set_Target(CTransform* pTarget) { m_pTarget = pTarget; }

    _bool       Is_Playing() const { return m_bPlaying; }
    _bool       Is_Finished() const { return m_bFinished; }

private:
    void        Evaluate(_float fRatio);
	void	    Start_NextShot();

private:
    CTransform*             m_pTarget;          // 대상이 존재하면 vLookAt과 맞춰줌

    CINEMATIC_DESC          m_tCurrentDesc{};   // 현재 구간
    queue<CINEMATIC_DESC>   m_queueDesc;        // 다음 구간들

    _float                  m_fElapsedTime;
    _bool                   m_bPlaying;
    _bool                   m_bFinished;
	_bool                   m_bStartFromCurrent;    // 현재 위치에서 시작할지 여부

public:
    static CCinematicCamera* Create(
        LPDIRECT3DDEVICE9 pGraphicDev);

private:
    void Free() override;
};