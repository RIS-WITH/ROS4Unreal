// Fill out your copyright notice in the Description page of Project Settings.


#include "RosPawn.h"

// Sets default values
ARosPawn::ARosPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	service = CreateDefaultSubobject<UtwoIntService>("service");
}

// Called when the game starts or when spawned
void ARosPawn::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ARosPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARosPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindAction("call", IE_Pressed, this, &ARosPawn::call);
		PlayerInputComponent->BindAction("call2", IE_Pressed, this, &ARosPawn::call2);
		PlayerInputComponent->BindAction("task", IE_Pressed, this, &ARosPawn::testTask);
		PlayerInputComponent->BindAction("unlock", IE_Pressed, this, &ARosPawn::unlock_task);

	}
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

