# PlayAround_d
+ 개발 기간 :  2024.04 ~ 진행중
+ 장르 : TPS&FPS / MultiPlayer
+ 서버 타입 : Listen_server
+ 엔진 : UE5.1

## 🚀 Trouble Shooting
## 1. Interface 착오
```c++
UMyCharacterWidgetInterface* CharacterWidget = Cast<UMyCharacterWidgetInterface>(OwningActor);
```
> 위와 같이 작성했을 때 CharacterWidget이 자꾸 null이 됐다..

-> 인터페이스는 "U"MyCharacterWidgetInterface가 아니라 **"I"MyCharacterWidgetInterface**이다!
<br/><br/>

## 2. ReplicatedUsing 나중에 추가 시 오류
Stat의 MaxHp와 CurrentHp를 replicate해야 할 일이 생겼다. 늘 했던 것 처럼 UPROPERTY()에 ```ReplicatedUsing = OnRep_func```을 추가했는데, 두 값을 100으로 초기화를 했음에도 0이 출력됐다.

https://github.com/cubee021/PlayAround_d/blob/2b16d255d7aeeaaae81e40333d35e3bb87eaf7c1/Project2/Character/MyCharacterStatComponent.h#L66-L70

-> Binaries, Intermediate 파일을 지우고 다시 시작하니 해결됐다. 

[도움 된 언리얼 포럼](https://forums.unrealengine.com/t/initializecomponent-not-firing-on-spawn/322782)
<br/><br/>

## 3. Skeletal Mesh Component Replication Failed
[캐릭터가 가지고 있는 아이템을 버리는 기능](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyDropItem.cpp)
을 만들 때 Skeletal mesh가 Replicate되지 않아 Client쪽에서 아무것도 보이지 않았다. 처음에는 Projectile 자체가 생성이 안된줄 알았는데, OnOverlapBegin은 반응해서 Skeletal Mesh에만 문제가 있는 것을 알았다.

-> 찾아보니 Skeletal Mesh Component는 Replicate를 지원하지 않는다고 한다. 보통은 이런 경우 Multicast로 바뀐 Skeletal mesh 정보를 보내고, OnRep으로 각 Client에서 바꾼다고 써있다.

그러나 이 경우에는 Item data에 mesh 정보가 있어서 위와 같이 우회하는 대신, ReplicatedUsing으로 Item Data가 바뀌었음을 알리고 OnRep에서 mesh를 바꾸도록 해봤더니 고쳐졌다. 이 방식이 Multicast->OnRep으로 Skeletal mesh만 보내는 것보다 좋은 방법인지는 잘 모르겠다. Item Data에 들어있는 정보가 더 많아서 비교적 무겁지 않을까..?
<br/><br/>

## 4. C++의 Enum을 블루프린트와 연동하고 싶다면..
UENUM(*Blueprint*) <- 이렇게 괄호 안에 표시👌

<br/><br/>

## 5. Seamless Travel하다 말음


<br/><br/>

## 📖 공부
## 1. RPC 헷갈렸던 내용
#### 1) RPC 함수 내에서 this는 그 함수를 실행시킨 대상

|RPC|실행 대상|this|
|---|---|---|
|ServerRPC|Client|Client|
|ClientRPC|Server|Server|
|MulticastRPC|Server|Server|

#### 2) OnRep은 Server -> Client들로 상태 전송
💡 DOREPLIFETIME으로 지정된 변수의 값이 변경될 때마다 전송한다. (Replicate 불가능한 data type은 제외)

#### 3) Authority는 무조건 Server가 아니다
💡 Authority = Server(리슨 서버는 서버도 참여하므로) & 서버에 있는 Client 본체

[언리얼 Networking overview](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/networking-overview-for-unreal-engine)

#### 4) HasAuthority() vs IsLocallyControlled()
+ HasAuthority() :
+ IsLocallyControlled() :

<br/><br/>

## 2. Gameplay Framework
> 첫 번째 프로젝트에서는 신경 안쓰고 막 작성했다면 지금은 다르다..!

언리얼에서는 클래스마다 역할이 정해져있다. 각각 역할에 맞게 기능이 정의되어 있으므로 이에 따라 함수를 작성/정리하는 것이 바람직하다.
 
[언리얼 Framework Ref](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/gameplay-framework-quick-reference?application_version=4.27)
<br/><br/>

## 3. 실생활 적용
게임 플레이 시 친구가 이동하는 이유 공부해서 쓰기





