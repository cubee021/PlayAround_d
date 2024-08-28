# PlayAround_d
+ 개인 프로젝트
+ 개발 기간 :  2024.04 ~ 진행중
+ 장르 : TPS&FPS / Multiplayer
+ 서버 타입 : Listen_server
+ 엔진 : UE5.1

## 🚀 Trouble Shooting
## 1. Cpp 파일 분리
*Know Your Limits*를 만들때 가장 불편했던 점이 분야별로 cpp 파일을 분리하지 못한 것이다. 다른 언리얼 무료 프로젝트를 다운 받아보면 보기 좋게 정리되어 있던데..😳 매번 시도하면 크래시만 났었다.

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

## 3. Client쪽 Bullet Fire/Hit 판정
언리얼 포럼에서 사용하는 방식을 두 가지로 추렸다 :
1. **Server**에서 해당 Client의 총알 생성 / replicate / 판정. Client는 애니메이션만 수행
2. **Client**에서 안보이는 총알 생성 / 판정 & 판정 결과를 Server로 보냄. Server는 총알 생성 / replicate
<br/>

> 각각 장단점이 있을거라 생각하지만, "1번"을 채택했다. 그 이유는..
+ Server가 모든 Client의 정보를 가지고 있으므로, **Server에서 판정하는 것이 옳다**고 판단
+ 총알이 벽에 튕기고 구르기 때문에 Client와 Server의 [Bullet](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Weapon/MyBullet.cpp)을 따로 생성하면 싱크로가 맞지 않는다.
  + ex) 벽에 튕겼을 때 Client에서는 오른쪽, Server에서는 왼쪽으로 날아갔다면 상대가 왼쪽에 있음에도 총에 맞지 않은 것으로 판정이 날 수 있다👻👻
+ 구현 측면에서도 훨씬 간단하다..!

구현부 : [AMyCharacterPlayer::Fire()](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Character/MyCharacterPlayer.cpp)
<br/><br/>

