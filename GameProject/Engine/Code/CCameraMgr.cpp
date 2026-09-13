#include "CCameraMgr.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
	: m_pCurCamera(NULL, nullptr)
{
}

CCameraMgr::~CCameraMgr()
{
	Free();
}

void CCameraMgr::Update_Camera(const _float& fTimeDelta, const _vec3& vPlayerLook, const _vec3& vPlayerPos, const _vec3& vPlayerRight)
{
	m_pCurCamera.second->Update_Camera(fTimeDelta, vPlayerLook, vPlayerPos, vPlayerRight);
}

void CCameraMgr::LateUpdate_Camera(const _float& fTimeDelta)
{
	m_pCurCamera.second->LateUpdate_Camera(fTimeDelta);
}


HRESULT CCameraMgr::Ready_Camera(const _tchar* pCameraTag, CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraObj* pCamera = Find_Camera(pCameraTag);

	if (nullptr != pCamera)
		return E_FAIL;

	pCamera = CCameraObj::Create(tagCameraType, pGraphicDev);
	if (nullptr == pCamera)
		return E_FAIL;

	m_mapCamera.insert({ pCameraTag, pCamera });

	return S_OK;
}

HRESULT CCameraMgr::Select_Camera(const _tchar* pCameraTag)
{
	CCameraObj* pCamera = Find_Camera(pCameraTag);

	if (nullptr == pCamera)
		return E_FAIL;

	m_pCurCamera = { pCameraTag, pCamera };

	return S_OK;
}

CCameraObj* CCameraMgr::Find_Camera(const _tchar* pCameraTag)
{
	auto		iter = find_if(m_mapCamera.begin(), m_mapCamera.end(), CTag_Finder(pCameraTag));

	if (iter == m_mapCamera.end())
		return nullptr;

	return iter->second;
}

HRESULT CCameraMgr::Get_CamLook(_vec3* pLook)
{
	if (m_pCurCamera.second == nullptr) return E_FAIL;

	m_pCurCamera.second->Get_CamLook(pLook);

	return S_OK;
}

HRESULT CCameraMgr::Get_CameraAngle(_float* pAngle)
{
	if (m_pCurCamera.second == nullptr) return E_FAIL;

	m_pCurCamera.second->Get_CameraAngle(pAngle);

	return S_OK;
}


void CCameraMgr::Free()
{
	for_each(m_mapCamera.begin(), m_mapCamera.end(), CDeleteMap());
	m_mapCamera.clear();
}

