// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DDGameModeBase.h"
#include "DDGameStateBase.h"
#include "Player/DDPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Player/DDPlayerState.h"
#include "TimerManager.h"

void ADDGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ADDPlayerController* DDPlayerController = Cast<ADDPlayerController>(NewPlayer);
	if (IsValid(DDPlayerController) == true)
	{
		DDPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(DDPlayerController);

		ADDPlayerState* DDPS = DDPlayerController->GetPlayerState<ADDPlayerState>();
		if (IsValid(DDPS) == true)
		{
			DDPS->PlyaerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ADDGameStateBase* DDGameStateBase =  GetGameState<ADDGameStateBase>();
		if (IsValid(DDGameStateBase) == true)
		{
			DDGameStateBase->MulticastRPCBroadcastLoginMessage(DDPS->PlyaerNameString);
		}

		if (CurrentTurnIndex == INDEX_NONE)
		{
			CurrentTurnIndex = 0;
			ResetTurnTimer();
		}

		UpdateTurnPlayerState();
	}

}

void ADDGameModeBase::Logout(AController* Exiting)
{
	ADDPlayerController* ExitingPlayerController = Cast<ADDPlayerController>(Exiting);
	int32 ExitingPlayerIndex = AllPlayerControllers.IndexOfByKey(ExitingPlayerController);
	if (ExitingPlayerIndex != INDEX_NONE)
	{
		bool IsCurrentTurnPlayer = ExitingPlayerIndex == CurrentTurnIndex;
		AllPlayerControllers.RemoveAt(ExitingPlayerIndex);

		if (AllPlayerControllers.IsEmpty() == true)
		{
			CurrentTurnIndex = INDEX_NONE;
			GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		}
		else if (IsCurrentTurnPlayer == true)
		{
			CurrentTurnIndex %= AllPlayerControllers.Num();
			ResetTurnTimer();
		}
		else if (ExitingPlayerIndex < CurrentTurnIndex)
		{
			CurrentTurnIndex--;
		}

		UpdateTurnPlayerState();
	}

	Super::Logout(Exiting);
}

FString ADDGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });
	
	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ADDGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0' || UniqueDigits.Contains(C) == true)
			{
				bIsUnique = false;
				break;
			}
			
			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
		
	} while (false);	

	return bCanPlay;
}

