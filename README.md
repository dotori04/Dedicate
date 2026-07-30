# Dedicate 구현 과정

## PlayerController 생성자 수정

- `DDPlayerController.h`에 `ADDPlayerController()` 생성자를 선언했다.
- `DDPlayerController.cpp`의 잘못된 생성자 이름을 `ADDPlayerController::ADDPlayerController()`로 수정했다.
- 생성자에서 `bReplicates`를 활성화했다.
- `NotificationText` 복제를 위해 `GetLifetimeReplicatedProps`를 선언하고 등록했다.

## 정답 입력 횟수 제한

- 서버의 `ADDGameModeBase::PrintChatMessageString`에서 정답 처리 전에 입력 횟수를 검사한다.
- `CurrentGuessCount`가 `MaxGuessCount` 이상이면 정답 처리를 중단한다.
- 제한을 초과한 클라이언트에만 Client RPC를 호출한다.
- 해당 클라이언트 화면에 `입력최대횟수초과` 문구를 빨간색으로 5초간 출력한다.
- 승리 또는 무승부 판정 후 `ResetGame`이 실행되면 모든 플레이어의 `CurrentGuessCount`를 0으로 초기화한다.

## 정답 유효성 검사

- 채팅 문자열에서 `:` 뒤의 실제 입력값만 분리한다.
- 숫자 입력은 정확히 3자리인지 검사한다.
- `TSet`을 사용하여 중복 숫자가 있는지 검사한다.
- 기존 규칙에 따라 `0`이 포함된 입력도 허용하지 않는다.
- 유효하지 않은 숫자 입력이면 해당 클라이언트에만 `유효하지않은 정답입니다.` 문구를 빨간색으로 5초간 출력한다.
- 유효성 검사 실패 시 즉시 반환하므로 `CurrentGuessCount`는 증가하지 않는다.

## 일반 채팅과 정답 입력 분리

- 일반 채팅은 `PlayerName: 메시지` 형식으로 출력한다.
- 숫자 정답 입력은 `PlayerName(현재횟수/최대횟수): 숫자` 형식으로 출력한다.
- 일반 채팅에는 정답 입력 횟수를 표시하지 않는다.

## 처리 흐름

1. 클라이언트가 메시지를 입력한다.
2. 숫자 입력이면 정답 입력 형식으로 서버에 전달한다.
3. 서버에서 입력 횟수와 정답 유효성을 검사한다.
4. 검사를 통과한 경우에만 판정하고 `CurrentGuessCount`를 증가시킨다.
5. 일반 채팅은 횟수 검사와 정답 판정 없이 모든 클라이언트에 전달한다.
