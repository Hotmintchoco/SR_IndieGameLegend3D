#include "pch.h"
#include "CDesertSpecialRoomLayer.h"
#include "CManagement.h"
#include "CStage.h"
#include "CPlayer.h"
#include "CClientCameraMgr.h"
#include "CPlayerCamera.h"

CDesertSpecialRoomLayer::CDesertSpecialRoomLayer(int iRoomIndex)
    : CRoomLayer(iRoomIndex)
{
    m_bEnableDesertVeil = false;
}

HRESULT CDesertSpecialRoomLayer::Ready_Layer()
{
    if (FAILED(CRoomLayer::Ready_Layer()))
        return E_FAIL;

    m_OnRoomEvent.AddBinding(GetToken(), [this](const TRoomEventCtx& t)
        {
            if (t.eType != ERoomEventType::ROOM_CLEAR)
                return;

            // 이 방이 클리어되는 순간 실행
            auto* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
            if (pStage->GetPlayer() && m_bStart)
            {
                pStage->GetPlayer()->SetPseudoScale(1.f, 1.f);
                pStage->GetPlayer()->SetForcedSprint(false);
                CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::FIRST_PERSON);

                auto pPlayerCam = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Get_ActiveCamera());
                if (pPlayerCam)
                    pPlayerCam->Set_Distance(pPlayerCam->Get_Distance() * 3.3f);
            }
        });

    return S_OK;
}

void CDesertSpecialRoomLayer::OnRoomTriggerBlockCollided()
{
    CRoomLayer::OnRoomTriggerBlockCollided();

    auto* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage && pStage->GetPlayer() && !m_bStart)
    {
        m_bStart = true;

        pStage->GetPlayer()->SetPseudoScale(0.3f, 1.f);
        pStage->GetPlayer()->SetForcedSprint(true);
		CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::THIRD_PERSON);

        auto pPlayerCam = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Get_ActiveCamera());
        if (pPlayerCam)
            pPlayerCam->Set_Distance(pPlayerCam->Get_Distance() * 0.3f);
    }
}

CDesertSpecialRoomLayer* CDesertSpecialRoomLayer::Create(int iRoomIndex)
{
    auto* pLayer = new CDesertSpecialRoomLayer(iRoomIndex);
    if (FAILED(pLayer->Ready_Layer()))
    {
        Safe_Release(pLayer);
        return nullptr;
    }
    return pLayer;
}
