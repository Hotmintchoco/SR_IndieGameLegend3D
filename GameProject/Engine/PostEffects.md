# 후처리 효과 추가 방법

현재는 등록된 효과 중 하나를 선택해 화면 전체에 적용합니다. 여러 효과를 연속으로 합성하는 구조는 아닙니다. 기존과 같이 게임 UI까지 후처리되고 ImGui는 이후에 그려집니다.

## 역할

- `CPostEffect`: 화면 캡처, HLSL 컴파일, 전체 화면 사각형 출력, 렌더 상태 복원, 실패 시 원본 화면 출력.
- `CUnderwaterEffect`: 수중 효과 시간, 물방울 텍스처, HLSL 상수 설정.
- `CRenderer`: 효과 등록·소유, 활성 효과 선택, Update/Begin/End 호출.
- `CStage::ApplyRoomShader`: 맵 특성과 효과 종류 연결.

수중 HLSL은 기존 `Client/Bin/Resource/Shader/WaterDrop.hlsl`을 사용합니다. 색상과 혼합 강도도 이 파일에 유지됩니다. 수중 왜곡 강도와 속도는 `CUnderwaterEffect`의 두 Set 함수로 조절합니다.

## 새 효과 추가

1. `CPostEffect.h`의 `POST_EFFECT`에 새 종류를 추가합니다.
2. `CPostEffect`를 상속한 클래스를 만듭니다. 생성자에서 HLSL 파일명을 전달하고, `Bind_Resources()`에서 해당 효과의 상수와 추가 텍스처를 설정합니다.
3. 추가 텍스처가 필요하면 `Ready_Resources()`에서 한 번 로딩하고 파생 클래스 소멸자에서 해제합니다. 시간 변화가 필요하면 `Update()`를 구현합니다.
4. 새 헤더와 cpp를 Engine 프로젝트에 추가합니다.
5. `CRenderer` 생성자에서 `Register_PostEffect()`로 등록합니다.
6. `CStage::ApplyRoomShader()`에서 원하는 맵에 `Set_PostEffect()`로 연결합니다.

아래 이름은 추가할 효과를 가정한 예시입니다.

```cpp
Register_PostEffect(POST_EFFECT::HEAT, new CHeatEffect);
Set_PostEffect(POST_EFFECT::HEAT);
Set_PostEffect(POST_EFFECT::NONE); // 효과 해제
```

등록 성공 시 렌더러가 객체를 소유합니다. 중복 종류나 동일 포인터의 중복 등록은 거부합니다. 실패한 등록의 객체는 호출자가 관리해야 합니다. 등록되지 않은 종류로 전환하면 false를 반환하고 기존 선택을 유지합니다. 캡처 도중 선택이 바뀌어도 해당 프레임은 Begin에서 선택했던 효과로 End합니다.

## HLSL 규칙

- 파일은 실행 파일 옆 `Resource/Shader`에 둡니다.
- 엔트리 포인트는 `main`, 셰이더 모델은 `ps_2_0`입니다.
- `s0`는 캡처한 게임 화면입니다. 추가 텍스처에는 `s1`부터 사용합니다.
- `Bind_Resources()`에 전달되는 화면 크기로 필요한 텍셀 크기를 계산합니다. 상수 레지스터 배치는 효과마다 정할 수 있습니다.
- 기본 샘플러 설정은 Linear/Clamp입니다. 필요하면 `Bind_Resources()`에서 변경할 수 있습니다.
- 파일은 UTF-8로 저장합니다. UTF-8 BOM도 로더가 제거합니다. 현재 HLSL은 독립 파일로 컴파일하며 include는 지원하지 않습니다.

쉐이더는 처음 효과를 사용할 때 컴파일하고 캐시합니다. 수정 후 게임을 재실행해야 합니다. 실패 내용은 디버그 출력에 표시되고 원본 화면으로 돌아갑니다. 다른 효과로 전환했다가 다시 선택하면 실패한 효과의 준비를 재시도합니다.
