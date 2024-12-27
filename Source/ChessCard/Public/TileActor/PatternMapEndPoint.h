#pragma once

#include "PatternMapEndPoint.generated.h"

enum class EMovementType : uint8;

USTRUCT()
struct FPatternMapEndPoint
{
	GENERATED_BODY()

public:

	UPROPERTY()
	EMovementType MovementType;

	UPROPERTY()
	FIntPoint Direction;
};