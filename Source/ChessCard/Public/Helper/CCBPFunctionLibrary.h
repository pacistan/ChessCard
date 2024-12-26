// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CHESSCARD_API UCCBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void AddWidgetToViewport(UGameInstance* GameInstance, UUserWidget* Widget, int32 ZOrder);

	UFUNCTION(BlueprintCallable)
	static void RemoveWidgetFromViewport(UGameInstance* GameInstance, UUserWidget* Widget);
	
};
