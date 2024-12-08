#pragma once
#include "GameplayTagContainer.h"
#include "FCardData.generated.h"

struct FCardAbilities;

UENUM()
enum class ECardType : uint8
{
	Unit UMETA(ToolTip = "Have a movement pattern and can be placed on the board"),
	Movement UMETA(ToolTip = "Card that can be used to move a unit"),
	SpecificUnit UMETA(ToolTip = "Card that can be used on a specific unit"),
	Ephemere UMETA(ToolTip = "Card That is Destroy when Draw"),
	Custom UMETA(ToolTip = "Custom Card Type for special cards if needed (not implemented Yet)", hidden) 
};

/* Struct that Handle of the Data relative to the patern of movement */
USTRUCT(BlueprintType)
struct FMovementPattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FVector2D> Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TSoftObjectPtr<UTexture2D> PatternTexture;

	FMovementPattern()
	: Pattern(TArray<FVector2d>())
	, PatternTexture(nullptr)
	{
	}
};

USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	ECardType CardType = ECardType::Unit;

	/* if the card is a specific unit, this is the name of the unit who can use this card
	 *  If the card choose is not a unit, The card will not be able to be used 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card", meta = (editcondition = "CardType == ECardType::SpecificUnit", RowType = "FCardData", EditConditionHides))
	FDataTableRowHandle UnitWhoCanUseThisCard;

	/* juste the name for the Ui, the actual name if the name of the Raw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FString CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FString CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UTexture2D> CardTexture;

	// On Death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "CardType = ECardType::Unit || CardType == ECardType::SpecificUnit", TitleProperty = "AbilityType", EditConditionHides))
	TArray<FCardAbilities> OnDeathEffect;

	// On Kill 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (editcondition = "CardType = ECardType::Unit || CardType == ECardType::SpecificUnit", TitleProperty = "AbilityType", EditConditionHides))
	TArray<FCardAbilities> OnKillEffect;

	// On Move (end of deplacement)
	/* the Effect is Trigger at End of Movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects",meta = (editcondition = "CardType = ECardType::Unit || CardType == ECardType::SpecificUnit", TitleProperty = "AbilityType", EditConditionHides))
	TArray<FCardAbilities> OnMoveEffect;

	/* Tags the Unit need To have at Start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer Tags;
};
