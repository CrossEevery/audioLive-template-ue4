#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Json.h"
#include "Networking.h"
#include "UDPData.h"
#include <thread>
#include <iostream>
#include <functional>
#include "UDPReceiver.generated.h"

#define RECV_BUFFER_SIZE 1024*12

UCLASS()
class UDPCOMMUNICATION_API AUDPReceiver : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	FUDPData MyData;
	TArray<FUDPBoneData> MyBodyData;
	bool UpdateInterest = true;
	bool got_new_data = false;

	bool UpdateBodyInterest = true;	// Ready for new data
	bool got_new_body_data = false;
	  
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UDPCommunication")
		void BPEvent_DataReceived(const FUDPData& data);
public:
	
	FSocket* ListenSocket = NULL;
	FUdpSocketReceiver* Receiver = nullptr;
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	virtual void Archive(const FArrayReaderPtr & ArrayReaderPtr);
	virtual void UpdateReceiverData(FUDPData data);
	
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		FUDPData GetData();
		
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool IsNewDataReady();
	
	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool StartUDPReceiver(const FString& SocketName,
			const int32 Port, const int32 BroadcastMode);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool StartUDPReceiverBody(const FString& SocketName,
			const int32 Port);

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		bool IsNewBodyDataReady();

	UFUNCTION(BlueprintCallable, Category = "UDPCommunication")
		TArray<FUDPBoneData> GetBodyData();

	FSocket* ListenSocketBody = NULL;
public:

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	int ConnectTCPServer(FString ip, int32 Port, int mode);
	void SendDataTCP(uint8* buffer, int32 len, int mode);
	void SendDataTCP(FUDPData data);
	void tcpRecving();
	void bodyRecving();
	void tcpBodyRecving();
	void reconnect();
	void HandleBodyData(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData, int mode);
	void UpdateBodyData(TArray<FUDPBoneData> bodyData);
	void SendBodyDataTCP(uint8* buffer, int32 len);
	void SendBodyDataTCP2(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData, int32 len);
	// Socket to receive data on
	FSocket* SocketTCP = NULL;
	FSocket* SocketTCPBody = NULL;
	// Subsystem associated to Socket
	ISocketSubsystem* SocketSubsystem;

	int broadcastMode;
	bool useData;

	TArray<FName> BoneNamesCur;
	int64 numPosesCur;
	TArray<uint8> RecvBuffer;
	TArray<uint8> BodyRecvBuffer;
	std::thread tcpRecvThread;
	std::thread tcpBodyRecvThread;
	std::thread bodyRecvThread;
	std::thread reconnectThread;
	FTimespan WaitTime;
	std::string uid;
	FString TcpServerIp;
	int32 TcpPort;
	bool is_running;
	int32 printLog;
	bool doReconnect;
};