#include "pch.h"
#include "CIceRoomLayer.h"
#include "CRoomLoadingMgr.h"
#include "CSpriteTile.h"

CIceRoomLayer::CIceRoomLayer(int iRoomIndex)
	: CRoomLayer(iRoomIndex)
{
}

CIceRoomLayer::~CIceRoomLayer()
{
}

HRESULT CIceRoomLayer::Ready_Layer()
{
	return S_OK;
}

_int CIceRoomLayer::Update_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return S_OK;

	_int iExit = CRoomLayer::Update_Layer(fTimeDelta);

	return iExit;
}

void CIceRoomLayer::LateUpdate_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return;

	CRoomLayer::LateUpdate_Layer(fTimeDelta);
}

HRESULT CIceRoomLayer::SpawnRoom()
{
	if (FAILED(CRoomLayer::SpawnRoom()))
		return E_FAIL;

	TRoomData* t = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iRoomIndex);

	for (size_t i = 0; i < t->vecTile.size(); ++i)
	{
		wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Tile_" + to_wstring(i);

		if (CSpriteTile* pTile = dynamic_cast<CSpriteTile*>(Get_GameObject(wstrTileName)))
		{
			pTile->Contaminate(EContaminateType::ICE, 99999.f);
		}
	}

	return S_OK;
}

CIceRoomLayer* CIceRoomLayer::Create(int iRoomIndex)
{
	CIceRoomLayer* pLayer = new CIceRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[CIceRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CIceRoomLayer::Free()
{
	CRoomLayer::Free();
}