FString ADDGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else 
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;				
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ADDGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void ADDGameModeBase::PrintChatMessageString(ADDPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int32 SeparatorIndex = INDEX_NONE;
	InChatMessageString.FindLastChar(TEXT(':'), SeparatorIndex);
	FString GuessNumberString = InChatMessageString.Mid(SeparatorIndex + 1).TrimStartAndEnd();

	if (GuessNumberString.IsNumeric() == true && IsGuessNumberString(GuessNumberString) == false)
	{
		InChattingPlayerController->ClientRPCPrintInvalidGuess();
		return;
	}

	if (IsGuessNumberString(GuessNumberString) == true)
	{
		ADDPlayerState* DDPlayerState = InChattingPlayerController->GetPlayerState<ADDPlayerState>();
		if (IsValid(DDPlayerState) == true && DDPlayerState->CurrentGuessCount >= DDPlayerState->MaxGuessCount)
		{
			InChattingPlayerController->ClientRPCPrintGuessLimitExceeded();
			return;
		}

		const ADDGameStateBase* DDGameStateBase = GetGameState<ADDGameStateBase>();
		if (IsValid(DDGameStateBase) == true && DDGameStateBase->RemainingTurnTime <= 0)
		{
			return;
		}

		if (IsPlayerTurn(InChattingPlayerController) == false)
		{
			InChattingPlayerController->ClientRPCPrintNotPlayerTurn();
			return;
		}

		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);
		for (TActorIterator<ADDPlayerController> It(GetWorld()); It; ++It)
		{
			ADDPlayerController* DDPlayerController = *It;
			if (IsValid(DDPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				DDPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
		AdvanceTurn();
	}
	else
	{
		for (TActorIterator<ADDPlayerController> It(GetWorld()); It; ++It)
		{
			ADDPlayerController* DDPlayerController = *It;
			if (IsValid(DDPlayerController) == true)
			{
				DDPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ADDGameModeBase::IncreaseGuessCount(ADDPlayerController* InChattingPlayerController)
{
	ADDPlayerState* DDPS = InChattingPlayerController->GetPlayerState<ADDPlayerState>();
	if (IsValid(DDPS) == true)
	{
		DDPS->CurrentGuessCount++;
	}
}

void ADDGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& DDPlayerController : AllPlayerControllers)
	{
		ADDPlayerState* DDPS = DDPlayerController->GetPlayerState<ADDPlayerState>();
		if (IsValid(DDPS) == true)
		{
			DDPS->CurrentGuessCount = 0;
			DDPlayerController->NotificationText = FText::FromString(TEXT("GameStarted"));
		}
	}
}

void ADDGameModeBase::JudgeGame(ADDPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ADDPlayerState* DDPS = InChattingPlayerController->GetPlayerState<ADDPlayerState>();
		for (const auto& DDPlayerController : AllPlayerControllers)
		{
			if (IsValid(DDPS) == true)
			{
				FString CombinedMessageString = DDPS->PlyaerNameString + TEXT(" has won the game.");
				DDPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}

		ResetGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& DDPlayerController : AllPlayerControllers)
		{
			ADDPlayerState* DDPS = DDPlayerController->GetPlayerState<ADDPlayerState>();
			if (IsValid(DDPS) == true)
			{
				if (DDPS->CurrentGuessCount < DDPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& DDPlayerController : AllPlayerControllers)
			{
				DDPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}

			ResetGame();
		}
	}
}

bool ADDGameModeBase::IsPlayerTurn(const ADDPlayerController* InPlayerController) const
{
	return AllPlayerControllers.IsValidIndex(CurrentTurnIndex)
		&& AllPlayerControllers[CurrentTurnIndex] == InPlayerController;
}

void ADDGameModeBase::AdvanceTurn()
{
	if (AllPlayerControllers.IsEmpty() == true)
	{
		CurrentTurnIndex = INDEX_NONE;
		UpdateTurnPlayerState();
		return;
	}

	for (int32 Offset = 1; Offset <= AllPlayerControllers.Num(); ++Offset)
	{
		int32 NextTurnIndex = (CurrentTurnIndex + Offset) % AllPlayerControllers.Num();
		ADDPlayerController* PlayerController = AllPlayerControllers[NextTurnIndex];
		ADDPlayerState* PlayerState = PlayerController->GetPlayerState<ADDPlayerState>();
		if (IsValid(PlayerState) == true && PlayerState->CurrentGuessCount < PlayerState->MaxGuessCount)
		{
			CurrentTurnIndex = NextTurnIndex;
			break;
		}
	}

	UpdateTurnPlayerState();
	ResetTurnTimer();
}

void ADDGameModeBase::UpdateTurnPlayerState()
{
	ADDGameStateBase* DDGameStateBase = GetGameState<ADDGameStateBase>();
	if (IsValid(DDGameStateBase) == false)
	{
		return;
	}

	DDGameStateBase->CurrentTurnPlayerState = AllPlayerControllers.IsValidIndex(CurrentTurnIndex)
		? AllPlayerControllers[CurrentTurnIndex]->GetPlayerState<ADDPlayerState>()
		: nullptr;

	if (IsValid(DDGameStateBase->CurrentTurnPlayerState) == false)
	{
		DDGameStateBase->RemainingTurnTime = 0;
		return;
	}

	for (const auto& DDPlayerController : AllPlayerControllers)
	{
		if (IsValid(DDPlayerController) == true)
		{
			const ADDPlayerState* DDPlayerState = DDPlayerController->GetPlayerState<ADDPlayerState>();
			if (IsValid(DDPlayerState) == true)
			{
				const FString TurnNotificationString = FString::Printf(
					TEXT("%s\n%s 차례입니다."),
					*DDPlayerState->PlyaerNameString,
					*DDGameStateBase->CurrentTurnPlayerState->PlyaerNameString);
				DDPlayerController->NotificationText = FText::FromString(TurnNotificationString);
			}
		}
	}
}

void ADDGameModeBase::ResetTurnTimer()
{
	ADDGameStateBase* DDGameStateBase = GetGameState<ADDGameStateBase>();
	if (IsValid(DDGameStateBase) == false)
	{
		return;
	}

	DDGameStateBase->RemainingTurnTime = TurnDuration;
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ThisClass::UpdateTurnTimer, 1.0f, true);
}

void ADDGameModeBase::UpdateTurnTimer()
{
	ADDGameStateBase* DDGameStateBase = GetGameState<ADDGameStateBase>();
	if (IsValid(DDGameStateBase) == false)
	{
		return;
	}

	DDGameStateBase->RemainingTurnTime = FMath::Max(0, DDGameStateBase->RemainingTurnTime - 1);
	if (DDGameStateBase->RemainingTurnTime == 0)
	{
		if (AllPlayerControllers.IsValidIndex(CurrentTurnIndex) == true)
		{
			ADDPlayerController* CurrentPlayerController = AllPlayerControllers[CurrentTurnIndex];
			IncreaseGuessCount(CurrentPlayerController);
			JudgeGame(CurrentPlayerController, 0);
		}

		AdvanceTurn();
	}
}
