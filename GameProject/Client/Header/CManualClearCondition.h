#pragma once

#include "CClearCondition.h"

class CManualClearCondition : public CClearCondition
{
private:
	CManualClearCondition() = delete;
	CManualClearCondition(const CManualClearCondition&) = delete;
	CManualClearCondition& operator=(const CManualClearCondition&) = delete;
	explicit CManualClearCondition(CRoomLayer* pLayer);
	virtual ~CManualClearCondition();

public:
	virtual bool IsSatisfied() override;

private:
	virtual void OnRoomEvent(const TRoomEventCtx& t) override;

public:
	static CManualClearCondition* Create(CRoomLayer* pLayer);

private:
	virtual void Free() override;
};

