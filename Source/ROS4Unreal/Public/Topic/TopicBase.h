#pragma once
#include <string>
#include "utils/json.h"
#include "utils/typeDefinitions.h"
#include "CoreMinimal.h"
#include "Sockets.h"
#include "Socket/WebSocket.h"


typedef struct advertiseMessage_t {
	std::string op = "advertise";
	TOptional<std::string> id;
	std::string topic;
	std::string type;
} advertiseMessage_t;


inline void to_json(nlohmann::json& j, const advertiseMessage_t& M) {
	j = nlohmann::json{ {"op",M.op},{"topic",M.topic},{"type",M.type} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	};

};

inline void from_json(const nlohmann::json& j, advertiseMessage_t& M) {

	j.at("op").get_to(M.op);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}
	j.at("topic").get_to(M.topic);
	j.at("type").get_to(M.type);
};


typedef struct unadvertiseMessage_t {
	std::string op = "unadvertise";
	TOptional<std::string> id;
	std::string topic;
} unadvertiseMessage_t;

inline void to_json(nlohmann::json& j, const unadvertiseMessage_t& M) {
	j = nlohmann::json{ {"op",M.op},{"topic",M.topic} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	};

};

inline void from_json(const nlohmann::json& j, unadvertiseMessage_t& M) {

	j.at("op").get_to(M.op);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}
	j.at("topic").get_to(M.topic);

};


template<typename Message>
struct publishMessage_t {
	std::string op = "publish";
	TOptional<std::string> id;
	std::string topic;
	Message msg;
};

template<typename Message>
inline void to_json(nlohmann::json& j, const publishMessage_t<Message>& M) {
	j = nlohmann::json{ {"op",M.op},{"topic",M.topic},{"msg",M.msg} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	};
};

template<typename Message>
inline void from_json(const nlohmann::json& j, publishMessage_t<Message>& M) {

	j.at("op").get_to(M.op);
	j.at("topic").get_to(M.topic);
	j.at("msg").get_to(M.msg);

};



typedef struct subscribeMessage_t {
	std::string op = "subscribe";
	TOptional<std::string> id;
	std::string topic;
	TOptional<std::string> type;
	TOptional<int> queue_length = 1;
} subscribeMessage_t;


inline void to_json(nlohmann::json& j, const subscribeMessage_t& M) {
	j = nlohmann::json{ {"op",M.op},{"topic",M.topic}, };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	};
	if (M.type.IsSet()) {
		j["type"] = M.type.GetValue();
	};
	if (M.queue_length.IsSet()) {
		j["queue_length"] = M.queue_length.GetValue();
	};
};


inline void from_json(const nlohmann::json& j, subscribeMessage_t& M) {

	j.at("op").get_to(M.op);
	j.at("topic").get_to(M.topic);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}
	if (j.count("type") != 0)
	{
		M.type = j.at("type").get< std::string >();
	}
	if (j.count("queue_length") != 0)
	{
		M.queue_length = j.at("queue_length").get<int>();
	}
};


typedef struct unsubscribeMessage_t {
	std::string op = "unsubscribe";
	TOptional<std::string> id;
	std::string topic;
} unsubscribeMessage_t;


inline void to_json(nlohmann::json& j, const unsubscribeMessage_t& M) {
	j = nlohmann::json{ {"op",M.op},{"topic",M.topic}, };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	};

};


inline void from_json(const nlohmann::json& j, unsubscribeMessage_t& M) {

	j.at("op").get_to(M.op);
	j.at("topic").get_to(M.topic);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}

};



class TopicBase
{
public:

	/**
	 * Initializes this topic
	 * @param TopicName - Topic name that is used to subscribe/advertize later. Used as an ID internally.
	 * @param MessageClass - The message class that is used to (de-)serialize
	 */
	inline void initialize(UWebSocket* socket, const FString& TopicName) {

		stored_topic_name_ = TopicName;
		socket_ = socket;
		socket_->initialize();

	};

