// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <Runtime/Core/Public/Tasks/Task.h>
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

	UE::Tasks::FTask send(UE::Tasks::FTaskEvent Event) {

		return UE::Tasks::Launch(UE_SOURCE_LOCATION, [this,&Event] { 
			UE_LOG(LogTemp, Warning, TEXT("Hello Tasks! wait ")); 
			for (int k = 0; k < 10; k++) {
				UE_LOG(LogTemp, Warning, TEXT("Hello Tasks! wait %d : time %d"),k,time(NULL));
				FPlatformProcess::Sleep(0.2);
			}
			complet = true;
			Event.Wait();
			//FPlatformProcess::Sleep(2);
			
		});

	};

	bool complet = false;
	
	

private:
	FRunnableThread* Thread = nullptr;
	FCriticalSection Umutex;
	FEvent* U_Semaphore;

	
};
