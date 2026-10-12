#include "pch.h"
#include "CManualClearCondition.h"
#include "CRoomLayer.h"

CManualClearCondition::CManualClearCondition(CRoomLayer* pLayer)
	: CClearCondition(pLayer)
{
	pLayer->m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t) { OnRoomEvent(t); });
}

CManualClearCondition::~CManualClearCondition()
{
}

bool CManualClearCondition::IsSatisfied()
{
	return m_bActivated;
}

void CManualClearCondition::OnRoomEvent(const TRoomEventCtx& t)
{
	switch (t.eType)
	{
	case ERoomEventType::ROOM_MANUALCLEAR_REQUEST:
		m_bActivated = true;
		break;
	}
}

CManualClearCondition* CManualClearCondition::Create(CRoomLayer* pLayer)
{
	return new CManualClearCondition(pLayer);
}

void CManualClearCondition::Free()
{
	CClearCondition::Free();
}
