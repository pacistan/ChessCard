// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCUserExperienceDefinition.generated.h"


class UTexture2D;
class UUserWidget;
class UCommonSession_HostSessionRequest;

/**
 *  Setting For the Experience In UI And Start A New Session 
 */
UCLASS(BlueprintType)
class CHESSCARD_API UCCUserExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public :
	
	/** The specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;
	
	/** Primary title in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	FText TileTitle;

	/** Full description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	FText TileDescription;

	/** Icon used in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	TObjectPtr<UTexture2D> TileIcon;
	
	/** The loading screen widget to show when loading into (or back out of) a given experience */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=LoadingScreen)
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	/** Max number of players for this session */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience)
	int32 MaxPlayerCount = 4;

public:
	/** Create a request object that is used to actually start a session with these settings */
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;
};
