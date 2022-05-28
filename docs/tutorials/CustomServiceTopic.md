# Create Custom Service or Topic Client

## Custom Topic 

Examples from chatterTopic

1. your new class must inherit from TopicBase and UActorComponent

2. Define structure for c++ and Blueprints: 

for c++ use :
```cpp 
typedef struct {
	std::string data;
} chatterMessage_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(chatterMessage_t, data)
```
for blueprint use : 
```cpp
USTRUCT(BlueprintType)
struct FchatterMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString data;
};
```
Define the type of the message for ROS 
```cpp
 FString type_topic_ = "std_msgs/String";
```

3. Define conversion between C++ type and Unreal Engine type 

```cpp
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
```

4. Adapt with the name of your message (here chatterMessage_t for c++ and FchatterMessage for Blueprint)

```cpp
	void Publish(const chatterMessage_t& msg) {
		TopicBase::publish<chatterMessage_t>(msg);
	};
	UFUNCTION(BlueprintCallable, Category = "chatterTopic")
	void Publish(const FchatterMessage& msg) {
		TopicBase::publish<chatterMessage_t>(fromUnreal(msg));
	};

	void socket_callback(FString msg) {
		nlohmann::json j = nlohmann::json::parse(fstring2string(msg));
		publishMessage_t<chatterMessage_t> revMes = j.get<publishMessage_t<chatterMessage_t>>();
		if (TopicBase::bInitialized) {
			if (revMes.op == "publish" && revMes.topic == fstring2string(stored_topic_name_)) {
				if (callback_) {
					callback_(revMes.msg);
				}
				else {
					newMessageEvent.Broadcast(toUnreal(revMes.msg));
				}
			}
		}
		
	};

	bool subscribe(std::function<void(chatterMessage_t)> callback) { 
		callback_ = callback;
		TopicBase::subscribe();
		return true;
	};
	
	std::function<void(chatterMessage_t)> callback_;

```

5. Define the category to easily find your function in Blueprint 
```cpp 
UFUNCTION(BlueprintCallable, Category = "chatterTopic")
```



## Custom Service
Examples from twoIntsService
1. your new class must inherit from ServiceBase and UActorComponent


1. Define structure for c++ and Blueprints: 

for c++ use :
```cpp 
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
```
for blueprint use : 
```cpp

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
```

1. Define conversion between C++ type and Unreal Engine type 

```cpp
	
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
```

4. Adapt with the name of your message

```cpp
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

```

5. Define the category to easily find your function in Blueprint 
```cpp 
UFUNCTION(BlueprintCallable, Category = "twoIntService")
```
