
#include "DDPlayerState.h"

#include "Net/UnrealNetwork.h"

ADDPlayerState::ADDPlayerState()
	: PlyaerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void ADDPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlyaerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ADDPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlyaerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount+1) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}