#include "UDPReceiver.h"
#include <string>
#include<sstream>
#include <fstream> 
#include <iostream>



AUDPReceiver::AUDPReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FString configDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir());
	std::string path(TCHAR_TO_UTF8(*configDir));
	std::string uidFile = path + "uid.txt";
	std::ifstream cfile(uidFile);
	getline(cfile, uid);
	cfile.close();

	std::string tcpFile = path + "tcp.txt";
	std::ifstream tfile(tcpFile);
	std::string sTcpServerIp;
	std::string sTcpPort;
	getline(tfile, sTcpServerIp);
	getline(tfile, sTcpPort);
	TcpServerIp = FString(sTcpServerIp.c_str());
	TcpPort = atoi(sTcpPort.c_str());
	tfile.close();

	std::string logFile = path + "logSwitch.txt";
	std::ifstream lfile(logFile);
	std::string logSwitch;
	getline(lfile, logSwitch);
	printLog = atoi(logSwitch.c_str());
	lfile.close();


	ListenSocket = NULL;
	broadcastMode = 0;
	RecvBuffer.SetNumUninitialized(RECV_BUFFER_SIZE);
	BodyRecvBuffer.SetNumUninitialized(RECV_BUFFER_SIZE);
	is_running = true;
	WaitTime = FTimespan::FromMilliseconds(100);
	doReconnect = false;
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	delete Receiver;
	Receiver = nullptr;
	is_running = false;

	if (broadcastMode != 1)
	{
		bodyRecvThread.join();
	}
	else
	{
		tcpRecvThread.join();
		tcpBodyRecvThread.join();
	}

	//Clear all sockets
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}

	if (ListenSocketBody)
	{
		ListenSocketBody->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocketBody);
	}

	if (SocketTCP)
	{
		SocketTCP->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SocketTCP);
	}

	if (SocketTCPBody)
	{
		SocketTCPBody->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SocketTCPBody);
	}
}

bool AUDPReceiver::StartUDPReceiver(const FString& SocketName, const int32 Port, const int32 BroadcastMode)
{
	broadcastMode = BroadcastMode;
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = RECV_BUFFER_SIZE;
	ListenSocket = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.AsBlocking()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (broadcastMode != 2)
	{
		if (ConnectTCPServer(TcpServerIp, TcpPort, 0) != 0)
		{
			doReconnect = true;
		}
		if (printLog)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, FString::Printf(TEXT("broadcastmode %d"), broadcastMode));
			if (!SocketTCP)
			{
				GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, TEXT("TCP socket create failed!"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, TEXT("face TCP socket created!"));
			}
		}
	}

	if (broadcastMode != 1)
	{
		FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
		Receiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP Receiver"));
		Receiver->OnDataReceived().BindUObject(this, &AUDPReceiver::Recv);
		Receiver->Start();
	}
	else
	{
		uint8* sendData = (uint8*)malloc(sizeof(uint8) * 1024);
		std::string str = uid + ":RECV:CHARACTER-FACE:NULL";
		memcpy(sendData, str.c_str(), str.length());
		SendDataTCP(sendData, str.length(), 0);
		FString msg(str.c_str());
		free(sendData);

		tcpRecvThread = std::thread(std::bind(&AUDPReceiver::tcpRecving, this));

		//reconnectThread = std::thread(std::bind(&AUDPReceiver::reconnect, this));
	}
	return true;
}

void AUDPReceiver::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	if (!&ArrayReaderPtr) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot read array, nullptr returned."));
		return;
	}

	got_new_data = true;

	if (UpdateInterest)
		Archive(ArrayReaderPtr);
}

void AUDPReceiver::Archive(const FArrayReaderPtr& ArrayReaderPtr) {

	FUDPData data;
	*ArrayReaderPtr << data;

	//Class Parameter Update function to set object ready for a query from blueprint
	UpdateReceiverData(data);
}

void AUDPReceiver::UpdateReceiverData(FUDPData data)
{
	if (broadcastMode == 0)
	{
		SendDataTCP(data);
	}

	MyData = data;
	;
}

FUDPData AUDPReceiver::GetData()
{
	UpdateInterest = true;	// Ready for new data
	got_new_data = false;
	return MyData;
}

bool AUDPReceiver::IsNewDataReady() {
	return got_new_data;
}

