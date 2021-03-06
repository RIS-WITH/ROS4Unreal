// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <mutex>
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "WebSocket.generated.h"

/**
 * 
 */
UCLASS()
class ROS4UNREAL_API UWebSocket : public UObject, public FRunnable
{
	GENERATED_BODY()
public : 
	UWebSocket();
	bool initialize();
	void uninitialize();
	bool sendMessage(const FString& Data) const;
	TSharedPtr<IWebSocket> socket_ = nullptr;
	uint32 Run() override;

	std::condition_variable cv_wait_connect;
	std::mutex waitConnected;

private:
	FRunnableThread* Thread = nullptr;
	FCriticalSection Umutex;
	FEvent* U_Semaphore;

	
};
