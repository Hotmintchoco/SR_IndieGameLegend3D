#include "pch.h"
#include "CLaserPuzzleRoomLayer.h"

CLaserPuzzleRoomLayer::CLaserPuzzleRoomLayer(int iRoomIndex)
	: CRoomLayer(iRoomIndex)
{
}

CLaserPuzzleRoomLayer::~CLaserPuzzleRoomLayer()
{
}

HRESULT CLaserPuzzleRoomLayer::Ready_Layer()
{
	return S_OK;
}

_int CLaserPuzzleRoomLayer::Update_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return S_OK;

	_int iExit = CRoomLayer::Update_Layer(fTimeDelta);

	return iExit;
}

void CLaserPuzzleRoomLayer::LateUpdate_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return;

	CRoomLayer::LateUpdate_Layer(fTimeDelta);
}

CLaserPuzzleRoomLayer* CLaserPuzzleRoomLayer::Create(int iRoomIndex)
{
	CLaserPuzzleRoomLayer* pLayer = new CLaserPuzzleRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[CLaserPuzzleRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLaserPuzzleRoomLayer::Free()
{
	CRoomLayer::Free();
}