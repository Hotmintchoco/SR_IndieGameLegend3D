#include "pch.h"
#include "CManualClearCondition.h"
#include "CRoomLayer.h"

CManualClearCondition::CManualClearCondition(CRoomLayer* pLayer)
	: CClearCondition(pLayer)
{
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

}

CManualClearCondition* CManualClearCondition::Create(CRoomLayer* pLayer)
{
	return new CManualClearCondition(pLayer);
}

void CManualClearCondition::Free()
{
	CClearCondition::Free();
}
