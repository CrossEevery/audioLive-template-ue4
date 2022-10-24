#include "CamDataReceiver.h"
#define ANGEL2FOCUS 0.33
ACamDataReceiver::ACamDataReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ListenSocket = NULL;
	is_running = true;
	RecvBuffer.SetNumUninitialized(1024);
	cam_type = -1;
	cam_orient = -1;
	cam_speed = 1.0;
	isset = true;
	deltaFocus = 0;
	angle_prev = -9999;
	angle = -9999;
	firstFrame = true;
	focusreset = true;

	connected = false;
}

bool ACamDataReceiver::StartCamReceiver(const FString& SocketName, const int32 Port)
{
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = 2 * 1024 * 1024;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	//SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	//TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	msgRecvThread = std::thread(std::bind(&ACamDataReceiver::msgRecving, this));
	return true;
}

void ACamDataReceiver::msgRecving()
{
	//TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
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
			parseMsg(ReceivedData);  // 处理躯干数据

		}
	}
}

void ACamDataReceiver::parseMsg(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData)
{
	FString JsonString;
	JsonString.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		JsonString += TCHAR(Byte);
	}


	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	bool flag = FJsonSerializer::Deserialize(Reader, JsonObject);
	if (flag)  // String转JSON
	{
		int value;
		double speed;
		const TArray<TSharedPtr<FJsonValue>>* tmp;
		if (JsonObject->TryGetNumberField(TEXT("CAM-TYPE"), value))
		{
			cam_type = value;
		}
		else if (JsonObject->TryGetNumberField(TEXT("CAM-ORIENT"), value))
		{
			cam_orient = value;
		}
		else if (JsonObject->TryGetNumberField(TEXT("CAM-SPEED"), speed))
		{
			cam_speed = speed;
		}
		else if (JsonObject->TryGetNumberField(TEXT("TRACKCAM-SET"), value))
		{
			SetOffset();
		}
		else if (JsonObject->TryGetNumberField(TEXT("TRACKCAM-FOCUS-SET"), value))
		{
			if (value == 1)
			{
				focusreset = true;
			}
			if (value == 0)
			{
				deltaFocus = 1;
			}
		}
		else if (JsonObject->TryGetArrayField(TEXT("TRACKCAM-OFFSET"), tmp))
		{
			TArray<TSharedPtr<FJsonValue>> offset = JsonObject->GetArrayField(TEXT("TRACKCAM-OFFSET"));
			FVector BoneLocation;
			BoneLocation = FVector(offset[0]->AsNumber() - initOffset.GetLocation().X, offset[1]->AsNumber() - initOffset.GetLocation().Y, offset[2]->AsNumber() - initOffset.GetLocation().Z);
			FRotator BoneQuat;
			FQuat q1(offset[3]->AsNumber(), offset[4]->AsNumber(), offset[5]->AsNumber(), offset[6]->AsNumber());
			FQuat q2(initOffset.GetRotation().X, initOffset.GetRotation().Y, initOffset.GetRotation().Z, initOffset.GetRotation().W);
			FQuat q3 = q1 * q2.Inverse();
			BoneQuat = q3.Rotator();
			FVector BoneScale;
			BoneScale = FVector(1.0, 1.0, 1.0);
			trackcam_offset = FTransform(BoneQuat, BoneLocation, BoneScale);
			abs_offset = FTransform(q1.Rotator(), FVector(offset[0]->AsNumber(), offset[1]->AsNumber(), offset[2]->AsNumber()), BoneScale);
			if (firstFrame)
			{
				SetOffset();
				firstFrame = false;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("log %f, %f, %f,%f"), trackcam_offset.GetRotation().X, trackcam_offset.GetRotation().Y, trackcam_offset.GetRotation().Z, trackcam_offset.GetRotation().W));
		}
		else if (JsonObject->TryGetNumberField(TEXT("TRACKCAM-FOCUS"), value))
		{
			if (focusreset)
			{
				angle_prev = value;
				focusreset = false;
			}
			else
			{
				if (angle_prev == -9999)
				{
					angle_prev = value;
				}
				else
				{
					angle = value;

					if (angle - angle_prev < -300)
					{
						deltaFocus = (angle + 360 - angle_prev) * ANGEL2FOCUS;
					}
					else if (angle - angle_prev > 300)
					{
						deltaFocus = (angle - 360 - angle_prev) * ANGEL2FOCUS;
					}
					else
					{
						deltaFocus = (angle - angle_prev) * ANGEL2FOCUS;
					}
					angle_prev = angle;
				}
			}
		}
	}
}

