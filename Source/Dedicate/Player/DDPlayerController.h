// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DDPlayerController.generated.h"

class UDDChatInput;

UCLASS()
class DEDICATE_API ADDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADDPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);
	void SetChatMessagesString(const FString& InChatMessagesString);
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintGuessLimitExceeded();
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintInvalidGuess();
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDDChatInput> ChatInputWidgetClass;
	UPROPERTY()
	TObjectPtr<UDDChatInput> ChatInputWidgetInstance;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;
	FString ChatMessageString;
};