void AUDPReceiver::tcpRecving()
{
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	while (is_running)
	{
		int32 Read = 0;

		if (SocketTCP->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			if (SocketTCP->RecvFrom(RecvBuffer.GetData(), RecvBuffer.Num(), Read, *Sender))
			{
				if (printLog)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Recving face data, size %d"), Read));
				}
				if (Read > 0)
				{
					FString tmpRecv((char*)RecvBuffer.GetData());

					TArray<FString> recvData;
					tmpRecv.ParseIntoArray(recvData, TEXT(","), true);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Recving face data  num %d"), recvData.Num()));
					if (recvData.Num() > 57)
					{
						TArray<float> faceData;
						faceData.SetNum(57);
						for (int i = 0; i < 57; i++)
						{
							faceData[i] = FCString::Atof(*recvData[i]);
							if (faceData[i] > 1 || faceData[i] < 0)
							{
								faceData[i] = 0;
							}
						}
						FUDPData data;
						data.eyeblinkleft = faceData[0];
						data.eyeblinkright = faceData[1];
						data.eyesquintleft = faceData[2];
						data.eyesquintright = faceData[3];
						data.eyelookdownleft = faceData[4];
						data.eyelookdownright = faceData[5];
						data.eyelookinleft = faceData[6];
						data.eyelookinright = faceData[7];
						data.eyewideleft = faceData[8];
						data.eyewideright = faceData[9];
						data.eyelookoutleft = faceData[10];
						data.eyelookoutright = faceData[11];
						data.eyelookupleft = faceData[12];
						data.eyelookupright = faceData[13];
						data.browdownleft = faceData[14];
						data.browdownright = faceData[15];
						data.browinnerup = faceData[16];
						data.browouterupleft = faceData[17];
						data.browouterupright = faceData[18];
						data.jawforward = faceData[19];
						data.jawleft = faceData[20];
						data.jawopen = faceData[21];
						data.jawright = faceData[22];
						data.jawopenmid = faceData[23];
						data.mouthleft = faceData[24];
						data.mouthright = faceData[25];
						data.mouthfrownleft = faceData[26];
						data.mouthfrownright = faceData[27];
						data.mouthsmileleft = faceData[28];
						data.mouthsmileright = faceData[29];
						data.mouthstretchleft = faceData[30];
						data.mouthstretchright = faceData[31];
						data.mouthdimpleleft = faceData[32];
						data.mouthdimpleright = faceData[33];
						data.mouthrolllower = faceData[34];
						data.mouthrollupper = faceData[35];
						data.mouthshrugupper = faceData[36];
						data.mouthlowerdown = faceData[37];
						data.mouthshruglower = faceData[38];
						data.mouthfunnel = faceData[39];
						data.mouthpucker = faceData[40];
						data.mouthcompress = faceData[41];
						data.mouthupperout = faceData[42];
						data.facesneer = faceData[43];
						data.cheekpuff = faceData[44];
						data.cheeksquintleft = faceData[45];
						data.cheeksquintright = faceData[46];
						data.nosesneerleft = faceData[47];
						data.nosesneerright = faceData[48];
						data.mouthupperupright = faceData[49];
						data.mouthupperupleft = faceData[50];
						data.mouthlowerdownright = faceData[51];
						data.mouthlowerdownleft = faceData[52];
						data.mouthpressleft = faceData[53];
						data.mouthpressright = faceData[54];
						data.mouthclose = faceData[55];
						data.tongueout = faceData[56];


						if (UpdateInterest)
							UpdateReceiverData(data);

						got_new_data = true;
					}
				}
			}
		}
	}
}




bool AUDPReceiver::StartUDPReceiverBody(const FString& SocketName, const int32 Port)
{
	FIPv4Endpoint Endpoint(FIPv4Address::Any, Port);
	int32 BufferSize = RECV_BUFFER_SIZE;
	ListenSocketBody = FUdpSocketBuilder(*SocketName).AsNonBlocking()
		.AsReusable()
		.AsBlocking()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);


	if (broadcastMode != 2)
	{
		if (ConnectTCPServer(TcpServerIp, TcpPort, 1) != 0)
		{
			doReconnect = true;
		}
		if (!SocketTCPBody)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, TEXT("TCP socket create failed!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, TEXT("body  socket created!"));
		}
	}
	if (broadcastMode != 1)
	{

		bodyRecvThread = std::thread(std::bind(&AUDPReceiver::bodyRecving, this));
	}
	else
	{
		uint8* sendData = (uint8*)malloc(sizeof(uint8) * 1024);
		std::string str = uid + ":RECV:CHARACTER-BODY:NULL";
		memcpy(sendData, str.c_str(), str.length());
		SendDataTCP(sendData, str.length(), 1);
		FString msg(str.c_str());
		free(sendData);
		tcpBodyRecvThread = std::thread(std::bind(&AUDPReceiver::tcpBodyRecving, this));
	}
	return true;
}

