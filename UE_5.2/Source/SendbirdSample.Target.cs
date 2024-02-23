// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SendbirdSampleTarget : TargetRules
{
	public SendbirdSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("SendbirdSample");

		if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac)
		{
			// for UE4.27.2
			bOverrideBuildEnvironment = true;
			//AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
		}
	}
}
