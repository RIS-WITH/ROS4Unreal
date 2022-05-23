// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ROS4UnrealSettings.h"

class FROS4UnrealModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnSessionEnd(UWorld* World, bool bSessionEnded, bool);


	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ROS4Unreal");
	}

	static inline FROS4UnrealModule& Get() {
		return FModuleManager::LoadModuleChecked<FROS4UnrealModule>("ROS4Unreal");
	}

	static const UROS4UnrealSettings* GetSettings() {
		static const UROS4UnrealSettings* Settings = GetDefault<UROS4UnrealSettings>();
		return Settings;
	}
};
