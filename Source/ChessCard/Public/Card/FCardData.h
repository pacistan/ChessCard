#pragma once
#include "GameplayTagContainer.h"
#include "FCardData.generated.h"

UENUM()
enum class ECardType : uint8
{
	Unit,
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
	TSoftObjectPtr<UTexture2D> CardTexture;

	// On Death 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnDeathEffect = false;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnDeathEffect"))

	// On Spawn 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnSpawnEffect = false;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnSpawnEffect"))
	
	// On Kill 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnKillEffect = false;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnKillEffect"))
	
	// On Move (end of deplacement)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (InlineEditConditionToggle))
	bool bHasOnMoveEffect = false;

	/* the Effect is Trigger at End of Movement */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "bHasOnMoveEffect"))
	
	/* Tags the Unit need To have at Start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer Tags;
};
 