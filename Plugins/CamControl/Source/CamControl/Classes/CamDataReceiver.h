#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Networking.h"
#include "Serialization/Archive.h"
#include "Json.h"
#include <thread>
#include <functional>
#include <iostream>
#include "CamDataReceiver.generated.h"

UCLASS()
class CAMCONTROL_API ACamDataReceiver : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CamControl")
		void BPEvent_DataReceived();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		FTransform GetData();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		bool StartCamReceiver(const FString& SocketName,
			const int32 Port);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		int SetCam();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		int SetControllerCam(const int32 new_cam_type);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		int SetOrient();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		float SetSpeed();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		float SetFocus();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SetInitCam(const int32 init_cam_type);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SetInitCamOrient(const int32 init_cam_orient);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		bool InitSet();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		bool IsOffsetSet();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		bool Connected();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SendMsg(TArray<int32> IDs, TArray<FName> names, const int32 count, const int32 defaultID);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SendCamNum(const int32 count);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SendInit();

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SendLoc(FVector loc);

	UFUNCTION(BlueprintCallable, Category = "CamControl")
		void SendDefault(const int32 ID);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void msgRecving();
	void parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData);
	int SetOffset();

	FSocket* ListenSocket;
	std::thread msgRecvThread;
	bool is_running;
	//ISocketSubsystem* SocketSubsystem;
	TArray<uint8> RecvBuffer;
	int cam_type;
	int cam_orient;
	float cam_speed;
	FTransform trackcam_offset;
	FTransform abs_offset;
	FTransform initOffset;
	bool isset;
	bool focusreset;
	float deltaFocus;
	float angle_prev;
	float angle;
	bool firstFrame;
	bool connected;
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
};