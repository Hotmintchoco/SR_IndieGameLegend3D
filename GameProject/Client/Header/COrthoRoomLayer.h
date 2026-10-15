#pragma once

#include "CRoomLayer.h"

enum GAMEMODE { PERSP, ORTHO };

/* 직교 물체 내려놓기 결과 */
enum class EPlaceResult { PLACED, FILLED_HOLE, BLOCKED };

class COrthoCamera;
class CPlayer;
class CFlatPlayer;
class IVerticalTerrain;
class CCubeNetKeyBlock;
class COrthoMarker;
class COrthoEmptyBlocker;
class CCubeNet;
class CCubeNetPlank;
class CTile;
class CGameMachine;
class CUI_PhaseClear;
class CUI_OrthoIntro;
class CUI_PhaseNumber;

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

	/* 2D 플레이어가 용암에 닿으면 호출 : 다음 업데이트에서 현재 페이즈를 처음 상태로 리셋 (충돌 콜백 중 생성/삭제 방지) */
	inline void RequestResetPhase() { if (m_bPhaseStarted) m_bPendingResetPhase = true; }

	/* 플레이어가 플랭크를 들고/내려놓을 때 호출 : 마커 표시 여부도 같이 갱신 */
	void SetGrabbing(bool bFlag);
	/* 플레이어가 들고 있는 동안 매 프레임 호출 : 들고 있는 물체 위치 기준으로 마커를 가장 가까운 타일 센터로 */
	void UpdateMarkerPos(const _vec3& vHeldPos);

	/* 플레이어가 내려놓기 시점에 요청 : 들고 있던 위치의 가장 가까운 칸에 물체를 놓는다
	   - 일반 바닥 : 타일 센터에서 y +0.5
	   - 뚫린 바닥 : 구멍을 메움 (공백 상태 해제, 블로커 제거)
	   - 다른 블럭이 있는 칸 : BLOCKED, 아무것도 바꾸지 않음 (계속 들고 있어야 함)
	   놓을 수 있으면 로컬(소켓) 모드를 해제하고 월드 위치로 옮김 */
	EPlaceResult RequestPlaceOrthoObject(CCubeNet* pObj, const _vec3& vHeldPos);

