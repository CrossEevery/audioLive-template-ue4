#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Networking.h"
#include "Serialization/Archive.h"
#include "Json.h"
#include <thread>
#include <functional>
#include "AIHumanCommReceiver.generated.h"

UCLASS()
class AIHUMANCOMM_API AAIHumanCommReceiver : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "AIHumanCommReceiver")
		void BPEvent_DataReceived();

	UFUNCTION(BlueprintCallable, Category = "AIHumanCommReceiver")
		FString GetData();

	UFUNCTION(BlueprintCallable, Category = "AIHumanCommReceiver")
		bool NewData();

	UFUNCTION(BlueprintCallable, Category = "AIHumanCommReceiver")
		bool StartAIHumanDataCommReceiver(const FString& SocketName, const int32 Port);

	UFUNCTION(BlueprintCallable, Category = "AIHumanCommReceiver")
		bool Connected();

	UFUNCTION(BlueprintCallable, Category = "AIHumanCommReceiver")
		void SendMsg(const int32 count);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void msgRecving();
	void parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData);

	FSocket* ListenSocket;
	std::thread msgRecvThread;
	bool is_running;
	TArray<uint8> RecvBuffer;
	bool connected;
	bool toSet;
	FString curMsg;
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
};