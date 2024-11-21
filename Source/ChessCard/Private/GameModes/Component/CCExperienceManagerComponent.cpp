// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Component/CCExperienceManagerComponent.h"

#include "Engine/AssetManager.h"
#include "Experience/CCExperienceDefinition.h"
#include "Macro/CCLogMacro.h"
#include "Net/UnrealNetwork.h"


UCCExperienceManagerComponent::UCCExperienceManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UCCExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UAssetManager& AssetManager = UAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	TSubclassOf<UCCExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UCCExperienceDefinition* Experience = GetDefault<UCCExperienceDefinition>(AssetClass);

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Experience;
	StartExperienceLoad();
}

void UCCExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnCCExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded()) {
		Delegate.Execute(CurrentExperience);
	} else {
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UCCExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnCCExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded()){
		Delegate.Execute(CurrentExperience);
	} else {
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

const UCCExperienceDefinition* UCCExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == ECCExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

bool UCCExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == ECCExperienceLoadState::Loaded) && (CurrentExperience != nullptr);
}

void UCCExperienceManagerComponent::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

void UCCExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience != nullptr);
	check(LoadState == ECCExperienceLoadState::Unloaded);

	DEBUG_LOG("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s,)", *CurrentExperience->GetPrimaryAssetId().ToString());

	LoadState = ECCExperienceLoadState::Loading;
	
	// TODO Load assets associated with the experience ( Decors of the game )
	OnExperienceLoadComplete();
}

void UCCExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState != ECCExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	
	DEBUG_LOG("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s)", *CurrentExperience->GetPrimaryAssetId().ToString());
	
	LoadState = ECCExperienceLoadState::Loaded;
	
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();
}

void UCCExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (LoadState == ECCExperienceLoadState::Loaded) {
		LoadState = ECCExperienceLoadState::Deactivating;

		// TODO Desactivate special rules load by the experience if there is

		LoadState = ECCExperienceLoadState::Unloaded;
		CurrentExperience = nullptr;
	}
}

void UCCExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentExperience);
}

