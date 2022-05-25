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


USTRUCT(BlueprintType)
struct FrequestAddTwoInts
{
	GENERATED_USTRUCT_BODY()
public:
		UPROPERTY(BlueprintReadWrite)
		int a;
		UPROPERTY(BlueprintReadWrite)
		int b;
};

USTRUCT(BlueprintType)
struct FresponseAddTwoInts
{
	GENERATED_USTRUCT_BODY()
public:
		UPROPERTY(BlueprintReadWrite)
		int sum;
};
USTRUCT(BlueprintType)
struct FserviceMessageAddTwoints
{
	GENERATED_USTRUCT_BODY()
public:
		UPROPERTY(BlueprintReadWrite)
		FrequestAddTwoInts req;
		UPROPERTY(BlueprintReadWrite)
		FresponseAddTwoInts res;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROS4UNREAL_API UtwoIntService : public UActorComponent, public ServiceBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UtwoIntService();

	UFUNCTION(BlueprintCallable, Category = "twoIntService")
	void initialize(const FString& service_name) {
		socket = NewObject<UWebSocket>();
		ServiceBase::initialize(socket, service_name);
		socket->socket_->OnMessage().AddLambda([this](const FString& msg)->void {AsyncTask(ENamedThreads::GameThread, [this, msg]() {
			//UE_LOG(LogTemp, Error, TEXT("ASYNC FUNC"));
			callbackService(msg);
			});
		});

	}

	static requestAddTwoInts_t fromUnreal(const FrequestAddTwoInts& msg) {
		requestAddTwoInts_t c;
		c.a = msg.a;
		c.b = msg.b;
		return c;
	}

	static FrequestAddTwoInts toUnreal(const requestAddTwoInts_t& msg) {
		FrequestAddTwoInts c;
		c.a = msg.a;
		c.b = msg.b;
		return c;
	}

	static responseAddTwoInts_t fromUnreal(const FresponseAddTwoInts& msg) {
		responseAddTwoInts_t c;
		c.sum = msg.sum;
		return c;
	}

	static FresponseAddTwoInts toUnreal(const responseAddTwoInts_t& msg) {
		FresponseAddTwoInts c;
		c.sum = msg.sum;
		return c;
	}

	static serviceMessageAddTwoInts fromUnreal(const FserviceMessageAddTwoints& msg) {
		serviceMessageAddTwoInts c;
		c.req = fromUnreal(msg.req);
		c.res = fromUnreal(msg.res);
		return c;
	}

	static FserviceMessageAddTwoints toUnreal(const serviceMessageAddTwoInts& msg) {
		FserviceMessageAddTwoints c;
		c.req = toUnreal(msg.req);
		c.res = toUnreal(msg.res);
		return c;
	}

	UFUNCTION(BlueprintCallable, Category = "twoIntService")
	FresponseAddTwoInts call(const FrequestAddTwoInts& reqUnreal) {
		responseAddTwoInts_t res;
		requestAddTwoInts_t req = fromUnreal(reqUnreal);
		call(req, res);
		return toUnreal(res);
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

	UFUNCTION(BlueprintCallable)
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override {
		ServiceBase::EndPlay(EndPlayReason);
	}

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
