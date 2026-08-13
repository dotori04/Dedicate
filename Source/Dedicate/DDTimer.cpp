// Fill out your copyright notice in the Description page of Project Settings.


#include "DDTimer.h"

#include "DDGameStateBase.h"

void UDDTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(TimerText) == true)
	{
		TimerText->SetText(GetRemainingTimeText());
	}
}

FText UDDTimer::GetRemainingTimeText() const
{
	const UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return FText::AsNumber(0);
	}

	const ADDGameStateBase* DDGameStateBase = World->GetGameState<ADDGameStateBase>();
	return IsValid(DDGameStateBase)
		? FText::AsNumber(DDGameStateBase->RemainingTurnTime)
		: FText::AsNumber(0);
}

