// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Examples/Service/twoIntService.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RosPawn.generated.h"

UCLASS()
class ROS4UNREAL_API ARosPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARosPawn();
	UtwoIntService* service;
	void call() {
		UE_LOG(LogTemp, Warning, TEXT("EVENT on keyboard touch B no Stats"));
		for (int k = 0; k < 10; k++) {
			service->call(k);
			//FPlatformProcess::SleepNoStats(0.1);
		}
		
	}
	void call2() {
		UE_LOG(LogTemp, Warning, TEXT("EVENT on keyboard touch U"));
		service->call(l++);
			//FPlatformProcess::Sleep(0.1);
	}

	void testTask() {
		UE_LOG(LogTemp, Warning, TEXT("EVENT on keyboard touch T(ask)"));
		service->task();
	}

	void unlock_task() {
		UE_LOG(LogTemp, Warning, TEXT("EVENT on keyboard touch Y unlockTask"));
		service->unlock_task();
	}

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int l = 0;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
