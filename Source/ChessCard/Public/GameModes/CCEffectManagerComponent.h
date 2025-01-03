#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid/CCTile.h"
#include "CCEffectManagerComponent.generated.h"

struct FPlayerActionData;
class ACCPlayerPawn;

UENUM()
enum class EEffectTriggerType : uint8{OnKill, OnDeath, OnMove};

UENUM()
enum class EEffectType : uint8
{
	Midas UMETA(DisplayName = "Midas"),
	Promethee UMETA(DisplayName = "Promethee"),
	Minotaur UMETA(DisplayName = "Minotaur"),
	Trojan UMETA(DisplayName = "Trojan"),
	Meduse UMETA(DisplayName = "Meduse"),
	Persee UMETA(DisplayName = "Persee"),
	Circe UMETA(DisplayName = "Circe"),
	Heracles UMETA(DisplayName = "Heracles"),
	Embrasement UMETA(DisplayName = "Embrasement"),
	Gold UMETA(DisplayName = "Gold")
};

class ACCPieceBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHESSCARD_API UCCEffectManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="CC|EffectData")
	FDataTableRowHandle EmbrasementRowHandle;

	UPROPERTY(EditDefaultsOnly, Category="CC|EffectData")
	FDataTableRowHandle GoldRowHandle;

	UPROPERTY(EditDefaultsOnly, Category="CC|EffectData")
	TObjectPtr<UDataTable> CardDataTable;
	
public:
	UCCEffectManagerComponent();

	UFUNCTION()
	void TriggerResolveEffect(bool IsDivineAnger, FDataTableRowHandle InRowHandle, ACCPieceBase* InPiece, TArray<ACCTile*> InEffectTiles, EEffectTriggerType
	                          InTriggerType, TArray<ACCPieceBase*> RelevantUnits, FIntPoint Direction, FPlayerActionData ActionData);
};
