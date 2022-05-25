// Fill out your copyright notice in the Description page of Project Settings.


#include "Examples/ClientExamples.h"

void chatterCallback(const chatterMessage_t & msg) {
	
	UE_LOG(LogTemp, Warning, TEXT("Message receive in the callback for subscribe : %s"), *string2Fstring(msg.data));
}

void chatterCallback2(const chatterMessage_t & msg) {

	UE_LOG(LogTemp, Warning, TEXT(" % s Message receive in client 2 "), *string2Fstring(msg.data));
}

// Sets default valuess
AClientExamples::AClientExamples()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	topic_client = CreateDefaultSubobject<UchatterTopic>("listener");
	topic_client2 = CreateDefaultSubobject<UchatterTopic>("listener2");
	service_client = CreateDefaultSubobject<UtwoIntService>("clientTwoInts");

	


}

// Called when the game starts or when spawned
void AClientExamples::BeginPlay()
{
	Super::BeginPlay();
	topic_client->initialize("/chatter1", "std_msgs/String");
	topic_client2->initialize("/chatter2", "std_msgs/String");
	service_client->initialize("/add_two_ints");
	
	//topic_client->callback_ = callbackSubscribe;
	topic_client->subscribe(chatterCallback);
	topic_client2->subscribe(chatterCallback2);
	

	
}

// Called every frame
void AClientExamples::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	requestAddTwoInts_t req;
	req.a = 12;
	req.b = 30;
	responseAddTwoInts_t res;
	if (service_client->call(req, res)) {
		UE_LOG(LogTemp, Warning, TEXT("RESPONSE : ---- %d"), res.sum);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Call dropout"));
	}

}

