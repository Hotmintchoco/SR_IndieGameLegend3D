#include "pch.h"
#include "CPushAllButtonCondition.h"
#include "Client_Struct.h"

CPushAllButtonCondition::CPushAllButtonCondition(CRoomLayer* pLayer)
	: CClearCondition(pLayer)
{
}

CPushAllButtonCondition::~CPushAllButtonCondition()
{
}

bool CPushAllButtonCondition::IsSatisfied()
{
	return m_iLeftButton == 0;
}

void CPushAllButtonCondition::OnRoomEvent(const TRoomEventCtx& t)
{
	switch (t.eType)
	{
	case ERoomEventType::PUSH_BUTTON:
		--m_iLeftButton;
		break;
	default:
		break;
	}
}

CPushAllButtonCondition* CPushAllButtonCondition::Create(CRoomLayer* pLayer)
{
	return new CPushAllButtonCondition(pLayer);
}

void CPushAllButtonCondition::Free()
{
	CClearCondition::Free();
}
