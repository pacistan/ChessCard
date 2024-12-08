#pragma once

#include "FUnitMovementData.generated.h"

UENUM()
enum class EMovementDirection : uint8
{
	Up,
	Side,
	Diagonal,
	Up_Side,
	Up_Diagonal,
	Up_Side_Diagonal,
	Side_Diagonal
};

UENUM()
enum class EMovementType : uint8
{
	Normal,
	Stoppable,
	ApplyEffect
};

USTRUCT(BlueprintType)
struct FUnitMovementData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementDirection MovementDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementType MovementType;
};
