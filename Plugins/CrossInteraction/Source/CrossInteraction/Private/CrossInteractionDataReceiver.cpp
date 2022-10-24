#include "CrossInteractionDataReceiver.h"
ACrossInteractionDataReceiver::ACrossInteractionDataReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ListenSocket = NULL;
	is_running = true;
	RecvBuffer.SetNumUninitialized(1024);
	connected = false;
	toSet = false;
	
	curIP = "";
	curStartID = 0;
	curEndID = 0;
	curPortalType = 0;
	curPortalIP = "";
	curPortalPort = 0;
	curPort = 0;
	curBillboardUrl = "";
	curBillboardType = 0;
	curBillboardName = "";
	curMsgType = -1;
	curEffectID = 0;
	curEffectStatus = 0;
	curTYQID = 0;
	curDecalUrl = 0;
	curBillboardStatus = 0;
	curEditMode = false;
	curSkillID = -1;
	curEditModeCamID = 0;
	curBillboardInstSet = false;
}

bool ACrossInteractionDataReceiver::StartCrossInteractionReceiver(const FString& SocketName, const int32 Port)
{
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	msgRecvThread = std::thread(std::bind(&ACrossInteractionDataReceiver::msgRecving, this));
	return true;
}

void ACrossInteractionDataReceiver::msgRecving()
{
	while (is_running)
	{
		int32 Read = 0;
		ListenSocket->RecvFrom(RecvBuffer.GetData(), RecvBuffer.Num(), Read, *Sender);
		if (Read > 0)
		{
			connected = true;
			TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData = MakeShareable(new TArray<uint8>());
			ReceivedData->SetNumUninitialized(Read);
			memcpy(ReceivedData->GetData(), RecvBuffer.GetData(), Read);
			parseMsg(ReceivedData);

		}
	}
}

