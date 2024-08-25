# PlayAround_d
+ 개발 기간 :  2024.04 ~ 진행중
+ 장르 : TPS&FPS / MultiPlayer
+ 서버 타입 : Listen_server
+ 엔진 : UE5.1

## 🚀 Trouble Shooting
## 1. Cpp 파일 분리
*Know Your Limits*를 만들때 가장 불편했던 점이 분야별로 cpp 파일을 분리하지 못한 것이다. 다른 언리얼 무료 프로젝트를 다운 받아보면 보기 좋게 정리되어 있던데..😳

그리고 드디어 알아온 방법 : 
1. 폴더는 프로젝트 파일에 직접 생성
2. 빌드에 추가 -> ```PublicIncludePaths.AddRange(new string[] { "Project2" });```
<br/><br/>

## 2. Interface 착오
```c++
UMyCharacterWidgetInterface* CharacterWidget = Cast<UMyCharacterWidgetInterface>(OwningActor);
```
> 위와 같이 작성했을 때 CharacterWidget이 자꾸 null이 됐다..

-> 인터페이스는 "U"MyCharacterWidgetInterface가 아니라 **"I"MyCharacterWidgetInterface**이다!
<br/><br/>

## 3. ReplicatedUsing 나중에 추가 시 오류
[Stat의 MaxHp와 CurrentHp](https://github.com/cubee021/PlayAround_d/blob/2b16d255d7aeeaaae81e40333d35e3bb87eaf7c1/Project2/Character/MyCharacterStatComponent.h)를 replicate해야 할 일이 생겼다. 늘 했던 것 처럼 UPROPERTY()에 ```ReplicatedUsing = OnRep_func```을 추가하고 두 값이 바뀐 것을 알려줄 Delegate를 broadcast했다. 그런데 두 값을 100으로 초기화를 했음에도 Hp bar에 100/100이 아닌 0/0과 같이 출력됐다. 상대에게 공격 받아도 한번에 쓰러져버렸다.

-> Binaries, Intermediate 파일을 **지우고 다시 시작하니** 해결됐다. 

분명히 잘 썼는데 오류가 발생한다 싶으면 삭제하고 다시 시작해보자. 값이 갱신 안되는 오류인줄 알고 몇 달을 헤맸는데.. 약간 허무😅

[도움 된 언리얼 포럼](https://forums.unrealengine.com/t/initializecomponent-not-firing-on-spawn/322782)
<br/><br/>

## 4. Skeletal Mesh Component Replication Failed
캐릭터가 보유한 아이템을 버리는 기능[(MyDropItem)](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyDropItem.cpp)을 만들었는데, Server에서 떨궜을 때 Client쪽에서 물체가 보이지 않았다. 

> 💡 참고로 Projectile은 서버에서 클라이언트로 자동 Replication된다

[MyBullet](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Weapon/MyBullet.cpp)을 만들 때 동일한 문제가 있었어서 collision을 다시 설정해봤는데도 해결되지 않았다. 그런데 OnOverlapBegin()에 로그를 찍고 Client 캐릭터를 움직여보니 반응이 있다! Projectile에는 문제가 없고 보이지만 않는다면, Skeletal Mesh component가 replicate되지 않았을 것이다.
<br/><br/>

-> **사실 Skeletal Mesh component는 Replicate를 지원한다.** [언리얼 문서](https://dev.epicgames.com/documentation/en-us/unreal-engine/component-replication?application_version=4.27)에 따르면 이 경우와 같은 Static Component는 Actor가 spawn될 때, replicate 유무에 상관없이 자동 생성된다.

> 그렇다면 무엇이 문제일까?
>> 답은 "Default"에 있다.

MyDropItem의 "생성자"에 Skeletal Mesh component를 하나 지정해주었다. 그랬더니 Client쪽에서 mesh가 보이기 시작했다. *즉, default는 자동 replicate되지만 그 이후로는 아닌것이다.*

이를 해결하기 위해서 Skeletal Mesh component에 OnRep을 추가했.. 으나 해결되진 않았다.

대신, 컴포넌트 정보를 포함하고 있는 **ItemData는 잘 전달되기 때문에 이에 대한 OnRep을 추가하고 Skeletal Mesh를 바꾸도록** 우회해서 해결했다😀

비슷한 경험을 겪은 [관련 언리얼 포럼](https://forums.unrealengine.com/t/problem-with-mesh-replication/753047)에서는 :

1. Multicast로 바뀐 Skeletal mesh 정보를 보내고,
2. OnRep으로 Skeletal mesh를 바꾸는 방법을 주로 쓰는 모양이다.

둘 중 무엇이 더 좋은 방법인지는 잘 모르겠다. Item Data에 들어있는 정보가 더 많아서 비교적 무겁지 않을까..? 

맵에 랜덤 무기를 소환하는 [MyItemBox](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyItemBox.cpp)에서도 맨 처음 뜨는 물체가 Server와 Client 각각 달라서 고민이었는데, 이제 고쳐봄직 하다🔧🔧
<br/><br/>

## 5. C++의 Enum을 블루프린트와 연동하고 싶다면..
UENUM(*Blueprint*) <- 이렇게 괄호 안에 표시
<br/><br/>

## 6. Client에 HUD widget 띄우기
남은 게임 시간을 알려주는 타이머 위젯[(MyMatchTimrWidget)](https://github.com/cubee021/PlayAround_d/blob/main/Project2/UI/MyMatchTimerWidget.cpp)을 만들었다. 컨트롤러에 추가하고 실행했는데, Server밖에 타이머가 뜨지 않는다. 

-> CreateWidget의 첫 번째 파라미터에 GetWorld()대신 **this**를 넣으면 해결된다!
(https://github.com/cubee021/PlayAround_d/blob/main/Project2/Player/MyPlayerController.cpp#L30-L31)



<br/><br/>

## 7. Seamless Travel PlayerState 복사 


<br/><br/>

## 📖 공부
## 1. 네트워킹 헷갈렸던 내용
#### 1) RPC 함수 내에서 this는 그 함수를 실행시킨 대상

|RPC|실행 대상|this|
|---|---|---|
|ServerRPC|Client|Client|
|ClientRPC|Server|Server|
|MulticastRPC|Server|Server|

#### 2) OnRep은 Server -> Client들로 상태 전송
>> DOREPLIFETIME으로 지정된 변수의 값이 변경될 때마다 전송한다. (Replicate 불가능한 data type은 제외)

#### 3) Authority는 무조건 Server가 아니다
>> Authority = Server(리슨 서버는 서버도 참여하므로) & 서버에 있는 Client 본체

[언리얼 Networking overview](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/networking-overview-for-unreal-engine)

#### 4) HasAuthority() vs IsLocallyControlled()
+ HasAuthority() :
+ IsLocallyControlled() :

#### 5) GameMode & GameState
|GameMode|GameState|
|---|---|
|Server **ONLY**|Server, Client|

GameState가 GameMode의 하위 클래스라기 보다는 :
 + GameMode : 게임의 전반적 흐름 관리
 + GameDtate : GameMode에 관련된 정보 관리. Client가 모니터링할 수 있음

[문서에서 더 알아보기](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/game-mode-and-game-state-in-unreal-engine)
<br/><br/>

## 2. Gameplay Framework
> 첫 번째 프로젝트에서는 신경 안쓰고 막 작성했다면 지금은 다르다..!

언리얼에서는 클래스마다 역할이 정해져있다. 각각 역할에 맞게 기능이 정의되어 있으므로 이에 따라 함수를 작성/정리하는 것이 바람직.
 
[언리얼 Framework Ref](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/gameplay-framework-quick-reference?application_version=4.27)
<br/><br/>

## 3. 실생활 적용
게임 플레이 시 친구가 이동하는 이유 공부해서 쓰기





