#pragma once

#include "CScene.h"

class CEnding : public CScene
{
private:
    explicit CEnding(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CEnding();

public:
    virtual			HRESULT		Ready_Scene();
    virtual			_int		Update_Scene(_float fTimeDelta);
    virtual			void		LateUpdate_Scene(_float fTimeDelta);
    virtual			void		Render_Scene();

    virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override { return S_OK; }

private:
    HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);

public:
    static CEnding* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
    virtual void	Free();

private:
    _float m_fElapsedTime = 0.f;

    vector<_vec3> m_vC =
    {
        _vec3{ -2.9f, 0.f,  0.4f },
        _vec3{ -2.7f, 0.f,  0.4f },
        _vec3{ -2.5f, 0.f,  0.4f },
        _vec3{ -2.3f, 0.f,  0.4f },
        _vec3{ -2.1f, 0.f,  0.4f },
        _vec3{ -2.9f, 0.f,  0.2f },
        _vec3{ -2.9f, 0.f,  0.0f },
        _vec3{ -2.9f, 0.f, -0.2f },
        _vec3{ -2.9f, 0.f, -0.4f },
        _vec3{ -2.7f, 0.f, -0.4f },
        _vec3{ -2.5f, 0.f, -0.4f },
        _vec3{ -2.3f, 0.f, -0.4f },
        _vec3{ -2.1f, 0.f, -0.4f }
    };
    vector<_vec3> m_vCStart =
    {
        _vec3(-4.5f, 0.f,  3.5f),
        _vec3(-2.5f, 0.f,  3.8f),
        _vec3(0.f, 0.f,  3.8f),
        _vec3(2.5f, 0.f,  3.8f),
        _vec3(4.5f, 0.f,  3.5f),
        _vec3(-4.8f, 0.f,  2.f),
        _vec3(4.8f, 0.f,  2.f),
        _vec3(-4.8f, 0.f,  0.f),
        _vec3(4.8f, 0.f,  0.f),
        _vec3(-4.f, 0.f, -3.5f),
        _vec3(0.f, 0.f, -3.8f),
        _vec3(4.f, 0.f, -3.5f),
        _vec3(4.5f, 0.f, -3.5f)
    };

    vector<_vec3> m_vL =
    {
        _vec3{ -1.7f, 0.f,  0.4f },
        _vec3{ -1.7f, 0.f,  0.2f },
        _vec3{ -1.7f, 0.f,  0.0f },
        _vec3{ -1.7f, 0.f, -0.2f },
        _vec3{ -1.7f, 0.f, -0.4f },
        _vec3{ -1.5f, 0.f, -0.4f },
        _vec3{ -1.3f, 0.f, -0.4f },
        _vec3{ -1.1f, 0.f, -0.4f },
        _vec3{ -0.9f, 0.f, -0.4f }
    };
    vector<_vec3> m_vLStart =
    {
        _vec3(-4.5f, 0.f,  3.5f),
        _vec3(0.f, 0.f,  3.8f),
        _vec3(4.5f, 0.f,  3.5f),
        _vec3(-4.8f, 0.f,  0.f),
        _vec3(4.8f, 0.f,  0.f),
        _vec3(-4.5f, 0.f, -3.5f),
        _vec3(-2.5f, 0.f, -3.8f),
        _vec3(0.f, 0.f, -3.8f),
        _vec3(4.5f, 0.f, -3.5f)
    };

    vector<_vec3> m_vE =
    {
        _vec3{ -0.5f, 0.f,  0.4f },
        _vec3{ -0.3f, 0.f,  0.4f },
        _vec3{ -0.1f, 0.f,  0.4f },
        _vec3{  0.1f, 0.f,  0.4f },
        _vec3{  0.3f, 0.f,  0.4f },
        _vec3{ -0.5f, 0.f,  0.2f },
        _vec3{ -0.5f, 0.f,  0.0f },
        _vec3{ -0.3f, 0.f,  0.0f },
        _vec3{ -0.1f, 0.f,  0.0f },
        _vec3{  0.1f, 0.f,  0.0f },
        _vec3{  0.3f, 0.f,  0.0f },
        _vec3{ -0.5f, 0.f, -0.2f },
        _vec3{ -0.5f, 0.f, -0.4f },
        _vec3{ -0.3f, 0.f, -0.4f },
        _vec3{ -0.1f, 0.f, -0.4f },
        _vec3{  0.1f, 0.f, -0.4f },
        _vec3{  0.3f, 0.f, -0.4f }
    };
    vector<_vec3> m_vEStart =
    {
        _vec3(-4.5f, 0.f,  3.5f),
        _vec3(-2.5f, 0.f,  3.8f),
        _vec3(0.f, 0.f,  3.8f),
        _vec3(2.5f, 0.f,  3.8f),
        _vec3(4.5f, 0.f,  3.5f),
        _vec3(-4.8f, 0.f,  2.f),
        _vec3(-4.8f, 0.f,  0.f),
        _vec3(4.8f, 0.f,  1.f),
        _vec3(-4.5f, 0.f, -3.5f),
        _vec3(0.f, 0.f, -3.8f),
        _vec3(4.5f, 0.f, -3.5f),
        _vec3(-4.8f, 0.f, -2.f),
        _vec3(-4.f, 0.f, -3.5f),
        _vec3(-2.f, 0.f, -3.8f),
        _vec3(2.f, 0.f, -3.8f),
        _vec3(4.f, 0.f, -3.5f),
        _vec3(4.8f, 0.f,  0.f)
    };

