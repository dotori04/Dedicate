// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DEDICATE_API ADDPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ADDPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;	
	UPROPERTY(Replicated)
	FString PlyaerNameString;
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
	UPROPERTY(Replicated)
	int32 MaxGuessCount;
	FString GetPlayerInfoString();
	
};
