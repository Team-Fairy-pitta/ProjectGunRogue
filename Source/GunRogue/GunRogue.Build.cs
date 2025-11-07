// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GunRogue : ModuleRules
{
	public GunRogue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] {
			"GunRogue"
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			// Base
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",

			// Gameplay
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",

			// UMG
			"UMG",

			//AI
			"AIModule", "NavigationSystem",

		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			//UMG
			"Slate", "SlateCore",
		});
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
