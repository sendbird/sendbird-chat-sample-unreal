// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

using UnrealBuildTool;
using System.IO;

public class Sendbird : ModuleRules
{
    public Sendbird(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject",
                "Engine",
            }
        );

        bool isLibrarySupported = false;

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string arm64_v8aPath = Path.Combine(ModuleDirectory, "lib", "android", "arm64-v8a", "libSendbirdChat.so");
            if (File.Exists(arm64_v8aPath))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(arm64_v8aPath);
            }

            string armeabi_v7aPath = Path.Combine(ModuleDirectory, "lib", "android", "armeabi-v7a", "libSendbirdChat.so");
            if (File.Exists(armeabi_v7aPath))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(armeabi_v7aPath);
            }

            string x86Path = Path.Combine(ModuleDirectory, "lib", "android", "x86", "libSendbirdChat.so");
            if (File.Exists(x86Path))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(x86Path);
            }

            string x86_64Path = Path.Combine(ModuleDirectory, "lib", "android", "x86_64", "libSendbirdChat.so");
            if (File.Exists(x86_64Path))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(x86_64Path);
            }

            string BuildPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(BuildPath, "Sendbird_UPL.xml"));
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            string iosPath = Path.Combine(ModuleDirectory, "lib", "ios", "arm64", "libSendbirdChat.a");
            if (File.Exists(iosPath))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(iosPath);
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string win64Path = Path.Combine(ModuleDirectory, "lib", "windows", "x64", "SendbirdChat.lib");
            if (File.Exists(win64Path))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(win64Path);
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string macPath = Path.Combine(ModuleDirectory, "lib", "mac", "universal", "libSendbirdChat.a");
            if (File.Exists(macPath))
            {
                isLibrarySupported = true;
                PublicAdditionalLibraries.Add(macPath);
            }
        }

        if (isLibrarySupported)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

            bUseRTTI = true;
            bEnableExceptions = true;
        }

        PublicDefinitions.Add(string.Format("WITH_SENDBIRD={0}", isLibrarySupported ? 1 : 0));
    }
}
