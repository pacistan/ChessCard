#pragma once
#include "GameplayTagContainer.h"
#include "FCardData.generated.h"

struct FUnitMovementData;
struct FCardAbilities;

UENUM()
enum class ECardType : uint8
{
	Unit,
	Movement,
	SpecificUnit,
	Custom // Custom card type for special cards if needed /* not implemented Yet */
};


USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	ECardType CardType = ECardType::Unit;

	/* if the card is a specific unit, this is the name of the unit who can use this card */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card", meta = (editcondition = "CardType == ECardType::SpecificUnit"))
	FString NameOfUnitWhoCanUseThisCard;

	/* juste the name for the Ui, the actual name if the name of the Raw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FString CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FString CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UMaterialInterface> CardMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UStaticMesh> CardUnitStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UStaticMesh> UnitStaticMesh;
	
	// On Death 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnDeathEffect = true;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnDeathEffect", TitleProperty = "AbilityType"))
	//TArray<FCardAbilities> OnDeathEffect;
	
	// On Spawn 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnSpawnEffect = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnSpawnEffect", TitleProperty = "AbilityType"))
	//TArray<FCardAbilities> OnSpawnEffect;
	
	// On Kill 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnKillEffect = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnKillEffect", TitleProperty = "AbilityType"))
	//TArray<FCardAbilities> OnKillEffect;
	
	// On Move (end of deplacement)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnMoveEffect = false;

	/* the Effect is Trigger at End of Movement */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnMoveEffect", TitleProperty = "AbilityType"))
	//TArray<FCardAbilities> OnMoveEffect;
	
	/* Tags the Unit need To have at Start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FUnitMovementData> Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TSoftObjectPtr<UTexture2D> PatternTexture;
};