## 4. ReplicatedUsing 나중에 추가 시 오류
[Stat의 MaxHp와 CurrentHp](https://github.com/cubee021/PlayAround_d/blob/2b16d255d7aeeaaae81e40333d35e3bb87eaf7c1/Project2/Character/MyCharacterStatComponent.h)를 replicate해야 할 일이 생겼다. 늘 했던 것 처럼 UPROPERTY()에 ```ReplicatedUsing = OnRep_func```을 추가하고 두 값이 바뀐 것을 알려줄 Delegate를 broadcast했다. 그런데 두 값을 100으로 초기화를 했음에도 Hp bar에 100/100이 아닌 0/0과 같이 출력됐다. 상대에게 공격 받아도 한번에 쓰러져버렸다.

-> Binaries, Intermediate 파일을 **지우고 다시 시작하니** 해결됐다. 

분명히 잘 썼는데 오류가 발생한다 싶으면 삭제하고 다시 시작해보자. 값이 갱신 안되는 오류인줄 알고 몇 달을 헤맸는데.. 약간 허무😅

[도움 된 언리얼 포럼](https://forums.unrealengine.com/t/initializecomponent-not-firing-on-spawn/322782)
<br/><br/>

## 5. Skeletal Mesh Component Replication Failed
캐릭터가 보유한 아이템을 버리는 기능[(MyDropItem)](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyDropItem.cpp)을 만들었는데, Server에서 떨궜을 때 Client쪽에서 물체가 보이지 않았다. 

> 💡 참고로 Projectile은 서버에서 클라이언트로 자동 Replication된다

[MyBullet](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Weapon/MyBullet.cpp)을 만들 때 동일한 문제가 있었어서 collision을 다시 설정해봤는데도 해결되지 않았다. 그런데 OnOverlapBegin()에 로그를 찍고 Client 캐릭터를 움직여보니 반응이 있다! 추가로, MyDropItem에 디폴트로 Skeletal Mesh를 하나 지정해주었더니 Client쪽에서 mesh가 보이기 시작했다.

*즉, 디폴트 이후 동적으로 바뀌지 않는 상황이다.*

이를 통해 "Skeletal Mesh component가 replicate되지 않았다"고 추측할 수 있다.
<br/><br/>

-> **실제로 Skeletal Mesh component는 replicate를 지원하지 않는다.** [네트워킹 개요](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/networking-overview-for-unreal-engine)를 쭉 내려보면 replicate되지 않는 목록이 적혀 있는데, Skeletal Mesh component가 그 중 하나이다. 그래서 아무리 값을 바꿔도 꼼짝도 하지 않았던 것이다.

이를 해결하기 위해서 Skeletal Mesh component에 OnRep을 추가했.. 으나 해결되진 않았다.

대신, 컴포넌트 정보를 포함하고 있는 **ItemData는 잘 전달되기 때문에 이에 대한 OnRep을 추가하고 Skeletal Mesh를 바꾸도록** 우회해서 해결했다😀

비슷한 경험을 겪은 [관련 언리얼 포럼](https://forums.unrealengine.com/t/problem-with-mesh-replication/753047)에서는 :

1. Multicast로 바뀐 Skeletal mesh 정보를 보내고,
2. OnRep으로 Skeletal mesh를 바꾸는 방법을 주로 쓰는 모양이다.

둘 중 무엇이 더 좋은 방법인지는 잘 모르겠다. Item Data에 들어있는 정보가 더 많아서 비교적 무겁지 않을까..? 

맵에 랜덤 무기를 소환하는 [MyItemBox](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyItemBox.cpp)에서도 맨 처음 뜨는 물체가 Server와 Client 각각 달라서 고민이었는데, 이제 고쳐봄직 하다🔧🔧
<br/><br/>

## 6. C++의 Enum을 블루프린트와 연동하고 싶다면..
UENUM(*Blueprint*) <- 이렇게 괄호 안에 표시
<br/><br/>

## 7. Client에 HUD widget 띄우기
남은 게임 시간을 알려주는 타이머 위젯[(MyMatchTimrWidget)](https://github.com/cubee021/PlayAround_d/blob/main/Project2/UI/MyMatchTimerWidget.cpp)을 만들었다. 컨트롤러에 추가하고 실행했는데, Server밖에 타이머가 뜨지 않는다. 

-> CreateWidget의 첫 번째 파라미터에 GetWorld()대신 **this**를 넣으면 된다!

https://github.com/cubee021/PlayAround_d/blob/b7a3178a2b7c20dc0c4f11a120c9cc8dea22ace7/Project2/Player/MyPlayerController.cpp#L30-L30

#### 💡 추가로 알게 된 사실
+ UserWidget은 replicate되지 않는다. Local로만 존재하기 떄문에 Server와 Client 각각 구현해줘야 한다.
<br/><br/>

## 8. Seamless Travel 할 때 데이터 넘기기
메인 맵에 미니게임 형식으로 점프맵을 추가하고 싶었다. 메인 맵에서 점프맵으로 Server를 비롯한 모든 Client를 이동시키기 위해 Seamless Travel을 활성화했다.

분명 서버와 연결이 끊기지 않는다고 해서 모두 그대로 이동할 것 같았는데, 플레이어 캐릭터가 모두 초기화 됐다..

-> [문서](https://dev.epicgames.com/documentation/en-us/unreal-engine/travelling-in-multiplayer-in-unreal-engine?application_version=5.1)를 살펴보면 다음의 경우를 제외하면 모두 데이터를 잃는다고 한다 :
+ GameMode
+ Controllers & PlayerState
+ GetSeamlessTravelActorList()를 통해 추가된 Actor들

> 심지어 PlayerController와 Pawn의 연결성도 잃는다..;;
<br/>

#### 그래도 PlayerState는 CopyProperties를 override해서 해결할 수 있다! [도움 된 언리얼 포럼](https://forums.unrealengine.com/t/how-can-i-use-seamless-travel-to-persist-data-across-map-changes/317174)

https://github.com/cubee021/PlayAround_d/blob/15474570a421d4b7a1532ff3a374c33353367087/Project2/Game/MyPlayerState.cpp#L12-L22
<br/>

> 여담 - 현재 Seamless Travel 이후
> + 보존되는 것 : 플레이어 스코어
> + 현재 보존되지 않는 것 : Pawn을 비롯한 Component 데이터(Health, 인칭 시점, Weapon Mode)
> 
> 과 같이 정리할 수 있겠다. 안타깝게도 플레이어들이 서로 다른 점프맵으로 이동하는 큰 문제도 남아있다.
<br/><br/>

## 📖 공부
## 1. 네트워크 (헷갈렸던 내용 위주로🌠)
+ **RPC 함수 내에서 this는 그 함수를 실행시킨 대상**
  
  |RPC|실행 대상|this|
  |---|---|---|
  |ServerRPC|Client|Client|
  |ClientRPC|Server|Server|
  |MulticastRPC|Server|Server|

+ **OnRep은 "Server -> Clients"로 상태 전송**

  : DOREPLIFETIME으로 지정된 변수의 값이 변경될 때마다 전송한다. (Replicate 불가능한 data type은 제외)

+ **Authority는 무조건 Server가 아니다**

  : Authority = Server(리슨 서버는 서버도 참여하므로) & 서버에 있는 Client 본체

<br/>

+ **HasAuthority() vs IsLocallyControlled()**
  + HasAuthority() : 권한이 있으면 true. 프로젝트에서는 주로 Client Proxy와 서버에 있는 본체를 구분할 때 사용했다.
  + [IsLocallyControlled()](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/GameFramework/AController/IsLocalController?application_version=5.1) : 로컬 컨트롤러면 true. Local은 아래와 같이 정의할 수 있다.
    
    ||Server|Client|
    |---|---|---|
    |Server|Local|Remote|
    |Client|Remote|Local|

    > 🚀 Trouble Shooting - *7. Client에 HUD widget 띄우기*를 생각해보자.
  
    **UserWidget은 Local로만 존재하기 떄문에 Server와 Client 각각 구현해줘야 한다.**

    -> Server에서 Server를 바라보면 로컬, Client에서 Client를 바라보면 로컬

<br/>

+ GameMode & GameState
  
  |GameMode|GameState|
  |---|---|
  |Server **ONLY**|Server, Client|

  GameState가 GameMode의 하위 클래스라기 보다는 :
  + GameMode : 게임의 전반적 흐름 관리
  + GameState : GameMode에 관련된 정보 관리. Client가 모니터링할 수 있음
 


[문서에서 더 알아보기](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/game-mode-and-game-state-in-unreal-engine)
<br/><br/>

## 2. Gameplay Framework
> 첫 번째 프로젝트에서는 신경 안쓰고 막 작성했다면 지금은 다르다..!

언리얼에서는 클래스마다 역할이 정해져있다. 각각 역할에 맞게 기능이 정의되어 있으므로 이에 따라 함수를 작성/정리하는 것이 바람직.
 
[언리얼 Framework Ref](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/gameplay-framework-quick-reference?application_version=4.27)
<br/><br/>

## 3. 실생활 적용
게임 플레이 시 친구가 이동하는 이유 공부해서 쓰기





