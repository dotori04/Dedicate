// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/DDPlayerController.h"
#include "DDGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class DEDICATE_API ADDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;	
	
	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	
	virtual void BeginPlay() override;
	
	void PrintChatMessageString(ADDPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ADDPlayerController* InChattingPlayerController);
	
protected:
	FString SecretNumberString;
	
	TArray<TObjectPtr<ADDPlayerController>> AllPlayerControllers; 
	
};
