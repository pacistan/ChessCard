// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CCButtonBase.h"

void UCCButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateButtonStyle();
	UpdateButtonText(ButtonText);
}

void UCCButtonBase::SetButtonText(const FText& InText)
{
	bOverride_ButtonText = InText.IsEmpty();
	ButtonText = InText;
	UpdateButtonText(InText);
}
