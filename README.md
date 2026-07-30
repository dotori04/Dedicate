# Dedicate 구현 과정

## PlayerController 생성자 및 복제 설정

- `DDPlayerController.h`에 `ADDPlayerController()` 생성자를 선언했다.
- `DDPlayerController.cpp`의 생성자 이름을 `ADDPlayerController::ADDPlayerController()`로 수정했다.
- 생성자에서 `bReplicates`를 활성화했다.
- `NotificationText` 복제를 위해 `GetLifetimeReplicatedProps`를 선언하고 등록했다.

## 일반 채팅과 정답 입력 분리

- 일반 채팅은 `PlayerName: 메시지` 형식으로 출력한다.
- 숫자 정답은 `PlayerName(현재횟수/최대횟수): 숫자` 형식으로 출력한다.
- 일반 채팅에는 정답 입력 횟수를 표시하지 않는다.
- 일반 채팅은 턴과 남은 시간에 관계없이 항상 사용할 수 있다.

## 정답 유효성 검사

- 채팅 문자열에서 `:` 뒤의 실제 입력값을 분리한다.
- 숫자 입력이 정확히 3자리인지 검사한다.
- `TSet`을 사용하여 중복 숫자가 있는지 검사한다.
- 기존 규칙에 따라 `0`이 포함된 입력은 허용하지 않는다.
- 유효하지 않은 숫자이면 해당 클라이언트에만 `유효하지않은 정답입니다.`를 빨간색으로 5초간 출력한다.
- 유효성 검사에 실패하면 즉시 반환하므로 `CurrentGuessCount`는 증가하지 않는다.

## 정답 입력 횟수 제한

- 서버의 `ADDGameModeBase::PrintChatMessageString`에서 정답 처리 전에 입력 횟수를 검사한다.
- `CurrentGuessCount`가 `MaxGuessCount` 이상이면 정답 처리를 중단한다.
- 제한을 초과한 클라이언트에만 Client RPC를 호출한다.
- 해당 클라이언트 화면에 `입력최대횟수초과`를 빨간색으로 5초간 출력한다.
- 승리 또는 무승부 판정 후 `ResetGame`에서 모든 플레이어의 `CurrentGuessCount`를 0으로 초기화한다.

## 턴 시스템

- 서버의 `ADDGameModeBase`에서 `CurrentTurnIndex`를 관리한다.
- 플레이어가 서버에 접속한 순서대로 정답 입력 턴을 진행한다.
- 유효한 정답 입력을 처리한 후 다음 플레이어에게 턴을 넘긴다.
- 최대 시도 횟수에 도달한 플레이어는 다음 턴 선택에서 제외한다.
- 자신의 턴이 아닌 플레이어가 정답을 입력하면 처리를 중단한다.
- 해당 클라이언트에 `현재 정답 입력 순서가 아닙니다.`를 빨간색으로 출력한다.
- 플레이어가 퇴장하면 배열과 현재 턴 인덱스를 보정한다.
- `ADDGameStateBase::CurrentTurnPlayerState`를 복제하여 모든 클라이언트가 동일한 현재 턴 정보를 사용한다.
- 턴이 변경되면 모든 플레이어의 `NotificationText`에 `PlayerN 차례입니다.`를 설정한다.

## 서버 타이머

- 서버의 `ADDGameModeBase`에서 `FTimerHandle`을 사용하여 턴 제한 시간을 관리한다.
- 기본 턴 제한 시간은 30초이며 `TurnDuration`으로 설정할 수 있다.
- 턴이 시작되거나 변경되면 남은 시간을 `TurnDuration`으로 초기화한다.
- 서버에서 1초마다 남은 시간을 감소시킨다.
- `ADDGameStateBase::RemainingTurnTime`을 복제하여 모든 클라이언트의 시간을 동기화한다.
- 접속 중인 플레이어가 없으면 타이머를 정지하고 남은 시간을 0으로 설정한다.

## 시간 초과 처리

- 남은 시간이 0초가 되면 현재 플레이어의 숫자 정답 입력을 차단한다.
- 제한 시간 동안 정답 입력이 없으면 현재 플레이어의 `CurrentGuessCount`를 1 증가시킨다.
- 횟수 증가 후 무승부 여부를 판정한다.
- 판정이 끝나면 접속 순서에 따라 다음 입력 가능한 플레이어에게 턴을 넘긴다.
- 다음 턴이 시작되면 타이머를 다시 초기화한다.
- 일반 채팅은 시간 초과 이후에도 사용할 수 있다.

## 타이머 위젯

- `UDDTimer`를 상속하는 `WBP_Timer` 위젯을 사용한다.
- `BP_PlayerController`의 `TimerWidgetClass`에 `WBP_Timer`를 지정한다.
- `TimerWidgetClass`는 `EditDefaultsOnly`, `BlueprintReadOnly`로 Blueprint의 `UI` 카테고리에 노출한다.
- `UDDTimer::GetRemainingTimeText`에서 복제된 `RemainingTurnTime`을 `FText`로 반환한다.
- `NativeTick`에서 `TimerText`에 현재 남은 시간을 반영한다.
- 일반 Text 위젯은 `TObjectPtr<UTextBlock>`으로 바인딩하며 위젯 이름을 `TimerText`로 설정한다.

## 전체 처리 흐름

1. 플레이어가 서버에 접속하면 접속 순서대로 플레이어 목록에 추가된다.
2. 첫 번째 플레이어에게 턴을 지정하고 서버 타이머를 시작한다.
3. 일반 채팅은 별도 제한 없이 모든 클라이언트에 전달한다.
4. 숫자 입력이면 서버에서 유효성, 최대 시도 횟수, 남은 시간, 현재 턴을 검사한다.
5. 모든 검사를 통과한 정답만 판정하고 `CurrentGuessCount`를 증가시킨다.
6. 정답 처리가 끝나면 다음 입력 가능한 플레이어에게 턴을 넘긴다.
7. 제한 시간 안에 입력하지 않으면 시도 횟수를 증가시키고 다음 턴으로 이동한다.
8. 승리 또는 무승부가 결정되면 게임 상태와 모든 플레이어의 시도 횟수를 초기화한다.