	/**
	 * Subscribe this topic and get notified if a new message arrives (C++ variant)
	 */
	bool subscribe() {
		subscribeMessage_t subMsg;
		subMsg.id = subMsg.op+":"+ fstring2string(stored_topic_name_)+":"+getNextId();
		subMsg.topic = fstring2string(stored_topic_name_);
		subMsg.type = "std_msgs/String";

		nlohmann::json j = subMsg;
		//UE_LOG(LogTemp, Warning, TEXT("TEST JSON : %s"), *string2Fstring(j.dump()));
		
		if (socket_!=nullptr) {
			//UE_LOG(LogTemp, Warning, TEXT("Socket send "));
			socket_->sendMessage(string2Fstring(j.dump()));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Socket not init "));
		}
		

		return true;

	};

	/**
	 * Unsubscribe this topic. Automatically called on exit.
	 */
	void unsubscribe() {

	};

	/**
	 * Send an advertise message to the ROS Bridge. Called automatically on publish.
	 * @return Success of sending
	 */
	inline bool advertise() {
		if (!bInitialized)
		{
			UE_LOG(LogTemp, Warning, TEXT("You first have to initialize your ROSTopic before you Advertise it."));
			return false;
		}

		if (!IsAdvertised)
		{
			advertiseMessage_t advMsg;
			advMsg.id = advMsg.op+":" + fstring2string(stored_topic_name_)+":"+getNextId();
			advMsg.topic = fstring2string(stored_topic_name_);
			nlohmann::json j = advMsg;
			IsAdvertised = socket_->sendMessage(string2Fstring(j.dump()));


			/*
			UROSTopicAdvertiseMessage* AdvertiseMessage = NewObject<UROSTopicAdvertiseMessage>();
			AdvertiseMessage->ID = FString::Printf(TEXT("advertise:%s"), *StoredTopicName);
			AdvertiseMessage->TopicName = StoredTopicName;
			AdvertiseMessage->MessageType = StoredMessageClass.GetDefaultObject()->GetMessageType();

			IsAdvertised = SendMessage(*AdvertiseMessage); */
		}

		return IsAdvertised;
	};

	/**
	 * Send an unadverise message to the ROS Bridge
	 * @return Success of sending
	 */
	inline bool unadvertise() { return false; };

	/**
	 * Publish a message on the topic to the ROS Bridge
	 * @param Message - The message to send
	 */


	 /**
	  * @return The topic name specified in the initialize call. Internally used as an ID
	  */
	inline FString getTopicName() const {
		return stored_topic_name_;
	};


	/**
	 * Event that is called if a new message arrives (Blueprint variant)
	 */
	 //UPROPERTY(VisibleAnywhere, BlueprintAssignable) 
	 //FReceiveSignature OnNewMessage;

	 /*
	  * Unsubscribes and kills connection on Endplay
	  */
	
	inline void EndPlay(const EEndPlayReason::Type EndPlayReason) {
		UE_LOG(LogTemp, Error, TEXT("KILL socket"));
		switch (EndPlayReason)
		{
		case EEndPlayReason::LevelTransition:
		case EEndPlayReason::EndPlayInEditor:
		case EEndPlayReason::RemovedFromWorld:
		case EEndPlayReason::Quit:
			unsubscribe();
			socket_->uninitialize();
		case EEndPlayReason::Destroyed:
			break;
		}
	};

protected:
	template<typename Message>
	inline void publish(const Message msg) {
		advertise();
		publishMessage_t<Message> pubMsg;
		pubMsg.msg = msg;
		pubMsg.id = pubMsg.op+":" + fstring2string(stored_topic_name_) + ":" + getNextId();
		pubMsg.topic = fstring2string(stored_topic_name_);
		nlohmann::json j = pubMsg;
		socket_->sendMessage(string2Fstring(j.dump()));

	};
	FString stored_topic_name_ = "";
	FString stored_type = "";
	bool bInitialized = true;
private:

	bool IsSubscribed = false;
	
	bool IsAdvertised = false;
	long id = 0;
	
	std::string getNextId() {
		return std::to_string(id++);
	};


	UWebSocket* socket_;

};


