// Fill out your copyright notice in the Description page of Project Settings.


#include "Examples/ClientExamples.h"

void callbackSubscribe(chatterMessage_t msg) {
	
	UE_LOG(LogTemp, Warning, TEXT("Message receive in the callback for subscribe : %s"), *string2Fstring(msg.data));
}

void callbackSubscribe2(chatterMessage_t msg) {

	UE_LOG(LogTemp, Warning, TEXT(" % s Message receive in client 2 "), *string2Fstring(msg.data));
}

// Sets default values
AClientExamples::AClientExamples()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	topic_client = CreateDefaultSubobject<UchatterTopic>("/chatter1");
	topic_client2 = CreateDefaultSubobject<UchatterTopic>("/chatter2");

	


}

// Called when the game starts or when spawned
void AClientExamples::BeginPlay()
{
	Super::BeginPlay();
	topic_client->initialize("/chatter1", "std_msgs/String");
	topic_client2->initialize("/chatter2", "std_msgs/String");
	//topic_client->callback_ = callbackSubscribe;
	topic_client->subscribe(callbackSubscribe);
	topic_client2->subscribe(callbackSubscribe2);
	
}

// Called every frame
void AClientExamples::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

