#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid/CCTile.h"
#include "CCEffectManagerComponent.generated.h"

UENUM()
enum class EEffectTriggerType : uint8{OnKill, OnDeath, OnMove, DivineAnger};

UENUM()
enum class EEffectType : uint8{Midas, Promethee, Minotaur, Trojan, Meduse, Persee, Circe, Heracles, Embrasement};

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
	void TriggerResolveEffect(FDataTableRowHandle InRowHandle, ACCPieceBase* InPiece, ACCTile* InEffectTiles, EEffectTriggerType
	                          InTriggerType, ACCPieceBase* RelevantUnit, FIntPoint Direction);
};
