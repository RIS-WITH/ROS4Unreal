// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "WebSocket.generated.h"

/**
 * 
 */
UCLASS()
class UWebSocket : public UObject, public FRunnable
{
	GENERATED_BODY()
public : 
	bool initialize();

	void uninitialize();
	bool sendMessage(const FString& Data) const;
	TSharedPtr<IWebSocket> socket_ = nullptr;

	uint32 Run() override;

private:
	FRunnableThread* Thread = nullptr;

	
};
