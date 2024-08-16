# PlayAround_d
+ 개발 기간 :  2024.04 ~ 진행중
+ 장르 : TPS&FPS / MultiPlayer
+ 서버 타입 : Listen_server
+ 엔진 : UE5.1

## 🚀 Trouble Shooting
## 1. RPC 헷갈림

<br/><br/>

## 2. Interface 착오
```c++
UMyCharacterWidgetInterface* CharacterWidget = Cast<UMyCharacterWidgetInterface>(OwningActor);
```
CharacterWidget이 자꾸 null로 설정됐다.

-> 인터페이스는 'U'MyCharacterWidgetInterface가 아니라 **'I'MyCharacterWidgetInterface**이다!
<br/><br/>

## 3. ReplicatedUsing 사용한 변수값 저장 안됨
Stat에서 MaxHp의 UPROPERTY()에 ReplicatedUsing을 끼워넣으면 100으로 설정해도 0이 되어버리는 오류가 생겼다. 이것 때문에 Hp bar에 표시되는 숫자 역시 자꾸 0이 되었다.

https://github.com/cubee021/PlayAround_d/blob/2b16d255d7aeeaaae81e40333d35e3bb87eaf7c1/Project2/Character/MyCharacterStatComponent.h#L66-L70

-> Binaries, Intermediate 파일을 지우고 다시 시작하니 해결됐다. 

[도움 된 언리얼 포럼](https://forums.unrealengine.com/t/initializecomponent-not-firing-on-spawn/322782)
<br/><br/>

## 4. Skeletal Mesh Component Replication Failed
[캐릭터가 가지고 있는 아이템을 버리는 기능](https://github.com/cubee021/PlayAround_d/blob/main/Project2/Item/MyDropItem.cpp)
을 만들 때 Skeletal mesh가 Replicate되지 않아 Client쪽에서 아무것도 보이지 않았다. 처음에는 Projectile 자체가 생성이 안된줄 알았는데, OnOverlapBegin은 반응해서 Skeletal Mesh에만 문제가 있는 것을 알았다.

-> 찾아보니 Skeletal Mesh Component는 Replicate를 지원하지 않는다고 한다. 보통은 이런 경우 Multicast로 바뀐 Skeletal mesh 정보를 보내고, OnRep으로 각 Client에서 바꾼다고 써있다.

그러나 이 경우에는 Item data에 mesh 정보가 있어서 위와 같이 우회하는 대신, ReplicatedUsing으로 Item Data가 바뀌었음을 알리고 OnRep에서 mesh를 바꾸도록 해봤더니 고쳐졌다.

이 방식이 Multicast->OnRep으로 Skeletal mesh만 보내는 것보다 좋은 방법인지는 잘 모르겠다. Item Data에 들어있는 정보가 더 많아서 비교적 무겁지 않을까..?

<br/><br/>

## 5. Seamless Travel하다 말음


<br/><br/>
