#pragma once
#include "CrossInteractionData.generated.h"

USTRUCT(BlueprintType)
struct FCIPortalProp {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int type = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int ID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FName name = "";
};

USTRUCT(BlueprintType)
struct FCIEffectProp {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int ID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FName name = "";
};

USTRUCT(BlueprintType)
struct FCITYQProp {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int ID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FName name = "";
};

USTRUCT(BlueprintType)
struct FCIBillboardProp {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int type = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString name = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString url = "";
};



USTRUCT(BlueprintType)

struct FCIMsgData {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int msgType = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		bool editMode = false;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int portalType = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString portalIp = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int portalPort = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int startID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int endID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString ip = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int port = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString billboardUrl = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString billboardName = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int billboardType = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int billboardStatus = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		TArray<FCIBillboardProp> billboardProps;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int effectID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int effectStatus = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int TYQID = 0;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		FString decalUrl = "";
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int skillID = -1;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		int editModeCamID = -1;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "CrossInteraction")
		bool billboardInstSet = false;
	FCIMsgData() {
	}
};