void AUDPReceiver::bodyRecving()
{
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	while (is_running)
	{
		if (ListenSocketBody->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			uint32 Size;

			while (ListenSocketBody->HasPendingData(Size))
			{
				int32 Read = 0;
				if (ListenSocketBody->RecvFrom(BodyRecvBuffer.GetData(), BodyRecvBuffer.Num(), Read, *Sender))
				{
					if (Read > 0)
					{
						TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData = MakeShareable(new TArray<uint8>());
						ReceivedData->SetNumUninitialized(Read);
						memcpy(ReceivedData->GetData(), BodyRecvBuffer.GetData(), Read);
						//AsyncTask(ENamedThreads::GameThread, [this, ReceivedData]() { HandleReceivedData(ReceivedData); });
						if (broadcastMode == 0)
						{
							//SendBodyDataTCP(BodyRecvBuffer.GetData(), Read);
							SendBodyDataTCP2(ReceivedData, Read);
						}
						HandleBodyData(ReceivedData, 0);  // 处理躯干数据
					}
				}
			}
		}
	}
}

void AUDPReceiver::tcpBodyRecving()
{
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	while (is_running)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Recving body data, size %d"), 0));
		if (SocketTCPBody->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			int32 Read = 0;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Recving body data, size %d"), Read));
			//if (SocketTCPBody->RecvFrom(BodyRecvBuffer.GetData(), BodyRecvBuffer.Num(), Read, *Sender))
			if (!SocketTCPBody->Recv(BodyRecvBuffer.GetData(), BodyRecvBuffer.Num(), Read))
			{
				doReconnect = true;
			}
			if (printLog)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Recving body data, size %d"), Read));
			}
			if (Read > 0)
			{
				TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData = MakeShareable(new TArray<uint8>());
				ReceivedData->SetNumUninitialized(Read);
				memcpy(ReceivedData->GetData(), BodyRecvBuffer.GetData(), Read);
				HandleBodyData(ReceivedData, 1);  // 处理躯干数据

			}
		}
	}
}

void AUDPReceiver::HandleBodyData(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData, int mode)
{
	FString JsonString;
	JsonString.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		JsonString += TCHAR(Byte);
	}
	//JsonString.FindChar("}");
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (printLog)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, JsonString);
	}
	bool flag = FJsonSerializer::Deserialize(Reader, JsonObject);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("numPosesCur , %d"), numPosesCur));
	if (flag)  // String转JSON
	{
		JsonObject->TryGetNumberField(TEXT("numPoses"), numPosesCur);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("numPosesCur , %d"), numPosesCur));
		const TArray< TSharedPtr<FJsonValue> >* tmp;
		if (JsonObject->TryGetArrayField(TEXT("poseNames"), tmp))
		{
			TArray<TSharedPtr<FJsonValue>> poseNames = JsonObject->GetArrayField(TEXT("poseNames"));
			int boneNums = BoneNamesCur.Num();
			for (int i = 0; i < boneNums; i++)
			{
				BoneNamesCur.Pop();
			}
			for (int i = 0; i < numPosesCur; i++)
			{
				BoneNamesCur.Add(FName(poseNames[i]->AsString()));
			}
			useData = true;
		}
		else if (mode == 0)
		{
			useData = true;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), numPosesCur));
		if (useData)
		{
			TArray<FUDPBoneData> bodyData;
			TArray<TSharedPtr<FJsonValue>> framePose = JsonObject->GetArrayField(TEXT("weightMat"));

			if (framePose.Num() == numPosesCur * 6)
			{
				for (int poseId = 0; poseId < numPosesCur; poseId++)  // 获取每一帧
				{
					FName boneName = BoneNamesCur[poseId];
					FVector BoneLocation; // loaction的值

					double X = 0;
					double Y = 0;
					double Z = 0;
					if (boneName == FName("Hips"))
					{
						X = framePose[poseId * 6]->AsNumber();
						Y = framePose[1 + poseId * 6]->AsNumber();
						Z = framePose[2 + poseId * 6]->AsNumber();
					}
					BoneLocation = FVector(X, Y, Z);

					FRotator BoneQuat;
					double Roll = framePose[3 + poseId * 6]->AsNumber();
					double Pitch = framePose[4 + poseId * 6]->AsNumber();
					double Yaw = framePose[5 + poseId * 6]->AsNumber();
					if (boneName == FName("LeftShoulder") || boneName == FName("RightArm"))
					{
						Yaw -= 10;
					}
					if (boneName == FName("RightShoulder") || boneName == FName("leftArm"))
					{
						Yaw += 10;
					}
					BoneQuat = FRotator(Pitch, Yaw, Roll);  // FRotator(Pitch,Yaw,Roll)   Pitch:Y轴   Yaw:Z轴  Roll:X轴

					FVector BoneScale; // scale的值
					double ScaleX = 1.0;// framePose[6 + poseId * 9]->AsNumber();
					double ScaleY = 1.0;// framePose[7 + poseId * 9]->AsNumber();
					double ScaleZ = 1.0;// framePose[8 + poseId * 9]->AsNumber();
					BoneScale = FVector(ScaleX, ScaleY, ScaleZ);

					FUDPBoneData boneData;
					boneData.boneName = boneName;
					boneData.boneTransform = FTransform(BoneQuat, BoneLocation, BoneScale);
					bodyData.Add(boneData);
				}
				UpdateBodyData(bodyData);
				got_new_body_data = true;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("illegal data with %d bones and %d weights"), numPosesCur, framePose.Num()));
			}
		}
	}
}

