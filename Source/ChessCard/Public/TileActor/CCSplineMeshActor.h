// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SplineMeshActor.h"
#include "CCSplineMeshActor.generated.h"

class USplineComponent;

UCLASS()
class CHESSCARD_API ACCSplineMeshActor : public AActor
{
	GENERATED_BODY()

public:
	ACCSplineMeshActor();

	UPROPERTY(VisibleAnywhere)
	USplineComponent* SplineComponent;
};
