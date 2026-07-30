// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DDPlayerController.h"

#include "DDChatInput.h"
#include "DDTimer.h"
#include "DDPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Dedicate.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/DDGameModeBase.h"
#include "Net/UnrealNetwork.h"

ADDPlayerController::ADDPlayerController()
{
	bReplicates = true;
}

void ADDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UDDChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(TimerWidgetClass) == true)
	{
		TimerWidgetInstance = CreateWidget<UDDTimer>(this, TimerWidgetClass);
		if (IsValid(TimerWidgetInstance) == true)
		{
			TimerWidgetInstance->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TimerWidgetClass is None"));
	}
	
	if (IsValid(NotificationTextWidgetClass) == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotificationTextWidgetClass"));
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("NotificationTextWidgetInstance"));
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ADDPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	
	if (IsLocalController() == true)
	{
		// ServerRPCPrintChatMessageString(InChatMessageString);

		ADDPlayerState* DDPS = GetPlayerState<ADDPlayerState>();
		if (IsValid(DDPS) == true)
		{
			FString CombinedMessageString = DDPS->PlyaerNameString + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ADDPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	// UKismetSystemLibrary::PrintString(this, InChatMessageString, true, true, FLinearColor::Red, 5.0f);
	FString NetModeString = DedicateFunctionLibrary::GetNetModeString(this);
	FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	DedicateFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);
}

void ADDPlayerController::SetChatMessagesString(const FString& InChatMessagesString)
{
	ChatMessageString = InChatMessagesString;

	//PrintChatMessageString(InChatMessageString);
	if (IsLocalController() == true)
	{
		ADDPlayerState* DDPS = GetPlayerState<ADDPlayerState>();
		if (IsValid(DDPS) == true)
		{
			FString PlayerInfoString = InChatMessagesString.IsNumeric()
				? DDPS->GetPlayerInfoString()
				: DDPS->PlyaerNameString;
			FString CombinedMessageString = PlayerInfoString + TEXT(": ") + InChatMessagesString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ADDPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ADDPlayerController::ClientRPCPrintGuessLimitExceeded_Implementation()
{
	DedicateFunctionLibrary::MyPrintString(this, TEXT("입력최대횟수초과"), 5.0f, FColor::Red);
}

void ADDPlayerController::ClientRPCPrintInvalidGuess_Implementation()
{
	DedicateFunctionLibrary::MyPrintString(this, TEXT("유효하지않은 정답입니다."), 5.0f, FColor::Red);
}

void ADDPlayerController::ClientRPCPrintNotPlayerTurn_Implementation()
{
	DedicateFunctionLibrary::MyPrintString(this, TEXT("현재 정답 입력 순서가 아닙니다."), 5.0f, FColor::Red);
}

void ADDPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ADDGameModeBase* CXGM = Cast<ADDGameModeBase>(GM);
		if (IsValid(CXGM) == true)
		{
			CXGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void ADDPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}
