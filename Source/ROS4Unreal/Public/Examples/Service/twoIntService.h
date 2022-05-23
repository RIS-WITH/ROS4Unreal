// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Service/ServiceBase.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "twoIntService.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROS4UNREAL_API UtwoIntService : public UActorComponent, public ServiceBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UtwoIntService();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
