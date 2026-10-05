#pragma once
#include "CCamera.h"

namespace Engine
{
    class CTransform;
}

enum class CAMERA_MODE 
{ 
    FIRST_PERSON,
    THIRD_PERSON 
};

class CPlayerCamera : public CCamera
{
protected:
    explicit CPlayerCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayerCamera(const CPlayerCamera& rhs);
    virtual ~CPlayerCamera();

public:
    HRESULT		    Ready_GameObject(CTransform* pTarget);

    _int            Update_GameObject(_float fTimeDelta) override;
    void            LateUpdate_GameObject(_float fTimeDelta) override;
    void            Render_GameObject() {}

public:
    void            Set_Target(CTransform* pTarget) { m_pTarget = pTarget; }
    void            Set_EyeOffset(const _vec3& vOffset) { m_vEyeOffset = vOffset; }
	void			Set_CameraMode(CAMERA_MODE eMode) { m_eCameraMode = eMode; }
    CAMERA_MODE     Get_CameraMode() { return m_eCameraMode; }

	void            Set_Distance(_float fDistance) { m_fDistance = fDistance; }
    _float          Get_Distance() const { return m_fDistance; }

private:
    void            Mouse_Move();
    void            Follow_Target();

private:
    CTransform*     m_pTarget;

    // 실제 플레이어 Transform의 기준점에 맞춰 설정
    _vec3           m_vEyeOffset;
    _float          m_fDistance;
	CAMERA_MODE     m_eCameraMode = CAMERA_MODE::FIRST_PERSON;

public:
    static CPlayerCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pTarget);

private:
    virtual void		Free() override;
};

