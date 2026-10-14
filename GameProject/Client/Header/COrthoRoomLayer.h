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
class CTile;
class CGameMachine;

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

	/* 현재 시점(원근/직교)과 직교 카메라 : 빌보드 처리용 */
	inline GAMEMODE GetGameMode() const { return m_eMode; }
	inline COrthoCamera* GetOrthoCamera() { return m_pOrthoCamera; }
	/* 2D 플레이어 이동 범위 : 방 안쪽 영역의 x/z 최소·최대 (_vec2의 y에 z가 들어감) */
	void GetFlatBounds(_vec2& vMin, _vec2& vMax);

	/* 타일 유닛 : 미니 타일이 깔려 있는 동안만 0.5, 시작 전/종료 후에는 기본 방과 같은 1 */
	virtual float GetTileUnit() const override { return m_bMiniTileActive ? m_fMiniTileUnit : 1.f; }

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
	/* 페이즈 단위 스폰 : 블럭/열쇠/게이트 배치 + 페이즈별 3D/2D 플레이어 시작 위치 */
	HRESULT SpawnPhase(int iPhase);
	void RemoveDeadTerrain(IVerticalTerrain* pTerrain);
	void ClearCurrentPhase();
	/* 현재 페이즈 정리 : 페이즈 전용 오브젝트 Set_Dead + 페이즈 상태 초기화 */
	void CleanCurrentPhase();
	/* 페이즈 전용 오브젝트로 등록 : CleanCurrentPhase에서 일괄 제거 */
	void RegisterPhaseObject(CGameObject* pObj);
	/* 마지막 페이즈 클리어 후 : 미니게임 시작 전 상태(입력, 카메라, 타임스케일)로 되돌림 */
	void EndOrthoGame();
	void DestroyKeyBlocks();
	/* 뚫린 바닥 배치 : 블로커 생성 + 같은 칸 타일 비활성화 + 공백 상태 기록 */
	HRESULT SpawnEmptyTile(int iTileX, int iTileZ);
	inline int ToFlatIndex(const TTileIdx& tIdx) const { return tIdx.iRow * m_iTileCountX + tIdx.iCol; }
	/* 미니 타일이 깔려 있으면 미니 타일, 아니면 기존 타일 */
	virtual CTile* GetTileFromIndex2D(const TTileIdx& tIdx) override;
	/* 미니게임 시작 : 기존 타일을 모두 끄고 0.5 크기 미니 타일을 새로 깜 */
	HRESULT SpawnMiniTiles();
	/* 미니게임 종료 : 미니 타일 제거 + 기존 타일 다시 켬 */
	void RemoveMiniTiles();

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
	int m_iLastPhase = 2;

	/* 게임기 : 상호작용하면 시작 요청을 받아두고 제거된다 */
	CGameMachine* m_pGameMachine = nullptr;
	bool m_bPendingStartPhase = false;
	bool m_bPhaseStarted = false;

	/* 열쇠 : 키를 배치할 때마다 카운트를 올리고, 획득할 때마다 내린다 */
	int m_iKeyCount = 0;
	vector<CCubeNetKeyBlock*> m_vecKeyBlock;

	/* 내려놓기 위치 마커 */
	COrthoMarker* m_pMarker = nullptr;
	bool m_bIsGrabbing = false;

	/* 미니 타일 : 시작 시 깔고 종료 시 제거. 기존 타일은 꺼둔 채로 따로 들고 있다가 종료 시 다시 켬 */
	static constexpr float m_fMiniTileUnit = 0.5f;
	bool m_bMiniTileActive = false;
	vector<CTile*> m_vecMiniTile;
	vector<CTile*> m_vecOriginTile;

	/* 뚫린 바닥 : 미니 타일 격자 크기만큼, 인덱스는 iRow * m_iTileCountX + iCol */
	int m_iTileCountX = 0;
	vector<bool> m_vecEmptyTile;
	vector<COrthoEmptyBlocker*> m_vecEmptyBlocker;
	/* 뚫린 바닥 때문에 꺼둔 타일 : 페이즈 정리 시 다시 켬 (타일은 방 소유라 포인터 유지됨) */
	vector<CTile*> m_vecDisabledTile;

	/* 페이즈 전용 오브젝트 : 중간에 먼저 죽을 수 있으므로 토큰으로 생존 확인 후 Set_Dead */
	vector<pair<CGameObject*, weak_ptr<void>>> m_vecPhaseObject;

public:
	static COrthoRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

