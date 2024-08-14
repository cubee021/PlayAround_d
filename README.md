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

## 4. Projectile not replicating


<br/><br/>

## 5. Seamless Travel하다 말음


<br/><br/>
