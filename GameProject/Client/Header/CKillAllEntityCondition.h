#pragma once

#include "CClearCondition.h"

class CKillAllEntityCondition : public CClearCondition
{
private:
	CKillAllEntityCondition() = delete;
	CKillAllEntityCondition(const CKillAllEntityCondition&) = delete;
	CKillAllEntityCondition& operator=(const CKillAllEntityCondition&) = delete;
	explicit CKillAllEntityCondition(CRoomLayer* pLayer);
	virtual ~CKillAllEntityCondition();

public:
	virtual bool IsSatisfied() override;

private:
	virtual void OnRoomEvent(const TRoomEventCtx& t) override;

public:
	static CKillAllEntityCondition* Create(CRoomLayer* pLayer);

private:
	virtual void Free() override;
};

