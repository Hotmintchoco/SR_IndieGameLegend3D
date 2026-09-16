#pragma once
#include "CClearCondition.h"

class CPushAllButtonCondition : public CClearCondition
{
private:
	CPushAllButtonCondition() = delete;
	CPushAllButtonCondition(const CPushAllButtonCondition&) = delete;
	CPushAllButtonCondition& operator=(const CPushAllButtonCondition&) = delete;
	explicit CPushAllButtonCondition(CRoomLayer* pLayer);
	virtual ~CPushAllButtonCondition();

	inline void AddButton() { ++m_iLeftButton; }

	virtual bool IsSatisfied() override;

private:
	virtual void OnRoomEvent(const TRoomEventCtx& t) override;
	int m_iLeftButton = 0;

public:
	static CPushAllButtonCondition* Create(CRoomLayer* pLayer);

private:
	virtual void Free() override;
};

