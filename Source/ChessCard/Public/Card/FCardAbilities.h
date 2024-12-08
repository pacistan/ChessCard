#pragma once


#include "FCardAbilities.generated.h"

UENUM(BlueprintType)
enum class EAbility : uint8
{
	Summon UMETA(ToolTip = " Summon a unit on the board "),
	addCardToDeck UMETA(ToolTip = " Add a card to the deck "), 
	Stun UMETA(ToolTip = " Stun a unit "), 
	ReplayMove UMETA(ToolTip = " replay the last move if possible "),
	CopyUnit UMETA(ToolTip = " Copy the Unit and transform the caller into the copy (Copy give by Parameters) "),
	DestroyUnit UMETA(ToolTip = " Ability that destroy unit "), 
	Custom UMETA(ToolTip = " Custom card type for special cards if needed (not implemented Yet)", Hidden), 
};

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	Owner, // the Owner of this card
	Enemy, // The enemy of the owner (need to be give by a parameter) 
	AllEnemy, // All the enemy of the owner
	All // all the player
};

UENUM(BlueprintType)
enum class ETargetTiles : uint8
{
	AdjacentTile, // All Adjacent Tile
	UniqueTileGiveByEffect, //  When this Effect is triggered, need to have a tile give by the caller  
};

USTRUCT(BlueprintType)
struct FCardAbilities
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAbility AbilityType = EAbility::Summon;

	/*  the card use by the ability
	 *   summon : the card to summon
	 *   addCardToDeck : the card to add to the deck
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (editcondition = "AbilityType == EAbility::Summon || AbilityType == EAbility::addCardToDeck", EditConditionHides, RowType = "FCardData"))
	FDataTableRowHandle Card;

	/*  The Tile where the ability will be triggered
	 *  Summon : the tile where the unit will be summoned (Random if > 1)
	 *  Stun : the tile where the unit will be stunned
	 *  DestroyUnit : the tile where the unit will be destroyed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (editcondition = "AbilityType == EAbility::Summon || AbilityType == EAbility::Stun || AbilityType == EAbility::DestroyUnit", EditConditionHides))
	ETargetTiles Tiles = ETargetTiles::AdjacentTile;
	
	/*  the player who will be affected by the ability
	 *   addCardToDeck : the player who will receive the card
	 *   Stun : player unit's who can be stunned
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (editcondition = "AbilityType == EAbility::addCardToDeck || AbilityType == EAbility::Stun", EditConditionHides))
	EPlayerType PlayerType = EPlayerType::Owner;
};

