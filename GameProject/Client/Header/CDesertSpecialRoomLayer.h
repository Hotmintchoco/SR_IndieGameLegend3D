#pragma once

#include "CRoomLayer.h"

class CDesertSpecialRoomLayer : public CRoomLayer
{
private:
    explicit CDesertSpecialRoomLayer(int iRoomIndex);
    virtual ~CDesertSpecialRoomLayer() = default;

    HRESULT Ready_Layer() override;

public:
    void OnRoomTriggerBlockCollided() override;
    static CDesertSpecialRoomLayer* Create(int iRoomIndex);

private:
	_bool m_bStart = false;
};