void AUDPReceiver::UpdateBodyData(TArray<FUDPBoneData> bodyData)
{
	MyBodyData = bodyData;
}

TArray<FUDPBoneData> AUDPReceiver::GetBodyData()
{
	UpdateBodyInterest = true;	// Ready for new data
	got_new_body_data = false;
	return MyBodyData;
}

bool AUDPReceiver::IsNewBodyDataReady() {
	return got_new_body_data;
}



int AUDPReceiver::ConnectTCPServer(FString ip, int32 Port, int mode)
{
	FIPv4Endpoint ServerEndpoint;
	FIPv4Endpoint::Parse(ip, ServerEndpoint);
	TSharedPtr<FInternetAddr> addr = SocketSubsystem->CreateInternetAddr();
	bool Success = true;
	addr->SetIp(*ip, Success);
	if (!Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("fail"));
	}

	addr->SetPort(Port);

	if (mode == 0)
	{
		SocketTCP = FTcpSocketBuilder(TEXT("Client face Socket"))
			.AsReusable()
			.AsBlocking()
			.WithReceiveBufferSize(RECV_BUFFER_SIZE)
			.WithSendBufferSize(RECV_BUFFER_SIZE);

		if (SocketTCP->Connect(*addr))
		{
			uint32 size;
			SocketTCP->HasPendingData(size);
			return 0;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, TEXT("Client Connect Fail"));
			return -1;
		}
	}
	else if (mode == 1)
	{
		SocketTCPBody = FTcpSocketBuilder(TEXT("Client body Socket"))
			.AsReusable()
			.AsBlocking()
			.WithReceiveBufferSize(RECV_BUFFER_SIZE)
			.WithSendBufferSize(RECV_BUFFER_SIZE);

		if (SocketTCPBody->Connect(*addr))
		{
			uint32 size;
			SocketTCPBody->HasPendingData(size);
			return 0;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, TEXT("Client Connect Fail"));
			return -1;
		}
	}
	return 0;
}

