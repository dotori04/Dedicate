// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DDPlayerController.h"

#include "DDChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Dedicate.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/DDGameModeBase.h"

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
}

void ADDPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	PrintChatMessageString(ChatMessageString);
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
		ServerRPCPrintChatMessageString(InChatMessagesString);		
	}
}

void ADDPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
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