void ACrossInteractionDataReceiver::parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData)
{
	FString JsonString;
	JsonString.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		JsonString += TCHAR(Byte);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, JsonString);
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	bool flag = FJsonSerializer::Deserialize(Reader, JsonObject);
	if (flag)  // StringתJSON
	{
		int type;
		int ID;
		FString str;
		int port;
		int status;
		bool mode;
		if (JsonObject->TryGetNumberField(TEXT("CI-PORTAL-TYPE"), type))
		{
			curPortalType = type;
			curMsgType = 0;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-PORTAL-END"), ID))
		{
			curEndID = ID;
		}
		if (JsonObject->TryGetStringField(TEXT("CI-PORTAL-IP"), str))
		{
			curPortalIP = str;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-PORTAL-PORT"), port))
		{
			curPortalPort = port;
		}
		if (JsonObject->TryGetStringField(TEXT("CI-SERVER-IP"), str))
		{
			curIP = str;
			curMsgType = 1;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-SERVER-PORT"), port))
		{
			curPort = port;
		}
		if (JsonObject->TryGetStringField(TEXT("CI-BILLBOARD-NAME"), str))
		{
			curMsgType = 2;
			curBillboardName = str;
		}
		if (JsonObject->TryGetStringField(TEXT("CI-BILLBOARD-URL"), str))
		{
			curBillboardUrl = str;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-BILLBOARD-TYPE"), type))
		{
			curBillboardType = type;
		}
		if (JsonObject->TryGetBoolField(TEXT("CI-BILLBOARD-INSTSET"), mode))
		{
			curBillboardInstSet = mode;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-EFFECT-ID"), ID))
		{
			curMsgType = 3;
			curEffectID = ID;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-EFFECT-CONTROL"), status))
		{
			curEffectStatus = status;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-TYQ-ID"), ID))
		{
			curMsgType = 4;
			curTYQID = ID;
		}
		if (JsonObject->TryGetStringField(TEXT("CI-DECAL-URL"), str))
		{
			curDecalUrl = str;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-BILLBOARD-STATUS"), ID))
		{
			curMsgType = 5;
			curBillboardStatus = ID;
		}
		if (JsonObject->TryGetBoolField(TEXT("CI-EDITMODE"), mode))
		{
			curMsgType = 6;
			curEditMode = mode;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-SKILL-ID"), ID))
		{
			curMsgType = 7;
			curSkillID = ID;
		}
		if (JsonObject->TryGetNumberField(TEXT("CI-EDITMODE-CAM-ID"), ID))
		{
			curMsgType = 8;
			curEditModeCamID = ID;
		}

		if (curMsgType == 2)
		{
			FCIBillboardProp tmp;
			tmp.name = curBillboardName;
			tmp.type = curBillboardType;
			tmp.url = curBillboardUrl;
			curBillboardProps.Add(tmp);
		}
		else
		{
			toSet = true;
		}
	}
}

FCIMsgData ACrossInteractionDataReceiver::GetData()
{
	FCIMsgData data;
	data.msgType = curMsgType;
	data.startID = curStartID;
	data.endID = curEndID;
	data.portalType = curPortalType;
	data.portalIp = curPortalIP;
	data.portalPort = curPortalPort;
	data.ip = curIP;
	data.port = curPort;
	data.billboardType = curBillboardType;
	data.billboardUrl = curBillboardUrl;
	data.billboardName = curBillboardName;
	data.effectID = curEffectID;
	data.effectStatus = curEffectStatus;
	data.TYQID = curTYQID;
	data.decalUrl = curDecalUrl;
	data.billboardStatus = curBillboardStatus;
	data.billboardProps = curBillboardProps;
	data.editMode = curEditMode;
	data.skillID = curSkillID;
	data.editModeCamID = curEditModeCamID;
	data.billboardInstSet = curBillboardInstSet;
	toSet = false;
	return data;
}

bool ACrossInteractionDataReceiver::NewData()
{
	return toSet;
}

bool ACrossInteractionDataReceiver::Connected()
{
	return connected;
}

void ACrossInteractionDataReceiver::SendPortalMsg(TArray<FCIPortalProp> PortalProps, const int32 count)
{
	if (connected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending: ID: %d, name: %s, type: %d"), ID, *name.ToString(), type));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-PORTAL-COUNT"), count);

		TArray<TSharedPtr<FJsonValue>> portalProperties;
		for (int i = 0; i < count; i++)
		{
			TSharedPtr<FJsonObject> portal = MakeShareable(new FJsonObject);
			portal->SetNumberField("ID", PortalProps[i].ID);
			portal->SetStringField("Name", PortalProps[i].name.ToString());
			portal->SetNumberField("type", PortalProps[i].type);
			portalProperties.Add(MakeShareable(new FJsonValueObject(portal)));
		}
		jsonObject->SetArrayField("CI-PORTAL-PROPERTY", portalProperties);
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

void ACrossInteractionDataReceiver::SendBillboardMsg(const int32 ID, FName name)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : billiboard ID: %d, name: %s"), ID, *name.ToString()));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetStringField(TEXT("CI-BILLIBOARD-NAME"), TCHAR_TO_UTF8(*name.ToString()));
		jsonObject->SetNumberField(TEXT("CI-BILLIBOARD-ID"), ID);
		jsonObject->SetNumberField(TEXT("type"), 6);

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

void ACrossInteractionDataReceiver::SendEffectMsg(TArray<FCIEffectProp> effectProps, const int32 count)
{
	if (connected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : effect ID: %d, name: %s"), ID, *name.ToString()));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-EFFECT-COUNT"), count);
		jsonObject->SetNumberField(TEXT("type"), 8);
		
		TArray<TSharedPtr<FJsonValue>> effectProperties;
		for (int i = 0; i < count; i++)
		{
			TSharedPtr<FJsonObject> effect = MakeShareable(new FJsonObject);
			effect->SetNumberField("ID", effectProps[i].ID);
			effect->SetStringField("Name", effectProps[i].name.ToString());
			effectProperties.Add(MakeShareable(new FJsonValueObject(effect)));
		}
		jsonObject->SetArrayField("CI-EFFECT-PROPERTY", effectProperties);

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

void ACrossInteractionDataReceiver::SendEffectNum(const int32 count)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : effect num: %d"), count));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-EFFECT-NUM"), count);
		jsonObject->SetNumberField(TEXT("type"), 8);

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

void ACrossInteractionDataReceiver::SendTYQMsg(TArray<FCITYQProp> TYQProps, const int32 count)
{
	if (connected)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : TYQ ID: %d, name: %s"), ID, *name.ToString()));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-TYQ-COUNT"), count);

		TArray<TSharedPtr<FJsonValue>> TYQProperties;
		for (int i = 0; i < count; i++)
		{
			TSharedPtr<FJsonObject> TYQ = MakeShareable(new FJsonObject);
			TYQ->SetNumberField("ID", TYQProps[i].ID);
			TYQ->SetStringField("Name", TYQProps[i].name.ToString());
			TYQProperties.Add(MakeShareable(new FJsonValueObject(TYQ)));
		}
		jsonObject->SetArrayField("CI-TYQ-PROPERTY", TYQProperties);

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

void ACrossInteractionDataReceiver::SendTYQStatus(const int32 ID)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : TYQ ID: %d"), ID));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-TYQ-STATUS-ID"), ID);

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

void ACrossInteractionDataReceiver::SendTYQFull()
{
	if (connected)
	{	
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-TYQ-FULL"), 1);

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

void ACrossInteractionDataReceiver::SendDecalMsg(const int32 ID, FName name)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : effect ID: %d, name: %s"), ID, *name.ToString()));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetStringField(TEXT("CI-DECAL-NAME"), TCHAR_TO_UTF8(*name.ToString()));
		jsonObject->SetNumberField(TEXT("CI-DECAL-ID"), ID);

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

void ACrossInteractionDataReceiver::SendOverlapMsg(const int32 ID)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending: trigger ID: %d"), ID));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI_PORTAL-TRIGGER-ID"), ID);

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

void ACrossInteractionDataReceiver::SendEditModeMsg()
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("CI-EDITMODE-SET")));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		jsonObject->SetNumberField(TEXT("CI-EDITMODE-SET"), 1);

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

void ACrossInteractionDataReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
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