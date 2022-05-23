// Fill out your copyright notice in the Description page of Project Settings.


#include "Socket/WebSocket.h"
#include "WebSocketsModule.h"
#include "ROS4UnrealSettings.h"
#include "ROS4Unreal.h"

bool UWebSocket::initialize()
{
	const UROS4UnrealSettings * settings_ = FROS4UnrealModule::Get().GetSettings();
	FString IPAddress = settings_->IP;
	int Port = settings_->Port;

	FString ConnectMsg = FString::Printf(TEXT("ws://%s:%d/"), *IPAddress, Port);
	UE_LOG(LogTemp, Error, TEXT("Init --- : %s"), *ConnectMsg);
	socket_ = FWebSocketsModule::Get().CreateWebSocket(FString::Printf(TEXT("ws://%s:%d/"), *IPAddress, Port), TEXT("ws"));

	socket_->OnConnectionError().AddLambda([this](const FString& Error) -> void {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Error);
		UE_LOG(LogTemp, Error, TEXT("Socket Error Occured: %s"), *Error);
		});

	socket_->OnMessageSent().AddLambda([this](const FString& msg)->void {
		return;
		});
	
	socket_->OnMessage().AddLambda([this](const FString& msg)-> void {
		return; // To init Event on OnMessage;
		});
	

	socket_->OnConnected().AddLambda([this]()->void {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Connect ok");
		UE_LOG(LogTemp, Error, TEXT("Connect ok "));
		});


	socket_->Connect();
	Thread = FRunnableThread::Create(this, TEXT("This is my thread example"));

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
	while (1) {

	}
	return EXIT_SUCCESS;
}
