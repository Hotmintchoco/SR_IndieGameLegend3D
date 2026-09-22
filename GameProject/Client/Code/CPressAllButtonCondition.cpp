#include "pch.h"
#include "CPressAllButtonCondition.h"
#include "Client_Struct.h"

CPressAllButtonCondition::CPressAllButtonCondition(CRoomLayer* pLayer)
	: CClearCondition(pLayer)
{
}

CPressAllButtonCondition::~CPressAllButtonCondition()
{
}

bool CPressAllButtonCondition::IsSatisfied()
{
	return m_iLeftButton == 0;
}

void CPressAllButtonCondition::OnRoomEvent(const TRoomEventCtx& t)
{
	switch (t.eType)
	{
	case ERoomEventType::BUTTON:
	{
		/* 타입 완전한 경우 */
		bool bPressed = get<bool>(t.varArgs);
		/* 타입 불완전한 경우*/
		// if (int* pValue = get_if<int>(&t.varArgs))	
		m_iLeftButton += ((bPressed) ? -1 : 1);
		break;
	}
	default:
		break;
	}
}

CPressAllButtonCondition* CPressAllButtonCondition::Create(CRoomLayer* pLayer)
{
	return new CPressAllButtonCondition(pLayer);
}

void CPressAllButtonCondition::Free()
{
	CClearCondition::Free();
}