void AUDPReceiver::SendDataTCP(FUDPData data)
{
	std::ostringstream oss;
	oss.precision(2);
	oss << data.eyeblinkleft << ",";
	oss << data.eyeblinkright << ",";
	oss << data.eyesquintleft << ",";
	oss << data.eyesquintright << ",";
	oss << data.eyelookdownleft << ",";
	oss << data.eyelookdownright << ",";
	oss << data.eyelookinleft << ",";
	oss << data.eyelookinright << ",";
	oss << data.eyewideleft << ",";
	oss << data.eyewideright << ",";
	oss << data.eyelookoutleft << ",";
	oss << data.eyelookoutright << ",";
	oss << data.eyelookupleft << ",";
	oss << data.eyelookupright << ",";
	oss << data.browdownleft << ",";
	oss << data.browdownright << ",";
	oss << data.browinnerup << ",";
	oss << data.browouterupleft << ",";
	oss << data.browouterupright << ",";
	oss << data.jawforward << ",";
	oss << data.jawleft << ",";
	oss << data.jawopen << ",";
	oss << data.jawright << ",";
	oss << data.jawopenmid << ",";
	oss << data.mouthleft << ",";
	oss << data.mouthright << ",";
	oss << data.mouthfrownleft << ",";
	oss << data.mouthfrownright << ",";
	oss << data.mouthsmileleft << ",";
	oss << data.mouthsmileright << ",";
	oss << data.mouthstretchleft << ",";
	oss << data.mouthstretchright << ",";
	oss << data.mouthdimpleleft << ",";
	oss << data.mouthdimpleright << ",";
	oss << data.mouthrolllower << ",";
	oss << data.mouthrollupper << ",";
	oss << data.mouthshrugupper << ",";
	oss << data.mouthlowerdown << ",";
	oss << data.mouthshruglower << ",";
	oss << data.mouthfunnel << ",";
	oss << data.mouthpucker << ",";
	oss << data.mouthcompress << ",";
	oss << data.mouthupperout << ",";
	oss << data.facesneer << ",";
	oss << data.cheekpuff << ",";
	oss << data.cheeksquintright << ",";
	oss << data.cheeksquintleft << ",";
	oss << data.nosesneerleft << ",";
	oss << data.nosesneerright << ",";
	oss << data.mouthupperupright << ",";
	oss << data.mouthupperupleft << ",";
	oss << data.mouthlowerdownright << ",";
	oss << data.mouthlowerdownleft << ",";
	oss << data.mouthpressleft << ",";
	oss << data.mouthpressright << ",";
	oss << data.mouthclose << ",";
	oss << data.tongueout;


	std::string str(oss.str());
	std::string prefix = uid + ":SEND:CHARACTER-FACE:";
	prefix.append(str);
	FString datastr(prefix.c_str());

	int len = prefix.length();
	int32 sent = 0;
	TCHAR* SendMessage = datastr.GetCharArray().GetData();
	if (printLog)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("sending face data size %d"), len));
	}

	if (!SocketTCP->Send((uint8*)TCHAR_TO_UTF8(SendMessage), len, sent))
	{
		doReconnect = true;
	}

	oss.str("");
	oss.clear();
}

void AUDPReceiver::SendBodyDataTCP(uint8* buffer, int32 len)
{
	int32 sent = 0;

	int32 size = 0;
	std::string prefix = uid + ":SEND:CHARACTER-BODY:";
	if (buffer)
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("%s"), (char*)buffer));
		std::string str((char*)buffer);
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "10");
		size = prefix.length() + len;
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "11");
		prefix.append(str);
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "12");
		FString datastr(prefix.c_str());
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "13");
		TCHAR* SendMessage = datastr.GetCharArray().GetData();
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "14");
		if (printLog)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("sending body data size %d"), size));
		}
		if (SocketTCPBody)
		{
			if (!SocketTCPBody->Send((uint8*)TCHAR_TO_UTF8(SendMessage), size, sent))
			{
				doReconnect = true;
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "SocketbodyTCP null");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "buffer null");
	}

}

void AUDPReceiver::SendBodyDataTCP2(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData, int32 len)
{
	FString JsonString;
	JsonString.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		JsonString += TCHAR(Byte);
	}

	int32 sent = 0;

	int32 size = 0;
	std::string prefix = uid + ":SEND:CHARACTER-BODY:";
	if (1)
	{
		size = prefix.length() + len;
		std:: string str = TCHAR_TO_UTF8(*JsonString);
		prefix.append(str);
		FString datastr(prefix.c_str());
		TCHAR* SendMessage = datastr.GetCharArray().GetData();
		if (printLog)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("sending body data size %d"), size));
		}
		if (SocketTCPBody)
		{
			if (!SocketTCPBody->Send((uint8*)TCHAR_TO_UTF8(SendMessage), size, sent))
			{
				doReconnect = true;
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "SocketbodyTCP null");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, "buffer null");
	}

}

void AUDPReceiver::SendDataTCP(uint8* buffer, int32 len, int mode)
{
	int32 sent = 0;
	if (mode == 1)
	{
		SocketTCPBody->Send(buffer, len, sent);
		FString str((char*)buffer);
		if (printLog)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, str);
		}
	}
	else
	{
		SocketTCP->Send(buffer, len, sent);
		FString str((char*)buffer);
		if (printLog)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, str);
		}
	}
}

void AUDPReceiver::reconnect()
{
	int ret = 0;
	int count = 0;
	while (is_running)
	{
		if (doReconnect)
		{
			while (count < 3)
			{
				FPlatformProcess::Sleep(10);
				ret = ConnectTCPServer(TcpServerIp, TcpPort, 0);
				if (ret == 0)
				{
					ret = ConnectTCPServer(TcpServerIp, TcpPort, 1);
				}
				else
				{
					count++;
					continue;
				}
				if (ret == 0)
				{
					break;
				}
				count++;
			}
			doReconnect = false;
		}
		else
		{
			FPlatformProcess::Sleep(0.1);
		}
	}
}
