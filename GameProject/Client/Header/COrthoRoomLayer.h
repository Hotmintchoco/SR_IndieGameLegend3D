#pragma once

#include "CRoomLayer.h"

enum GAMEMODE { PERSP, ORTHO };

class COrthoCamera;
class CPlayer;
class CFlatPlayer;
class IVerticalTerrain;
class CCubeNetKeyBlock;
class COrthoMarker;
class COrthoEmptyBlocker;
class CCubeNet;

class COrthoRoomLayer : public CRoomLayer
{
protected:
	explicit COrthoRoomLayer(int iRoomIndex);
	virtual ~COrthoRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

	const vector<IVerticalTerrain*>& GetVerticalTerrainList() { return m_vecVerticalTerrain; }

	/* 페이즈 게이트가 충돌 Enter/Exit에서 켜고 끄는 스위치 */
	inline void SetPlayerOnGate(bool bFlag) { m_bPlayerOnGate = bFlag; }
	inline void SetFlatPlayerOnGate(bool bFlag) { m_bFlatPlayerOnGate = bFlag; }

	/* 열쇠가 획득될 때 호출 : 남은 키 카운트를 줄이고, 0이 되면 열쇠 블럭 파괴 */
	void OnKeyAcquired();

	/* 플레이어가 플랭크를 들고/내려놓을 때 호출 : 마커 표시 여부도 같이 갱신 */
	void SetGrabbing(bool bFlag);
	/* 플레이어가 들고 있는 동안 매 프레임 호출 : 들고 있는 물체 위치 기준으로 마커를 가장 가까운 타일 센터로 */
	void UpdateMarkerPos(const _vec3& vHeldPos);

	/* 플레이어가 내려놓기 시점에 요청 : 들고 있던 위치의 가장 가까운 칸에 물체를 놓는다
	   - 일반 바닥 : 타일 센터에서 y +0.5
	   - 뚫린 바닥 : 구멍을 메움 (y -0.5, 공백 상태 해제, 블로커 제거) */
	void RequestPlaceOrthoObject(CCubeNet* pObj, const _vec3& vHeldPos);

protected:
	virtual HRESULT SpawnRoom() override;
	void RemoveDeadTerrain(IVerticalTerrain* pTerrain);
	void ClearCurrentPhase();
	void DestroyKeyBlocks();
	/* 뚫린 바닥 배치 : 블로커 생성 + 같은 칸 타일 비활성화 + 공백 상태 기록 */
	HRESULT SpawnEmptyTile(int iTileX, int iTileZ);
	inline int ToFlatIndex(const TTileIdx& tIdx) const { return tIdx.iRow * m_iTileCountX + tIdx.iCol; }

	GAMEMODE m_eMode = PERSP;

	bool m_bRegistered = false;
	COrthoCamera* m_pOrthoCamera = nullptr;
	CPlayer* m_pPlayer = nullptr;
	CFlatPlayer* m_pFlatPlayer = nullptr;

	vector<IVerticalTerrain*> m_vecVerticalTerrain;

	/* 페이즈 클리어 조건 : 두 플레이어가 동시에 게이트에 닿아 있어야 함 */
	bool m_bPlayerOnGate = false;
	bool m_bFlatPlayerOnGate = false;
	bool m_bWasBothOnGate = false;
	int m_iCurrentPhase = 0;

	/* 열쇠 : 키를 배치할 때마다 카운트를 올리고, 획득할 때마다 내린다 */
	int m_iKeyCount = 0;
	vector<CCubeNetKeyBlock*> m_vecKeyBlock;

	/* 내려놓기 위치 마커 */
	COrthoMarker* m_pMarker = nullptr;
	bool m_bIsGrabbing = false;

	/* 뚫린 바닥 : 타일 전체 크기만큼, 인덱스는 iRow * m_iTileCountX + iCol */
	int m_iTileCountX = 0;
	vector<bool> m_vecEmptyTile;
	vector<COrthoEmptyBlocker*> m_vecEmptyBlocker;

public:
	static COrthoRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

