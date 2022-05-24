// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <string>
#include "utils/json.h"
#include "Topic/TopicBase.h"
#include "utils/typeDefinitions.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "chatterTopic.generated.h"




typedef struct {
	std::string data;
} chatterMessage_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(chatterMessage_t, data)

USTRUCT(BlueprintType)
struct FchatterMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString data;
};


UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FnewMessageEvent, FchatterMessage, newMessage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROS4UNREAL_API UchatterTopic : public UActorComponent, public TopicBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	
	UchatterTopic();
	UFUNCTION(BlueprintCallable, Category = "chatterTopic")
	void initialize(const FString& topic_name, const FString& type_topic) {
		stored_topic_name_ = topic_name;
		stored_type = type_topic;
		socket = NewObject<UWebSocket>();
		TopicBase::initialize(socket, topic_name);
		socket->socket_->OnMessage().AddLambda([this](const FString& msg)->void {
			this->socket_callback(msg);
			});
	}

	static chatterMessage_t fromUnreal(const FchatterMessage& msg) {
		chatterMessage_t c;
		c.data = fstring2string(msg.data);
		return c;
	}

	static FchatterMessage toUnreal(const chatterMessage_t& msg) {
		FchatterMessage c;
		c.data = string2Fstring(msg.data);
		return c;
	}

	void Publish(const chatterMessage_t& msg) {
		TopicBase::publish<chatterMessage_t>(msg);
	};
	UFUNCTION(BlueprintCallable, Category = "chatterTopic")
	void Publish(const FchatterMessage& msg) {
		TopicBase::publish<chatterMessage_t>(fromUnreal(msg));
	};

	void socket_callback(FString msg) {

		//UE_LOG(LogTemp, Error, TEXT("Message recu dans le callback chatter: %s"), *msg);

		nlohmann::json j = nlohmann::json::parse(fstring2string(msg));
		publishMessage_t<chatterMessage_t> revMes = j.get<publishMessage_t<chatterMessage_t>>();
		if (revMes.op == "publish" && revMes.topic == fstring2string(stored_topic_name_)) {
			if (callback_) {
				//UE_LOG(LogTemp, Warning, TEXT("Message Recu socket callback callback enregister --- "));
				callback_(revMes.msg);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Message recu sans callback action --- : %s"), *string2Fstring(revMes.msg.data));
				newMessageEvent.Broadcast(toUnreal(revMes.msg));
			}	
		}
	};

	bool subscribe(std::function<void(chatterMessage_t)> callback) { //TODO Add number to queue message;
		//UE_LOG(LogTemp, Warning, TEXT("In Subscribe"));
		callback_ = callback;
		TopicBase::subscribe();
		return true;
	};
	

	UWebSocket* socket;
	std::function<void(chatterMessage_t)> callback_;
	FchatterMessage currentUnrealmsg;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) {
		TopicBase::EndPlay(EndPlayReason);
	}
	UFUNCTION(BlueprintCallable, Category = "chatterTopic")
	void subscribe() {
		TopicBase::subscribe();
	}

	UPROPERTY(BlueprintReadWrite,BlueprintAssignable,BlueprintCallable,meta=(DisplayName="OnnewMessage",Category="chatterTopic"))
		FnewMessageEvent newMessageEvent;

	/*
	UFUNCTION(BlueprintImplementableEvent, Category = "chatterTopic", meta = (DisplayName = "new Message from Topic "))
		void newMessageSubscribe(const FchatterMessage& msg);
	
	UFUNCTION(BlueprintNativeEvent, Category = "chatterTopic", meta = (DisplayName = "new Message from Topic "))
		void newMessageSubscribe(const FchatterMessage& msg);
	*/
	


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
