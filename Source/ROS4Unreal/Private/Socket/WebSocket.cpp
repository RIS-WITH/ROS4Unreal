// Fill out your copyright notice in the Description page of Project Settings.

#include <ctime>
#include "Socket/WebSocket.h"
#include "WebSocketsModule.h"
#include "ROS4UnrealSettings.h"
#include "ROS4Unreal.h"


bool UWebSocket::initialize()
{
	U_Semaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);

	FPlatformProcess::Sleep(1);
	const UROS4UnrealSettings * settings_ = FROS4UnrealModule::Get().GetSettings();
	FString IPAddress = settings_->IP;
	int Port = settings_->Port;

	FString ConnectMsg = FString::Printf(TEXT("ws://%s:%d/"), *IPAddress, Port);
	UE_LOG(LogTemp, Error, TEXT("Init --- : %s Test"), *ConnectMsg);
	socket_ = FWebSocketsModule::Get().CreateWebSocket(FString::Printf(TEXT("ws://%s:%d/"), *IPAddress, Port), TEXT("ws"));

	socket_->OnClosed().AddLambda([this](int32 statusCode, const FString&  Reason , bool  bWasClean )->void {
		UE_LOG(LogTemp, Error, TEXT("Socket close: %s"), *Reason);
		});
	
	socket_->OnConnectionError().AddLambda([this](const FString& Error) -> void {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		UE_LOG(LogTemp, Error, TEXT("Socket Error Occured: %s"), *Error);
		});

	socket_->OnMessageSent().AddLambda([this](const FString& msg)->void {
		UE_LOG(LogTemp, Warning, TEXT("Message Sent: %s, %d "), *msg,time(NULL));
		return;
		});
	
	socket_->OnMessage().AddLambda([this](const FString& msg)-> void {
		UE_LOG(LogTemp, Warning, TEXT("Message received: %s, %d "), *msg, time(NULL));
		return; // To init Event on OnMessage;
		});
	
	
	socket_->OnConnected().AddLambda([this]()->void {
		AsyncTask(ENamedThreads::GameThread, [this]() {
			if (U_Semaphore) {
				UE_LOG(LogTemp, Warning, TEXT("Semaphore exist"));
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("Semaphore not exist"));

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Connect ok");
			UE_LOG(LogTemp, Error, TEXT("Connect ok %d"), time(NULL));
			U_Semaphore->Trigger();
			});
		});

	
	socket_->Connect();
	
	FTimespan time_wait = FTimespan().FromMilliseconds(10);
	int compt = 10000 / 10;
	while (compt >= 0)
	{
		if (!U_Semaphore->Wait(time_wait)) {
			UE_LOG(LogTemp, Warning, TEXT("down compt %d"),time(NULL));
			compt--;
		}
		else
			break;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Compt : %d : %d"), compt, time(NULL));
	UE_LOG(LogTemp, Warning, TEXT("Connected : %d "),  socket_->IsConnected());

	if (compt < 0) {
		UE_LOG(LogTemp, Warning, TEXT("boucle timeout 10sec %d"), time(NULL));
		return false;
	}
	
	/*
	FTimespan time_wait = FTimespan().FromSeconds(10);
	if (!U_Semaphore->Wait(time_wait)) {
		UE_LOG(LogTemp, Warning, TEXT("TimeOut"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Unlock"));
	};
	*/
	return true;
}

void UWebSocket::uninitialize()
{
	socket_->Close();
}

bool UWebSocket::sendMessage(const FString& Data) const
{
	socket_->Send(Data);
	return true;
}

uint32 UWebSocket::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Thread run"));
	for (;;) {
		FPlatformProcess::Sleep(0.01);
	}
	return EXIT_SUCCESS;
}