FTransform ACamDataReceiver::GetData()
{
	return trackcam_offset;
}

int ACamDataReceiver::SetCam()
{
	return cam_type;
}

int ACamDataReceiver::SetControllerCam(const int32 new_cam_type)
{
	cam_type = new_cam_type;
	return cam_type;
}

int ACamDataReceiver::SetOrient()
{
	return cam_orient;
}

float ACamDataReceiver::SetSpeed()
{
	return cam_speed;
}

int ACamDataReceiver::SetOffset()
{
	initOffset = abs_offset;
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("log %f, %f, %f,%f"), initOffset.GetRotation().X, initOffset.GetRotation().Y, initOffset.GetRotation().Z, initOffset.GetRotation().W));
	isset = true;
	return 0;
}

float ACamDataReceiver::SetFocus()
{
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("log %f"), deltaFocus));
	return deltaFocus;
}

bool ACamDataReceiver::IsOffsetSet()
{
	return isset;
}

void ACamDataReceiver::SetInitCam(const int32 init_cam_type)
{
	cam_type = init_cam_type;
}

void ACamDataReceiver::SetInitCamOrient(const int32 init_cam_orient)
{
	cam_orient = init_cam_orient;
}

bool ACamDataReceiver::InitSet()
{
	if (cam_type >= 0 && cam_orient >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ACamDataReceiver::Connected()
{
	return connected;
}

void ACamDataReceiver::SendMsg(TArray<int32> IDs, TArray<FName> names, const int32 count, const int32 defaultID)
{
	if (connected)
	{
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		jsonObject->SetNumberField("type", 5);
		jsonObject->SetNumberField("CAM-DEFAULT", defaultID);
		jsonObject->SetNumberField("CAM-COUNT", count);
		TArray<TSharedPtr<FJsonValue>> camProperties;
		for (int i = 0; i < count; i++)
		{
			TSharedPtr<FJsonObject> cam = MakeShareable(new FJsonObject);
			cam->SetNumberField("ID", IDs[i]);
			cam->SetStringField("Name", names[i].ToString());
			camProperties.Add(MakeShareable(new FJsonValueObject(cam)));
		}
		jsonObject->SetArrayField("CAM-PROPERTY", camProperties);
		FString JsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

		int len = JsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = JsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out; 
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, JsonStr);
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("send to ip: %d, port %d, ret: %d, "), out, port, ret));
	}
}

void ACamDataReceiver::SendCamNum(const int32 count)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending: count: %d"), count));

		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		jsonObject->SetNumberField("CAM-NUM", count);
		jsonObject->SetNumberField("type", 5);
		FString JsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

		int len = JsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = JsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out;
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("send to ip: %d, port %d, ret: %d, "), out, port, ret));
	}
}

void ACamDataReceiver::SendDefault(const int32 ID)
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("connected and sending : default ID: %d"), ID));

		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		jsonObject->SetNumberField("CAM-DEFAULT", ID);
		FString JsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

		//FString msg = "CAM-DEFAULT:" + FString::FromInt(ID);
		int len = JsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = JsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out;
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("send to ip: %d, port %d, ret: %d, "), out, port, ret));
	}
}

void ACamDataReceiver::SendInit()
{
	if (connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red,"sending CAM-INIT");

		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		jsonObject->SetNumberField("CAM-DEFAULT", 1);
		FString JsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);

		int len = JsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = JsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out;
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("send to ip: %d, port %d, ret: %d, "), out, port, ret));
	}
}

void ACamDataReceiver::SendLoc(FVector loc)
{
	if (connected)
	{	
		double x = loc.X;
		double y = loc.Y;
		double z = loc.Z;

		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f, z: %f"), x, y, z));
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		TArray< TSharedPtr<FJsonValue> > data;
		data.Add(MakeShareable(new FJsonValueNumber(x)));
		data.Add(MakeShareable(new FJsonValueNumber(y)));
		data.Add(MakeShareable(new FJsonValueNumber(z)));

		jsonObject->SetArrayField("TRACKCAM-POS", data);
		FString JsonStr = TEXT("");
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), Writer);
		int len = JsonStr.Len();
		int32 sent = 0;
		TCHAR* SendMessage = JsonStr.GetCharArray().GetData();
		int ret = ListenSocket->SendTo((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent, *Sender);
		uint32 out;
		Sender->GetIp(out);
		int port = Sender->GetPort();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, JsonStr);
	}
}

void ACamDataReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
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