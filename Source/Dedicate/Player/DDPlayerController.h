// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DDPlayerController.generated.h"

class UDDChatInput;

/**
 * 
 */
UCLASS()
class DEDICATE_API ADDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDDChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UDDChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;
};
