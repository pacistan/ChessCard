// Fill out your copyright notice in the Description page of Project Settings.


#include "Helper/CCBPFunctionLibrary.h"

#include "Blueprint/UserWidget.h"

void UCCBPFunctionLibrary::AddWidgetToViewport(UGameInstance* GameInstance, UUserWidget* Widget, int32 ZOrder)
{
	UGameViewportClient* client = GameInstance->GetGameViewportClient();

	client->AddViewportWidgetContent(Widget->TakeWidget(), ZOrder);
}

void UCCBPFunctionLibrary::RemoveWidgetFromViewport(UGameInstance* GameInstance, UUserWidget* Widget)
{
	UGameViewportClient* client = GameInstance->GetGameViewportClient();

	client->RemoveViewportWidgetContent(Widget->TakeWidget());
}
