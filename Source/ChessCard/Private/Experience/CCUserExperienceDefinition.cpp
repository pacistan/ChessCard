// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Experience/CCUserExperienceDefinition.h"

#include "CommonSessionSubsystem.h"


UCommonSession_HostSessionRequest* UCCUserExperienceDefinition::CreateHostingRequest() const
{
	const FString UserExperienceName = GetPrimaryAssetId().PrimaryAssetName.ToString();
	UCommonSession_HostSessionRequest* HostSessionRequest = NewObject<UCommonSession_HostSessionRequest>();
	HostSessionRequest->OnlineMode  = ECommonSessionOnlineMode::LAN;
	HostSessionRequest->bUseLobbies = false;
	HostSessionRequest->MapID = MapID;
	HostSessionRequest->ModeNameForAdvertisement =  UserExperienceName;
	HostSessionRequest->MaxPlayerCount = MaxPlayerCount;
	
	return HostSessionRequest;
}
