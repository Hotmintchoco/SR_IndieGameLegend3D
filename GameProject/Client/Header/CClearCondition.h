#pragma once

#include "CBase.h"

class CRoomLayer;
struct TRoomEventCtx;

class CClearCondition : public CBase
{
public:
	CClearCondition() = delete;
	CClearCondition(const CClearCondition&) = delete;
	CClearCondition& operator=(const CClearCondition&) = delete;
	explicit CClearCondition(CRoomLayer* pLayer);
	virtual ~CClearCondition();

	virtual void OnRoomEvent(const TRoomEventCtx& e);
	virtual bool IsSatisfied() PURE;

protected:
	bool m_bActivated = false;

	CRoomLayer* m_pLayer = nullptr;

protected:
	virtual void Free() override;
};

