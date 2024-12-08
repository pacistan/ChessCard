#pragma once


#include "FCardAbilities.generated.h"

UENUM(BlueprintType)
enum class EAbility : uint8
{
	Summon,
	Custom,
};
USTRUCT(BlueprintType)
struct FCardAbilities
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	EAbility AbilityType = EAbility::Summon;

	// Each Ability effect here 
};

