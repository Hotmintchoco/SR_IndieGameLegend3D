#include "pch.h"
#include "CLiminalRoomLayer.h"

CLiminalRoomLayer::CLiminalRoomLayer(int iRoomIndex)
	: CRoomLayer(iRoomIndex)
{
}

CLiminalRoomLayer::~CLiminalRoomLayer()
{
}

HRESULT CLiminalRoomLayer::Ready_Layer()
{
	return S_OK;
}

_int CLiminalRoomLayer::Update_Layer(_float fTimeDelta)
{
	_int iExit = CRoomLayer::Update_Layer(fTimeDelta);

	return iExit;
}

void CLiminalRoomLayer::LateUpdate_Layer(_float fTimeDelta)
{
	CRoomLayer::LateUpdate_Layer(fTimeDelta);
}

CLiminalRoomLayer* CLiminalRoomLayer::Create(int iRoomIndex)
{
	CLiminalRoomLayer* pLayer = new CLiminalRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[CLiminalRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLiminalRoomLayer::Free()
{
	CRoomLayer::Free();
}
