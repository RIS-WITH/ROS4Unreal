// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Service/ServiceBase.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "twoIntService.generated.h"

typedef struct {
	int a;
	int b;
} requestAddTwoInts_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(requestAddTwoInts_t, a, b)

typedef struct {
	int sum;
} responseAddTwoInts_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(responseAddTwoInts_t, sum)

typedef struct {
	requestAddTwoInts_t req;
	responseAddTwoInts_t res;
}serviceMessageAddTwoInts;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(serviceMessageAddTwoInts, req, res)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROS4UNREAL_API UtwoIntService : public UActorComponent, public ServiceBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UtwoIntService();

	void initialize(const FString& service_name) {
		socket = NewObject<UWebSocket>();
		ServiceBase::initialize(socket, service_name);
		socket->socket_->OnMessage().AddLambda([this](const FString& msg)->void {AsyncTask(ENamedThreads::GameThread, [this, msg]() {
			//UE_LOG(LogTemp, Error, TEXT("ASYNC FUNC"));
			callbackService(msg);
			});
		});

	}

	bool call(const requestAddTwoInts_t& req, responseAddTwoInts_t& res) {
		return ServiceBase::call<requestAddTwoInts_t, responseAddTwoInts_t>(req, res);
	}

	void callbackService(const FString& msg) {
		//UE_LOG(LogTemp, Warning, TEXT("Dans le callback Ontologenius"));
		ServiceBase::callbackService<responseAddTwoInts_t>(msg);
	}

	UWebSocket* socket;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) {
		ServiceBase::EndPlay(EndPlayReason);
	}

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