    vector<_vec3> m_vA =
    {
        _vec3{ 0.7f, 0.f,  0.4f },
        _vec3{ 0.9f, 0.f,  0.4f },
        _vec3{ 1.1f, 0.f,  0.4f },
        _vec3{ 1.3f, 0.f,  0.4f },
        _vec3{ 1.5f, 0.f,  0.4f },
        _vec3{ 0.7f, 0.f,  0.2f },
        _vec3{ 1.5f, 0.f,  0.2f },
        _vec3{ 0.7f, 0.f,  0.0f },
        _vec3{ 0.9f, 0.f,  0.0f },
        _vec3{ 1.1f, 0.f,  0.0f },
        _vec3{ 1.3f, 0.f,  0.0f },
        _vec3{ 1.5f, 0.f,  0.0f },
        _vec3{ 0.7f, 0.f, -0.2f },
        _vec3{ 1.5f, 0.f, -0.2f },
        _vec3{ 0.7f, 0.f, -0.4f },
        _vec3{ 1.5f, 0.f, -0.4f }
    };
    vector<_vec3> m_vAStart =
    {
        _vec3(-4.5f, 0.f,  3.5f),
        _vec3(-2.5f, 0.f,  3.8f),
        _vec3(0.f, 0.f,  3.8f),
        _vec3(2.5f, 0.f,  3.8f),
        _vec3(4.5f, 0.f,  3.5f),
        _vec3(-4.8f, 0.f,  2.f),
        _vec3(4.8f, 0.f,  2.f),
        _vec3(-4.8f, 0.f,  0.f),
        _vec3(-4.f, 0.f, -3.5f),
        _vec3(-2.f, 0.f, -3.8f),
        _vec3(0.f, 0.f, -3.8f),
        _vec3(2.f, 0.f, -3.8f),
        _vec3(4.8f, 0.f,  0.f),
        _vec3(-4.8f, 0.f, -2.f),
        _vec3(4.8f, 0.f, -2.f),
        _vec3(-4.5f, 0.f, -3.5f),
        _vec3(4.5f, 0.f, -3.5f)
    };

    vector<_vec3> m_vR =
    {
        _vec3{ 1.9f, 0.f,  0.4f },
        _vec3{ 2.1f, 0.f,  0.4f },
        _vec3{ 2.3f, 0.f,  0.4f },
        _vec3{ 2.5f, 0.f,  0.4f },
        _vec3{ 2.7f, 0.f,  0.4f },
        _vec3{ 1.9f, 0.f,  0.2f },
        _vec3{ 2.7f, 0.f,  0.2f },
        _vec3{ 1.9f, 0.f,  0.0f },
        _vec3{ 2.1f, 0.f,  0.0f },
        _vec3{ 2.3f, 0.f,  0.0f },
        _vec3{ 2.5f, 0.f, -0.3f },
        _vec3{ 2.5f, 0.f,  0.0f },
        _vec3{ 1.9f, 0.f, -0.2f },
        _vec3{ 2.3f, 0.f, -0.2f },
        _vec3{ 1.9f, 0.f, -0.4f },
        _vec3{ 2.7f, 0.f, -0.4f }
    };
    vector<_vec3> m_vRStart =
    {
        _vec3(-4.5f, 0.f,  3.5f),
        _vec3(-2.5f, 0.f,  3.8f),
        _vec3(0.f, 0.f,  3.8f),
        _vec3(2.5f, 0.f,  3.8f),
        _vec3(4.5f, 0.f,  3.5f),
        _vec3(-4.8f, 0.f,  2.f),
        _vec3(4.8f, 0.f,  2.f),
        _vec3(-4.8f, 0.f,  0.f),
        _vec3(4.8f, 0.f,  1.f),
        _vec3(-4.f, 0.f, -3.5f),
        _vec3(4.f, 0.f, -3.5f),
        _vec3(-4.8f, 0.f, -2.f),
        _vec3(0.f, 0.f, -3.8f),
        _vec3(2.5f, 0.f, -3.8f),
        _vec3(-2.5f, 0.f, -3.8f),
        _vec3(4.5f, 0.f, -3.5f)
    };
};