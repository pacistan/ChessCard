#pragma once

#include "GameplayTagContainer.h"
#include "Card/FUnitMovementData.h"
#include "FCardData.generated.h"

enum class EEffectType : uint8;
struct FCardAbilities;

UENUM(BlueprintType)
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
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TArray<TSoftObjectPtr<UMaterialInterface>> CardMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UStaticMesh> CardUnitStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UStaticMesh> UnitStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FVector Rotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FVector PositionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	FVector ScaleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TSoftObjectPtr<UTexture2D> InfosCardTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIData")
	TArray<TSoftObjectPtr<UTexture2D>> AllEffectInfosForCardTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EEffectType EffectType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool CanScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool CanBeExchangedForMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool IsRandomSpawnable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int DivineAngerTriggerNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FUnitMovementData> Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FUnitMovementData> DivineAngerPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TSoftObjectPtr<UTexture2D> PatternTexture;

};
