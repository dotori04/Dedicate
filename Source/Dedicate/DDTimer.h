// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "DDTimer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATE_API UDDTimer : public UUserWidget
{
	GENERATED_BODY()
	
	
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure)
	FText GetRemainingTimeText() const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
};
