#include "AIHumanCommReceiver.h"
AAIHumanCommReceiver::AAIHumanCommReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ListenSocket = NULL;
	is_running = true;
	RecvBuffer.SetNumUninitialized(1024);
	connected = false;
	toSet = false;

}

bool AAIHumanCommReceiver::StartAIHumanDataCommReceiver(const FString& SocketName, const int32 Port)
{
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	msgRecvThread = std::thread(std::bind(&AAIHumanCommReceiver::msgRecving, this));
	return true;
}

void AAIHumanCommReceiver::msgRecving()
{
	while (is_running)
	{
		int32 Read = 0;
		ListenSocket->RecvFrom(RecvBuffer.GetData(), RecvBuffer.Num(), Read, *Sender);
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("Read: %d"), Read));
		if (Read > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("Read: %d"), Read));
			connected = true;
			TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData = MakeShareable(new TArray<uint8>());
			ReceivedData->SetNumUninitialized(Read);
			memcpy(ReceivedData->GetData(), RecvBuffer.GetData(), Read);
			parseMsg(ReceivedData);

		}
	}
}

void AAIHumanCommReceiver::parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData)
{
	FString msg;
	msg.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		msg += TCHAR(Byte);
	}
	GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, msg);
	curMsg = msg;
	toSet = true;
}

FString AAIHumanCommReceiver::GetData()
{
	toSet = false;
	return curMsg;
}

bool AAIHumanCommReceiver::NewData()
{
	return toSet;
}

bool AAIHumanCommReceiver::Connected()
{
	return connected;
}

void AAIHumanCommReceiver::SendMsg(const int32 count)
{
	if (connected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending: ID: %d, name: %s, type: %d"), ID, *name.ToString(), type));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);

		FString jsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&jsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, jsonStr);
		int len = jsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = jsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out;
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("send to ip: %d, port %d, ret: %d, "), out, port, ret));
	}
}

void AAIHumanCommReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	is_running = false;
	msgRecvThread.join();

	//Clear all sockets
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}