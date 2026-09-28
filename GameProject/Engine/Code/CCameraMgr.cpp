#include "CCameraMgr.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
	: m_pCurCamera(NULL, nullptr), m_iMoveIndex(0), m_fMoveDuring(0.f)
{
}

CCameraMgr::~CCameraMgr()
{
	Free();
}

void CCameraMgr::Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight)
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_B))
	{
		SetCameraMove({ 30.f, 1.f, 20.f }, { 20.f, 20.f, 30.f }, { 1.f, 0.f, 1.f }, 2.f);
	}

	if (nullptr == m_pCurCamera.second)
		return;

	if (!IsCameraMoving())
		m_pCurCamera.second->Input_Camera();

	for (auto& Pair : m_mapCamera)
		Pair.second->Update_Camera(fTimeDelta, vTargetLook, vTargetPos, vTargetRight);

	if (IsCameraMoving())
		Update_CameraMove(fTimeDelta);

	m_pCurCamera.second->Apply_Transform();
}

void CCameraMgr::Update_CameraMove(const _float& fTimeDelta)
{
	m_fMoveDuring += fTimeDelta;

	while (IsCameraMoving() && m_fMoveDuring >= m_vecCameraMove[m_iMoveIndex].fTime)
	{
		if (m_iMoveIndex + 1 == m_vecCameraMove.size())
		{
			Apply_CameraMove(m_vecCameraMove[m_iMoveIndex], 1.f);
			ClearCameraMove();
			return;
		}

		m_fMoveDuring -= m_vecCameraMove[m_iMoveIndex].fTime;
		++m_iMoveIndex;
	}

	const CAMERA_MOVE& tMove = m_vecCameraMove[m_iMoveIndex];
	Apply_CameraMove(tMove, m_fMoveDuring / tMove.fTime);
}

void CCameraMgr::Apply_CameraMove(const CAMERA_MOVE& tMove, _float fRatio)
{
	_vec3 vEye;
	D3DXVec3Lerp(&vEye, &tMove.vStartPos, &tMove.vEndPos, fRatio);

	_vec3 vAt = tMove.bLookAt ? tMove.vTarget : vEye + tMove.vTarget;

	_vec3 vDir = vAt - vEye;
	if (D3DXVec3Length(&vDir) < FLT_EPSILON)
	{
		m_pCurCamera.second->Get_CamLook(&vDir);
		vAt = vEye + vDir;
	}

	m_pCurCamera.second->Set_View(vEye, vAt);
}

void CCameraMgr::Push_CameraMove(CAMERA_MOVE tMove)
{
	if (tMove.fTime <= 0.f)
		tMove.fTime = 0.0001f;

	if (!IsCameraMoving())
		ClearCameraMove();

	m_vecCameraMove.push_back(tMove);
}

void CCameraMgr::SetCameraMoveAt(const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vAt, _float fTime)
{
	CAMERA_MOVE tMove;
	tMove.vStartPos = vStartPos;
	tMove.vEndPos = vEndPos;
	tMove.vTarget = vAt;
	tMove.bLookAt = true;
	tMove.fTime = fTime;

	Push_CameraMove(tMove);
}

void CCameraMgr::SetCameraMove(const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vLook, _float fTime)
{
	CAMERA_MOVE tMove;
	tMove.vStartPos = vStartPos;
	tMove.vEndPos = vEndPos;
	tMove.bLookAt = false;
	tMove.fTime = fTime;

	_vec3 vDir = vLook;
	if (D3DXVec3Length(&vDir) < FLT_EPSILON)
		vDir = vEndPos - vStartPos;
	if (D3DXVec3Length(&vDir) < FLT_EPSILON && nullptr != m_pCurCamera.second)
		m_pCurCamera.second->Get_CamLook(&vDir);
	if (D3DXVec3Length(&vDir) < FLT_EPSILON)
		vDir = { 0.f, 0.f, 1.f };

	D3DXVec3Normalize(&tMove.vTarget, &vDir);

	Push_CameraMove(tMove);
}

void CCameraMgr::SetCameraMoveInRoom(_int iRoomIndex, const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vLook, _float fTime)
{
	SetCameraMove(RoomNormalizedToWorld(iRoomIndex, vStartPos), RoomNormalizedToWorld(iRoomIndex, vEndPos), vLook, fTime);
}

void CCameraMgr::SetCameraMoveInRoomAt(_int iRoomIndex, const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vAt, _float fTime)
{
	SetCameraMoveAt(RoomNormalizedToWorld(iRoomIndex, vStartPos), RoomNormalizedToWorld(iRoomIndex, vEndPos), RoomNormalizedToWorld(iRoomIndex, vAt), fTime);
}

_vec3 CCameraMgr::RoomNormalizedToWorld(_int iRoomIndex, const _vec3& vNorm) const
{
	_int iRoomRow = iRoomIndex / m_iRoomColCount;
	_int iRoomCol = iRoomIndex % m_iRoomColCount;

	_vec3 vRoomCenterPos{
		m_vCenterRoomPosition.x - (_float)(m_iRoomColCount - 1) / 2.f * m_vOuterRoomSize.x + m_vOuterRoomSize.x * (_float)iRoomCol,
		0.f,
		m_vCenterRoomPosition.z + (_float)(m_iRoomRowCount - 1) / 2.f * m_vOuterRoomSize.z - m_vOuterRoomSize.z * (_float)iRoomRow
	};

	_float fX = max(-1.f, min(1.f, vNorm.x));
	_float fY = max(0.f, min(1.f, vNorm.y));
	_float fZ = max(-1.f, min(1.f, vNorm.z));

	return _vec3{
		vRoomCenterPos.x + fX * m_vInnerRoomSize.x * 0.5f,
		fY * m_fRoomHeight,
		vRoomCenterPos.z + fZ * m_vInnerRoomSize.z * 0.5f
	};
}

void CCameraMgr::ClearCameraMove()
{
	m_vecCameraMove.clear();
	m_iMoveIndex = 0;
	m_fMoveDuring = 0.f;
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

	if (pCamera != m_pCurCamera.second)
	{
		for (auto& Pair : m_mapCamera)
			Pair.second->Reset_Angle();
	}

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