protected:
	virtual HRESULT SpawnRoom() override;
	/* 페이즈 단위 스폰 : 블럭/열쇠/게이트 배치 + 페이즈별 3D/2D 플레이어 시작 위치 */
	HRESULT SpawnPhase(int iPhase);
	void RemoveDeadTerrain(IVerticalTerrain* pTerrain);
	void ClearCurrentPhase();
	/* 다음 페이즈로 교체 : 현재 페이즈 정리 -> 인덱스 증가 -> 스폰 (커튼이 다 덮였을 때 호출) */
	void AdvancePhase();
	/* 페이즈 전환(클리어 연출 ~ 다음 페이즈 시작) 동안 2D/3D 입력 잠금 */
	void LockTransitionInput();
	void UnlockTransitionInput();
	/* 현재 페이즈 정리 후 같은 페이즈를 다시 스폰 + 2D 플레이어 깜빡임 */
	void ResetCurrentPhase();
	/* 현재 페이즈 정리 : 페이즈 전용 오브젝트 Set_Dead + 페이즈 상태 초기화 */
	void CleanCurrentPhase();
	/* 페이즈 전용 오브젝트로 등록 : CleanCurrentPhase에서 일괄 제거 */
	void RegisterPhaseObject(CGameObject* pObj);
	/* 마지막 페이즈 클리어 후 : 미니게임 시작 전 상태(입력, 카메라, 타임스케일)로 되돌림 */
	void EndOrthoGame();
	/* 시점 전환 : 카메라 투영/궤도, 마커, 입력, 타임스케일을 한 번에 바꿈 (R 키와 시작 시 공용) */
	void SetGameMode(GAMEMODE eMode);
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

	/* 페이즈 전환 커튼 : 방 위(높이 2.5)에 미니 격자 크기만큼 블럭을 깔아두고 전환 때만 켬
	   행 + 열 인덱스의 합(대각선)이 같은 블럭끼리 같은 타이밍에 커졌다(덮기) 작아짐(걷기) */
	HRESULT SpawnCurtainBlocks();
	void RemoveCurtainBlocks();
	void StartCurtain();
	void UpdateCurtain(float fTimeDelta);
	inline bool IsCurtainPlaying() const { return m_eCurtainState != ECurtainState::NONE; }
	/* 클리어 처리 중 (클리어 UI 재생 ~ 커튼이 다 걷힐 때까지) : 게이트 / 0키 / 용암 리셋을 받지 않음 */
	inline bool IsTransitioning() const { return m_eClearFollowUp != EClearFollowUp::NONE || IsCurtainPlaying(); }
	/* 클리어 UI 재생이 끝난 뒤 처리 : 다음 페이즈면 커튼 시작, 마지막 페이즈면 미니게임 정리 */
	void OnPhaseClearUIFinished();
	/* 마지막 페이즈 클리어 후 : 페이즈 정리 -> 미니게임 종료 -> 방 클리어 요청 */
	void FinishOrthoGame();
	/* 미니게임 실제 시작 : 미니 타일/커튼/페이즈 스폰, BGM, 직교 시점 전환 (입장 연출이 끝난 뒤 호출) */
	void StartOrthoGame();
	/* 방 벽 4개의 벽지 : true면 미니게임용으로 교체, false면 원래대로 */
	void SetMiniGameWallpaper(bool bOn);

	/* 카메라 연출
	   - 입장 : 입장 키를 누른 순간 플레이어 위치에서 출발해 직교 카메라 Orbit으로 게임기 화면 앞까지 다가감 -> 인트로 UI (아무 키) -> 게임 시작
	   - 퇴장 : 플레이어를 방 중앙 앞에 세우고, 시네 카메라로 플레이어 뒤 위쪽에서 눈높이까지 내려온 뒤 플레이어 카메라로 */
	void StartEnterDirecting();
	void StartExitDirecting();
	void FinishExitDirecting();
	void UpdateDirecting(float fTimeDelta);

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
	int m_iLastPhase = 1;

	/* 게임기 : 상호작용하면 시작 요청을 받아두고 제거된다 */
	CGameMachine* m_pGameMachine = nullptr;

	/* 페이즈 클리어 UI : 방 생성 시 만들어 두고 클리어할 때 Play */
	CUI_PhaseClear* m_pPhaseClearUI = nullptr;
	/* 좌상단 페이즈 표시 UI : 방 생성 시 만들어 두고 미니게임 동안만 보임. 페이즈 스폰 때 값 갱신 */
	CUI_PhaseNumber* m_pPhaseNumberUI = nullptr;
	/* 전환 중 입력 잠금 여부 (잠금/해제 짝을 맞추기 위해) */
	bool m_bTransitionInputLocked = false;

	/* 클리어 UI 재생이 끝난 뒤 할 일 */
	enum class EClearFollowUp { NONE, NEXT_PHASE, END_GAME };
	EClearFollowUp m_eClearFollowUp = EClearFollowUp::NONE;
	/* 클리어 UI 종료 알림을 받았는지 : 알림은 UI 업데이트 도중에 오므로 플래그만 켜고 레이어 업데이트에서 처리 */
	bool m_bClearUIFinished = false;
	bool m_bPendingStartPhase = false;
	bool m_bPhaseStarted = false;
	bool m_bPendingResetPhase = false;
	/* 미니게임 시작 전 BGM : 종료 시 그 시점부터 이어서 재생 */
	TBGMSnapshot m_tPrevBGM{};

	/* 카메라 연출 상태 */
	enum class EDirecting { NONE, ENTERING, EXITING };
	EDirecting m_eDirecting = EDirecting::NONE;
	/* 입장 단계 완료 알림 : 알림은 카메라/UI 업데이트 도중에 오므로 플래그만 켜고 레이어 업데이트에서 처리 */
	bool m_bEnterOrbitFinished = false;
	bool m_bIntroFinished = false;
	/* 입장 인트로 UI : 방 생성 시 만들어 두고 Orbit이 끝나면 Play */
	CUI_OrthoIntro* m_pIntroUI = nullptr;
	/* 입장 : 방 중앙 바닥 기준. 시선(S)은 y 1.5, 끝 위치(I)는 거기서 -z로 0.5 */
	inline static const _vec3 s_vEnterAt{ 0.f, 1.0f, 0.f };
	inline static const _vec3 s_vEnterEye{ 0.f, 1.0f, -0.3f };
	static constexpr float s_fEnterDuration = 1.5f;	/* 다가가는 시간 */
	/* 퇴장 : 방 중앙 바닥 기준. 플레이어는 (0, -, -3), 카메라는 (0, 4, -4) -> (0, 1, -4) 로 내려오며 +z 방향을 봄 */
	inline static const _vec3 s_vExitPlayerPos{ 0.f, 0.f, -3.f };
	inline static const _vec3 s_vExitEyeFrom{ 0.f, 4.f, -4.f };
	inline static const _vec3 s_vExitEyeTo{ 0.f, 1.f, -4.f };
	static constexpr float s_fExitDuration = 2.5f;

	/* 페이즈 전환 커튼 */
	enum class ECurtainState { NONE, CLOSING, OPENING };
	ECurtainState m_eCurtainState = ECurtainState::NONE;
	float m_fCurtainTime = 0.f;							/* 현재 단계(덮기/걷기) 시작 후 지난 시간 */
	int m_iCurtainSizeX = 0;								/* 열 개수 */
	int m_iCurtainSizeZ = 0;								/* 행 개수 */
	vector<CCubeNetPlank*> m_vecCurtainBlock;				/* 인덱스 : iRow * m_iCurtainSizeX + iCol */
	static constexpr float s_fCurtainHeight = 2.5f;		/* 블럭 높이 (방 오브젝트와 겹치지 않는 높이) */
	static constexpr float s_fCurtainStepDelay = 0.03f;	/* 대각선 한 줄마다 시작 시간 차이 */
	static constexpr float s_fCurtainScaleTime = 0.25f;	/* 블럭 하나가 커지는(작아지는) 시간 */
	static constexpr float s_fCurtainHoldTime = 0.15f;	/* 다 덮인 뒤 걷기 전까지 대기 */

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
	/* 미니 타일 배경 애니메이션 공용 시간 : 레이어가 흘리고 타일은 읽기만 (뚫린 칸이 꺼져 있어도 장이 어긋나지 않도록) */
	float m_fMiniTileTime = 0.f;
	vector<CTile*> m_vecOriginTile;

	/* 뚫린 바닥 : 미니 타일 격자 크기만큼, 인덱스는 iRow * m_iTileCountX + iCol */
	int m_iTileCountX = 0;
	vector<bool> m_vecEmptyTile;
	vector<COrthoEmptyBlocker*> m_vecEmptyBlocker;

	/* 칸 점유 : 미니 격자 크기만큼, 그 칸에 놓인 큐브넷 (죽은 블럭은 토큰 만료로 자동으로 빈 칸 취급) */
	struct TTileOccupant
	{
		CCubeNet* pObj = nullptr;
		weak_ptr<void> wToken;
	};
	vector<TTileOccupant> m_vecTileOccupant;
	/* pIgnore는 점유자로 치지 않음 (들고 있는 물체가 원래 있던 칸) */
	bool IsTileOccupied(int iIndex, CCubeNet* pIgnore) const;
	/* pObj가 차지하던 칸을 비우고 iIndex 칸에 등록 (iIndex < 0 이면 비우기만) */
	void SetTileOccupant(int iIndex, CCubeNet* pObj);
	/* 뚫린 바닥 때문에 꺼둔 타일 : 페이즈 정리 시 다시 켬 (타일은 방 소유라 포인터 유지됨) */
	vector<CTile*> m_vecDisabledTile;

	/* 페이즈 전용 오브젝트 : 중간에 먼저 죽을 수 있으므로 토큰으로 생존 확인 후 Set_Dead */
	vector<pair<CGameObject*, weak_ptr<void>>> m_vecPhaseObject;

public:
	static COrthoRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

