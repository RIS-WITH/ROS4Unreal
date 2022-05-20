// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ROS4UnrealSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "ROS4Unreal Settings"))
class ROS4UNREAL_API UROS4UnrealSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "ROS Link", meta = (DisplayName = "IP address "))
		FString IP = "127.0.0.1";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "ROS Link", meta = (DisplayName = "Port"))
		int Port = 9090;
};
