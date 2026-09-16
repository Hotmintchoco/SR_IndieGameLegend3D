#include "pch.h"
#include "CClearCondition.h"
#include "CRoomLayer.h"

CClearCondition::CClearCondition(CRoomLayer* pLayer) : m_pLayer(pLayer)
{
	pLayer->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) { OnRoomEvent(t); });
}

CClearCondition::~CClearCondition()
{
}

void CClearCondition::OnRoomEvent(const TRoomEventCtx& e)
{
}

void CClearCondition::Free()
{
}
