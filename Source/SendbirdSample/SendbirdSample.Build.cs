// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SendbirdSample : ModuleRules
{
	public SendbirdSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "OpenSSL", "Sendbird" });
	}
}
