// Fill out your copyright notice in the Description page of Project Settings.


#include "DDPawn.h"
#include "Dedicate.h"

void ADDPawn::BeginPlay()
{
	Super::BeginPlay();

	// FString NetRoleString = DedicateFunctionLibrary::GetRoleString(this);
	// FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *DedicateFunctionLibrary::GetNetModeString(this), *NetRoleString);
	// DedicateFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ADDPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// FString NetRoleString = DedicateFunctionLibrary::GetRoleString(this);
	// FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *DedicateFunctionLibrary::GetNetModeString(this), *NetRoleString);
	// DedicateFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}


