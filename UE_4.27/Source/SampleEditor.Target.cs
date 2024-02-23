// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SampleEditorTarget : TargetRules
{
	public SampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Sample" } );

		if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac)
		{
			// for Xcode 13 or later
			bOverrideBuildEnvironment = true;
			AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
		}
	}
}
