// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DDGameStateBase.generated.h"

class ADDPlayerState;

/**
 * 
 */
UCLASS()
class DEDICATE_API ADDGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<ADDPlayerState> CurrentTurnPlayerState;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingTurnTime = 0;
};
