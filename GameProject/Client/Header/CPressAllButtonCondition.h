#pragma once
#include "CClearCondition.h"

class CPressAllButtonCondition : public CClearCondition
{
private:
	CPressAllButtonCondition() = delete;
	CPressAllButtonCondition(const CPressAllButtonCondition&) = delete;
	CPressAllButtonCondition& operator=(const CPressAllButtonCondition&) = delete;
	explicit CPressAllButtonCondition(CRoomLayer* pLayer);
	virtual ~CPressAllButtonCondition();

	inline void AddButton() { ++m_iLeftButton; }

	virtual bool IsSatisfied() override;

private:
	virtual void OnRoomEvent(const TRoomEventCtx& t) override;
	int m_iLeftButton = 0;

public:
	static CPressAllButtonCondition* Create(CRoomLayer* pLayer);

private:
	virtual void Free() override;
};

