// Fill out your copyright notice in the Description page of Project Settings.


#include "DDChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/DDPlayerController.h"

void UDDChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UDDChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UDDChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ADDPlayerController* OwningPlayerController = Cast<ADDPlayerController>(GetOwningPlayer());
		if (IsValid(OwningPlayerController) == true)
		{
			OwningPlayerController->SetChatMessagesString(Text.ToString());
			EditableTextBox_ChatInput->SetText(FText());
		}
	}
}