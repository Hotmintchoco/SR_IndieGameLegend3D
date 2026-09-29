#include "CCameraMgr.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
	: m_pCurCamera(NULL, nullptr), m_pCameraWorking(nullptr)
{
}

CCameraMgr::~CCameraMgr()
{
	Free();
}

void CCameraMgr::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{

	if (nullptr == m_pCurCamera.second)
		return;

	if (nullptr == m_pCameraWorking)
		return;

	if (!static_cast<CCameraWorking*>(m_pCameraWorking)->GetIsMoving()) m_pCurCamera.second->Input_Camera();

	for (auto& Pair : m_mapCamera)
		Pair.second->Update_Camera(fTimeDelta, vTargetLook, vTargetPos, vTargetRight);

	m_pCameraWorking->Update_Camera(fTimeDelta, vTargetLook, vTargetPos, vTargetRight);
}

void CCameraMgr::LateUpdate_Camera(const _float& fTimeDelta)
{
	if (nullptr == m_pCurCamera.second)
		return;

	if (nullptr == m_pCameraWorking)
		return;

	for (auto& Pair : m_mapCamera)
		Pair.second->LateUpdate_Camera(fTimeDelta);

	m_pCameraWorking->LateUpdate_Camera(fTimeDelta);
	
	if (static_cast<CCameraWorking*>(m_pCameraWorking)->GetIsMoving()) m_pCameraWorking->Apply_Transform();
	
	else m_pCurCamera.second->Apply_Transform();
}


HRESULT CCameraMgr::Ready_Camera(const _tchar* pCameraTag, CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (m_pCameraWorking == nullptr)
	{
		m_pCameraWorking = CCameraObj::Create(CAMERA_WORKING, pGraphicDev);
	}

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

	if (pCamera != m_pCurCamera.second)
	{
		for (auto& Pair : m_mapCamera)
			Pair.second->Reset_Setting();
	}

	m_pCurCamera = { pCameraTag, pCamera };

	return S_OK;
}

HRESULT CCameraMgr::SetCameraMove(const CAMERA_MOVE& camMoveInfo)
{
	if (m_pCameraWorking == nullptr) return E_FAIL;

	dynamic_cast<CCameraWorking*>(m_pCameraWorking)->SetCameraMove(camMoveInfo);

	return S_OK;
}

HRESULT CCameraMgr::ClearCameraMove()
{
	if (m_pCameraWorking == nullptr) return E_FAIL;

	dynamic_cast<CCameraWorking*>(m_pCameraWorking)->ClearCameraMove();

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
	Safe_Release(m_pCameraWorking);
	for_each(m_mapCamera.begin(), m_mapCamera.end(), CDeleteMap());
	m_mapCamera.clear();
}

