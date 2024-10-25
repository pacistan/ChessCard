// Fill out your copyright notice in the Description page of Project Settings.


#include "Experience/CCUserExperience.h"

#include "CommonSessionSubsystem.h"


UCommonSession_HostSessionRequest* UCCUserExperience::CreateHostingRequest() const
{
	// const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	const FString UserExperienceName = GetPrimaryAssetId().PrimaryAssetName.ToString();
	UCommonSession_HostSessionRequest* HostSessionRequest = NewObject<UCommonSession_HostSessionRequest>();
	HostSessionRequest->OnlineMode  = ECommonSessionOnlineMode::Online;
	HostSessionRequest->bUseLobbies = true;
	HostSessionRequest->MapID = MapID;
	HostSessionRequest->ModeNameForAdvertisement =  UserExperienceName;
	//HostSessionRequest->ExtraArgs.Add(TEXT("Experience"), ExperienceName);
	HostSessionRequest->MaxPlayerCount = MaxPlayerCount;
	
	return HostSessionRequest;
}
