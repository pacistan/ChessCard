// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCUserExperience.generated.h"

/**
 *  Setting For the Experience At Start of a New Session
 */
UCLASS()
class CHESSCARD_API UCCUserExperience : public UDataAsset
{
	GENERATED_BODY()
public :
	/** The specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;
	
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
