// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GoGoHunters : ModuleRules
{
	public GoGoHunters(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"HeadMountedDisplay", 
			"Niagara",
			"HTTP", 
			"WebSockets",
            "AudioMixer",
            "AudioCapture",
            "AudioCaptureCore",
            "SignalProcessing",
            "Json",
            "JsonUtilities",
            "UMG", "Slate", "SlateCore"
        });
	}
}
