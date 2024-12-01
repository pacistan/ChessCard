// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChessCard : ModuleRules
{
	public ChessCard(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", 
			"GameplayTags", "DeveloperSettings", "NetCore"});

		PrivateDependencyModuleNames.AddRange(new string[] { "CommonUser", "CommonUI", "UIExtension"});

		// Online 
		PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
