#include "Card/CCCardMovementComponent.h"
#include "Card/CCCard.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CCPlayerPawn.h"


UCCCardMovementComponent::UCCCardMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UCCCardMovementComponent::StartMovement(int InCardIndex, int InHandNumber, FOnCardMovementEnd OnCardMovementEnd, bool IsCustomDuration, float CustomDuration)
{
	if(!IsValid(this) || !IsValid(GetWorld()))
	{
		UE_LOG(LogTemp, Error, TEXT("NO WORLD"));
		return false;
	}
	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(!IsValid(CameraManager))
	{
		UE_LOG(LogTemp, Error, TEXT("NO CAMERA MANAGER"));
		return false;
	}
	//PrimaryComponentTick.SetTickFunctionEnable(true);
	MovementData.Interpolator = 0;
	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRightVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
	const FVector CameraUpVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Z);
	auto State = Owner->GetCurrentCardState();
	
	MovementData.OnDrawCardEnd = OnCardMovementEnd;
	MovementData.StartPosition = GetOwner()->GetActorLocation();
	MovementData.StartRotation = GetOwner()->GetActorRotation();
	MovementData.StartScale = GetOwner()->GetActorScale();
	
	
	float HoveredPositionOffset = ExposedDataMap[State].HeightOffset;
	int32 HoveredLayerNumber = ExposedDataMap[State].LayerNumber;
	MovementData.EndScale = BaseScale * ExposedDataMap[State].ScaleCoefficient;
	CurrentDuration = IsCustomDuration ? CustomDuration : ExposedDataMap[State].Duration;
	MovementData.EndPosition =FVector(CameraLocation + (CameraRotation.Vector() * (DistanceFromCamera - HoveredLayerNumber))
	+ CameraRightVector * HandPositionOffset.X 
	+ CameraRightVector * CardXPositionOffSet * InCardIndex
	- CameraRightVector * CardXPositionOffSet * (InHandNumber - 1) / 2.0f
	+ CameraUpVector * HandPositionOffset.Y
	- CameraUpVector * CardHeightPositionOffset * FMath::Abs(InCardIndex - (InHandNumber - 1) / 2.0f)
	- CameraUpVector * CardHeightPositionOffset
	+ CameraUpVector * HoveredPositionOffset);
	IsMoving = true;
	MovementData.EndRotation = MovementData.StartRotation;
	MovementData.EndRotation.Roll = FMath::Lerp(
		CardTiltOffset * static_cast<float>(InHandNumber - 1) / 2,
		-CardTiltOffset * static_cast<float>(InHandNumber - 1) / 2,
		InCardIndex / static_cast<float>(InHandNumber - 1));
	//PrimaryComponentTick.SetTickFunctionEnable(true);
	return true;
}

void UCCCardMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(IsMoving)
	{
		MovementTick(DeltaTime);
	}
}

void UCCCardMovementComponent::MovementTick(float DeltaTime)
{
	if(!GetWorld())
	{
		//PrimaryComponentTick.SetTickFunctionEnable(false);
		return;
	}

	
	ECardState State =  Owner->GetCurrentCardState();
	MovementData.Interpolator = FMath::Clamp(MovementData.Interpolator +  DeltaTime / CurrentDuration, 0, 1);
	const FVector newPosition = FMath::Lerp(MovementData.StartPosition, MovementData.EndPosition, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	const FRotator newRotation = FMath::Lerp(MovementData.StartRotation, MovementData.EndRotation, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	const FVector newScale = FMath::Lerp(MovementData.StartScale, MovementData.EndScale, ExposedDataMap[State].AnimCurve->FloatCurve.Eval(MovementData.Interpolator));
	
	Owner->SetActorLocation(newPosition);
	Owner->SetActorRotation(newRotation);
	Owner->SetActorScale3D(newScale);

	if(MovementData.Interpolator >= 1 )
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement Ended"));
		Owner->SetActorLocation(MovementData.EndPosition);
		Owner->SetActorRotation(MovementData.EndRotation);
		Owner->SetActorScale3D(MovementData.EndScale);
		MovementData.OnDrawCardEnd.ExecuteIfBound();
		//PrimaryComponentTick.SetTickFunctionEnable(false);
		IsMoving = false;
	}
}

void UCCCardMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = Cast<ACCCard>(GetOwner());
	BaseScale = Owner->GetActorScale();
}

