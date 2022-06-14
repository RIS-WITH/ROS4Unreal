#pragma once
#include "Socket/WebSocket.h"
#include <string>
#include "Misc/Optional.h"
#include "Misc/ScopeLock.h"
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>
#include <condition_variable>
#include "utils/json.h"
#include "utils/typeDefinitions.h"
#include "CoreMinimal.h"
#include "Sockets.h"
#include <ctime>



typedef struct advertiseServiceMessage_t {
	std::string op = "advertise_service";
	std::string type;
	std::string service;
} advertiseServiceMessage_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(advertiseServiceMessage_t, op, service, type)

typedef struct unadvertiseServiceMessage_t {
	std::string op = "unadvertise_service";
	std::string service;
} unadvertiseServiceMessage_t;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(unadvertiseServiceMessage_t, op, service)

template<typename Request>
struct callService_t {
	std::string op = "call_service";
	TOptional<std::string> id;
	std::string service;
	TOptional<Request> args;
};

template<typename Request>
inline void to_json(nlohmann::json& j, const callService_t<Request>& M) {
	j = nlohmann::json{ {"op",M.op},{"service",M.service} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	}
	if (M.args.IsSet()) {
		j["args"] = M.args.GetValue();
	}


};

template<typename Request>
inline void from_json(const nlohmann::json& j, callService_t<Request>& M) {

	j.at("op").get_to(M.op);
	j.at("service").get_to(M.service);

	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}
	if (j.count("args") != 0)
	{
		j.at("args").get<Request>(M.args);
	}
};

struct serviceResponseCheck_t {
	std::string op = "service_response";
	TOptional<std::string> id;
	std::string service;
	bool result;
};


inline void to_json(nlohmann::json& j, const serviceResponseCheck_t& M) {
	j = nlohmann::json{ {"op",M.op},{"service",M.service},{"result",M.result} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	}

};

inline void from_json(const nlohmann::json& j, serviceResponseCheck_t& M) {

	j.at("op").get_to(M.op);
	j.at("service").get_to(M.service);
	j.at("result").get_to(M.result);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}

};


template<typename Response>
struct serviceResponse_t {
	std::string op = "service_response";
	TOptional<std::string> id;
	std::string service;
	TOptional<Response> values;
	bool result;
};





template<typename Response>
inline void to_json(nlohmann::json& j, const serviceResponse_t<Response>& M) {
	j = nlohmann::json{ {"op",M.op},{"service",M.service},{"result",M.result} };
	if (M.id.IsSet()) {
		j["id"] = M.id.GetValue();
	}
	if (M.values.IsSet()) {
		j["values"] = M.values.GetValue();
	}
};

template<typename Response>
inline void from_json(const nlohmann::json& j, serviceResponse_t<Response>& M) {

	j.at("op").get_to(M.op);
	j.at("service").get_to(M.service);
	j.at("result").get_to(M.result);
	if (j.count("id") != 0)
	{
		M.id = j.at("id").get< std::string >();
	}
	if (j.count("values") != 0)
	{
		M.values = j.at("values").get<Response>();
	}
};



class ServiceBase
{
public:

