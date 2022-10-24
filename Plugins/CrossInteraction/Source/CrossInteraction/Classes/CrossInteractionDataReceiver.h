#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Networking.h"
#include "Serialization/Archive.h"
#include "Json.h"
#include <thread>
#include <functional>
#include <iostream>
#include "CrossInteractionData.h"
#include "CrossInteractionDataReceiver.generated.h"

UCLASS()
class CROSSINTERACTION_API ACrossInteractionDataReceiver : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CrossInteraction")
		void BPEvent_DataReceived();

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		FCIMsgData GetData();

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		bool NewData();

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		bool StartCrossInteractionReceiver(const FString& SocketName,
			const int32 Port);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		bool Connected();

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendPortalMsg(TArray<FCIPortalProp> portalProps, const int32 count);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendOverlapMsg(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendBillboardMsg(const int32 ID, FName name);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendEffectMsg(TArray<FCIEffectProp> effectProps, const int32 count);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendEffectNum(const int32 count);


	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendTYQMsg(TArray<FCITYQProp> TYQProps, const int32 count);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendTYQFull();


	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendTYQStatus(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendDecalMsg(const int32 ID, FName name);
	
	UFUNCTION(BlueprintCallable, Category = "CrossInteraction")
		void SendEditModeMsg();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void msgRecving();
	void parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData);

	FSocket* ListenSocket;
	std::thread msgRecvThread;
	bool is_running;
	TArray<uint8> RecvBuffer;
	bool connected;
	bool toSet;
	int curPortalType;
	FString curPortalIP;
	int curPortalPort;
	int curStartID;
	int curEndID;
	FString curIP;
	int curMsgType;
	int curPort;
	FString curBillboardName;
	FString curBillboardUrl;
	int curBillboardType;
	int curBillboardStatus;
	int curEffectID;
	int curEffectStatus;
	int curTYQID;
	bool curEditMode;
	int curSkillID;
	FString curDecalUrl;
	int curEditModeCamID;
	bool curBillboardInstSet;
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	TArray<FCIBillboardProp> curBillboardProps;
};