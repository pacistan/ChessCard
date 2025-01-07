#include "TileActor/CCSplineMeshActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "TileActor/CCPieceBase.h"


// Sets default values
ACCSplineMeshActor::ACCSplineMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = SplineComponent;
}


