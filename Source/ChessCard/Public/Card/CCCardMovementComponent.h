#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/CCPlayerPawn.h"
#include "CCCardMovementComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnCardMovementEnd);

enum class ECardState : uint8;
class ACCCard;

USTRUCT()
struct CHESSCARD_API FMovementData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	float Interpolator;

	UPROPERTY()
	FVector StartPosition;

	UPROPERTY()
	FVector EndPosition;

	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	FRotator EndRotation;

	UPROPERTY()
	FVector StartScale;

	UPROPERTY()
	FVector EndScale;

	UPROPERTY()
	float Duration;

	UPROPERTY()
	FOnCardMovementEnd OnDrawCardEnd;
};

USTRUCT(BlueprintType)
struct CHESSCARD_API FExposedMovementData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	float Duration;

	UPROPERTY(EditDefaultsOnly)
	float HeightOffset;

	UPROPERTY(EditDefaultsOnly)
	int LayerNumber;

	UPROPERTY(EditDefaultsOnly)
	float ScaleCoefficient;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> AnimCurve;

};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories
	(Variable, "Sockets", Tags, Component_Tick, Component_Replication, Activation, Cooking, Events, Asset_User_Data, Replication, Navigation))
class CHESSCARD_API UCCCardMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCCardMovementComponent();
	
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere)
	TMap<ECardState, FExposedMovementData> ExposedDataMap;

public:
	UPROPERTY(EditDefaultsOnly)
	float DistanceFromCamera;
	
	UPROPERTY(EditDefaultsOnly)
	float CardXPositionOffSet;

	UPROPERTY(EditDefaultsOnly)
	float CardHeightPositionOffset;
	
	UPROPERTY(EditDefaultsOnly)
	FVector2D HandPositionOffset;

	UPROPERTY(EditDefaultsOnly)
	float CardTiltOffset;
	
	UPROPERTY()
	FVector BaseScale;
	
	UPROPERTY()
	TObjectPtr<ACCCard> Owner;

	UPROPERTY()
	FMovementData MovementData;

	UPROPERTY()
	float CurrentDuration;

	UPROPERTY()
	bool IsMoving = false;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION()
	bool StartMovement(int InCardIndex, int InHandNumber, FOnCardMovementEnd OnCardMovementEnd = FOnCardMovementEnd(), bool IsCustomDuration = false, float CustomDuration = 0);

	UFUNCTION()
	void MovementTick(float DeltaTime);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
