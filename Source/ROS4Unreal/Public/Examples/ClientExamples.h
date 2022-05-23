// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Topic/chatterTopic.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClientExamples.generated.h"

UCLASS()
class ROS4UNREAL_API AClientExamples : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClientExamples();
	UchatterTopic* topic_client;
	UchatterTopic* topic_client2;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