	/**
	 * Initialize handle
	 * @param ServiceName - Service name that is used to advertise/call later. Used as an ID internally.
	 * @param socket - Instance of socket is to communication later.
	 * @param ServiceClass - The service class that is responsible for (de-)serialization of messages
	 */
	void initialize(UWebSocket* socket, const FString & ServiceName) {

		stored_service_name = ServiceName;
		socket_ = socket;
		socket_->initialize();
		bInitialized = true;
		U_Event_service = FGenericPlatformProcess::GetSynchEventFromPool(false);
		std::ofstream myfile;
	


		auto f = []() {

			std::ofstream myfile;
			myfile.open("cpplog.txt");
			for (int i = 0; i < 1000; i++) {
				myfile << "logcpp i: " << i << " time : " << time(NULL);
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			myfile.close();

		};
		f();
		//std::thread th(f);


	};

	/**
	 * Advertise this service to the ROS Bridge (C++ variant)
	 * @param Callback - Function that should process requests
	 * not yet finished
	 */
	bool advertise() {
		if (!bInitialized)
		{
			UE_LOG(LogTemp, Error, TEXT("You first have to initialize your ROSTopic before you Advertise it."));
			return false;
		}

		if (!bAdvertised)
		{
			advertiseServiceMessage_t advMsg;
			advMsg.type = "Type";
			advMsg.service = fstring2string(stored_service_name);
			nlohmann::json j = advMsg;
			bAdvertised = socket_->sendMessage(string2Fstring(j.dump()));

		}

		return bAdvertised;
	};

	/**
	 * Unadvertise this service from the ROS Bridge (does nothing if not advertised)
	 */
	bool unadvertise() {
		if (!bAdvertised) return true; //No need to unadvertise

		unadvertiseServiceMessage_t unadvMsg;
		unadvMsg.service = fstring2string(stored_service_name);
		nlohmann::json j = unadvMsg;
		bAdvertised = socket_->sendMessage(string2Fstring(j.dump()));
		return !bAdvertised;
	};

	/**
	 * Call the service via the ROS Bridge (C++ variant)
	 * @param Service - With Request to send and Response to receive data
	 */
	template<typename Serv>
	bool call(Serv& service) {
		return Call(service.request, service.response);
	};
	/**
	 * Call the service via the ROS Bridge (C++ variant)
	 * @param Request - Request to send
	 * @param Response - Response to receive data
	 */
	template<typename Request, typename Response>
	bool call(const Request& req, Response& res) {
		FScopeLock ScopeLock(&UmutexService);
		U_Event_service->Reset();
		string_resp = "";

		if (!socket_->socket_->IsConnected()) {
			UE_LOG(LogTemp, Error, TEXT("Not connected"));
			return false;
		}
			
		if (!bInitialized)
		{
			UE_LOG(LogTemp, Error, TEXT("You first have to initialize your ROSServiceHandle before you can Call it."));
			return false;
		}

		if (bAdvertised) return false; //Can't call ourselfes

		callService_t<Request> callMsg;
		callMsg.service = fstring2string(stored_service_name);
		callMsg.args = req;
		callMsg.id = callMsg.op+":" + fstring2string(stored_service_name) + ":" + getNextId();
		current_id_ = callMsg.id.GetValue();
		UE_LOG(LogTemp, Warning, TEXT("Message sent with this id : %s"),*string2Fstring(current_id_));
		nlohmann::json j = callMsg;
		socket_->sendMessage(string2Fstring(j.dump()));
		//UE_LOG(LogTemp, Warning, TEXT("Lock avant"));

		//std::unique_lock<std::mutex> lck(syncMsg);
		//UE_LOG(LogTemp, Warning, TEXT("Lock apres"));
		/*
		if (cv.wait_for(lck, std::chrono::milliseconds(1200)) == std::cv_status::timeout) {
			UE_LOG(LogTemp, Warning, TEXT("boucle timeout 1sec %d"),time(NULL));
			return false;
		}
		*/
		/*
		int compt = 1200/10;
		while (compt >= 0)
		{
			if (!U_Event_service->Wait(10))
				compt--;
			else
				break;
		}
		UE_LOG(LogTemp, Warning, TEXT("Compt : %d"), compt);
		if (compt < 0) {
			UE_LOG(LogTemp, Error, TEXT("boucle timeout 1sec %d"), time(NULL));
			return false;
		}
		
		
		if (!string_resp.empty()) {
			nlohmann::json jres = nlohmann::json::parse(string_resp);
			serviceResponse_t<Response> inMsg = jres.get<serviceResponse_t<Response>>();
			res = inMsg.values.GetValue();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Empty response imposible to parse"));
			return false;
		}
		*/

		return true;


	}



	inline FString getServiceName() const {
		return stored_service_name;
	};


	  /*
	   * Undavertises and kills connection on Endplay
	   */
	
	inline void EndPlay(const EEndPlayReason::Type EndPlayReason)  {
		//UE_LOG(LogTemp, Error, TEXT("KILL socket"));
		switch (EndPlayReason)
		{
		case EEndPlayReason::LevelTransition:
			
		case EEndPlayReason::EndPlayInEditor:
		case EEndPlayReason::RemovedFromWorld:
		case EEndPlayReason::Quit:
			socket_->uninitialize();
		case EEndPlayReason::Destroyed:
			break;
			
		}
	};
	template<typename Response>
	void callbackService(const FString& msg) {

		UE_LOG(LogTemp, Error, TEXT("Message recu deans le callback: %s"), *msg);

		nlohmann::json j = nlohmann::json::parse(fstring2string(msg));
		serviceResponseCheck_t inMsg = j.get<serviceResponseCheck_t>();
		if (inMsg.op != "service_response") {
			UE_LOG(LogTemp, Error, TEXT("Wrong Op code : %s"), *msg);
			return;
		}
		if (inMsg.id.GetValue() != current_id_) {
			UE_LOG(LogTemp, Error, TEXT("Wrong id : %s, current id : %s"), *string2Fstring(inMsg.id.GetValue()),*string2Fstring(current_id_));
			return;
		}
		if (!inMsg.result) {
			UE_LOG(LogTemp, Error, TEXT("Fail to Call Service check if service exists"));
			return;
		}
		string_resp = fstring2string(msg);
		UE_LOG(LogTemp, Warning, TEXT("unlock : %s"), *msg);
		//U_Event_service->Trigger();
		//Event.Trigger();
		
		return;

	}

	FCriticalSection UmutexService;
	FEvent* U_Event_service;
	UE::Tasks::FTaskEvent Event{ UE_SOURCE_LOCATION };


	template<typename Request, typename Response>
	void callTask(const Request & req, Response & res) {
		long t = time(NULL);
		UE_LOG(LogTemp, Warning, TEXT("Before send %d "), t);
		UE::Tasks::FTask sendTask = UE::Tasks::Launch(TEXT(""), [this, &req, &res] {
			UE_LOG(LogTemp, Warning, TEXT("send!"));
			call<Request, Response>(req, res);
			//Event.Wait();
			FPlatformProcess::Sleep(5);
		});
		sendTask.BusyWait(FTimespan::FromSeconds(10));
		if (!string_resp.empty()) {
			nlohmann::json jres = nlohmann::json::parse(string_resp);
			serviceResponse_t<Response> inMsg = jres.get<serviceResponse_t<Response>>();
			res = inMsg.values.GetValue();

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Empty response imposible to parse"));

		}
		//.BusyWait();
		UE_LOG(LogTemp, Warning, TEXT("after send %d "), time(NULL));
		UE_LOG(LogTemp, Warning, TEXT("wait time  %d "), time(NULL) - t);
	}


	void task() {
		long t = time(NULL);
		UE_LOG(LogTemp, Warning, TEXT("Before send %d "),t);
		bool plop = false;
		bool& plop_ref = plop;
		socket_->send(Event).BusyWait(FTimespan::FromSeconds(10));
		UE_LOG(LogTemp, Warning, TEXT("after send %d "), time(NULL));
		UE_LOG(LogTemp, Warning, TEXT("wait time  %d "), time(NULL)-t);
	}

	void unlock_task() {
		Event.Trigger();
	}


private:

	FString stored_service_name = "";
	std::string string_resp;
	UWebSocket* socket_;
	std::mutex syncMsg;
	std::mutex lock_call_;
	std::condition_variable cv;
	bool bAdvertised = false;
	bool bInitialized = false;
	long id = 0;
	std::string current_id_;




	std::string getNextId() {
		return std::to_string(id++);
	}



};

