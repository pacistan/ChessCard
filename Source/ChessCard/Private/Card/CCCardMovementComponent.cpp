#include "Card/CCCardMovementComponent.h"

#include "Card/CCCard.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CCPlayerPawn.h"


UCCCardMovementComponent::UCCCardMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Owner = Cast<ACCCard>(GetOwner());
}

void UCCCardMovementComponent::StartMovement(int InCardIndex, int InHandNumber, FOnEndCardDraw& OnDrawCardEnd)
{
	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(!GetWorld() || !CameraManager)
	{
		UE_LOG(LogTemp, Error, TEXT("No World or no Camera Manager"));
		return;
	}

	PrimaryComponentTick.SetTickFunctionEnable(true);

	MovementData.OnDrawCardEnd = OnDrawCardEnd;
	MovementData.StartPosition = GetOwner()->GetActorLocation();
	MovementData.StartRotation = GetOwner()->GetActorRotation();
	MovementData.StartScale = GetOwner()->GetActorScale();
	
	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRightVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
	const FVector CameraUpVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Z);

	auto State = Owner->GetCurrentCardState();
	
	float HoveredPositionOffset = ExposedDataMap[State].HeightOffset;
	int32 HoveredLayerNumber = ExposedDataMap[State].LayerNumber;
	MovementData.EndScale = BaseScale * ExposedDataMap[State].ScaleCoefficient;

	MovementData.EndPosition = FVector(CameraLocation + (CameraRotation.Vector() * (DistanceFromCamera - HoveredLayerNumber))
	+ CameraRightVector * HandPositionOffset.X
	+ CameraRightVector * CardXPositionOffSet * InCardIndex
	- CameraRightVector * CardXPositionOffSet * (InHandNumber - 1) / 2.0f
	+ CameraUpVector * HandPositionOffset.Y
	- CameraUpVector * CardHeightPositionOffset * FMath::Abs(InCardIndex - (InHandNumber - 1) / 2.0f)
	- CameraUpVector * CardHeightPositionOffset
	+ CameraUpVector * HoveredPositionOffset);

	MovementData.EndRotation = MovementData.StartRotation;
	MovementData.EndRotation.Roll = FMath::Lerp(
		CardTiltOffset * static_cast<float>(InHandNumber - 1) / 2,
		-CardTiltOffset * static_cast<float>(InHandNumber - 1) / 2,
		InCardIndex / static_cast<float>(InHandNumber));
}

void UCCCardMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MovementTick(DeltaTime);
}

void UCCCardMovementComponent::MovementTick(float DeltaTime)
{
	if(!GetWorld())
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
		return;
	}

	ECardState State =  Owner->GetCurrentCardState();
	MovementData.Interpolator = FMath::Clamp(MovementData.Interpolator +  DeltaTime / ExposedDataMap[State].Duration, 0, 1);
	const FVector newPosition = FMath::Lerp(MovementData.StartPosition, MovementData.EndPosition, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	const FRotator newRotation = FMath::Lerp(MovementData.StartRotation, MovementData.EndRotation, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	const FVector newScale = FMath::Lerp(MovementData.StartScale, MovementData.EndScale, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	
	Owner->SetActorLocation(newPosition);
	Owner->SetActorRotation(newRotation);
	Owner->SetActorScale3D(newScale);

	if(MovementData.Interpolator >= 1 )
	{
		Owner->SetActorLocation(MovementData.EndPosition);
		Owner->SetActorRotation(MovementData.EndRotation);
		Owner->SetActorScale3D(MovementData.EndScale);
		MovementData.OnDrawCardEnd.ExecuteIfBound();
	}
}

