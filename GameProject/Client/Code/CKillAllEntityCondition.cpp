#include "pch.h"
#include "CKillAllEntityCondition.h"
#include "CRoomLayer.h"

CKillAllEntityCondition::CKillAllEntityCondition(CRoomLayer* pLayer)
	: CClearCondition(pLayer)
{
}

CKillAllEntityCondition::~CKillAllEntityCondition()
{
}

bool CKillAllEntityCondition::IsSatisfied()
{
	if (!m_bActivated) return false;

	/* 지금은 이벤트 이용하지 않고 직접 레이어의 몬스터 개수를 세는 방식을 이용 */
	return m_pLayer->GetEntityCount() == 0;
}

void CKillAllEntityCondition::OnRoomEvent(const TRoomEventCtx& t)
{
	switch (t.eType)
	{
	case ERoomEventType::ROOM_BEGIN:
		m_bActivated = true;
		break;
	default:
		break;
	}
}

CKillAllEntityCondition* CKillAllEntityCondition::Create(CRoomLayer* pLayer)
{
	return new CKillAllEntityCondition(pLayer);
}

void CKillAllEntityCondition::Free()
{
	CClearCondition::Free();
}
