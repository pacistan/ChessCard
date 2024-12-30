// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCBaseState.h"
#include "CCStateResolve.generated.h"

/**
 * 
 */
UCLASS()
class CHESSCARD_API UCCStateResolve : public UCCBaseState
{
	GENERATED_BODY()

public:
	void OnEnterState() override;
};
