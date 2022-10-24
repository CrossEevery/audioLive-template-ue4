#pragma once
#include "Serialization/Archive.h"
#include "UDPData.generated.h"

USTRUCT(BlueprintType)

struct FUDPBoneData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		FName boneName = FName("none");
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		FTransform boneTransform;// = FTransform(FVector(0, 0, 0), FRotator(0, 0, 0), FVector(1, 1, 1));
	FUDPBoneData() {
	}
};



USTRUCT(BlueprintType)
struct FUDPData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyeblinkleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyeblinkright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyesquintleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyesquintright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookdownleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookdownright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookinleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookinright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyewideleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyewideright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookoutleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookoutright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookupleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float eyelookupright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float browdownleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float browdownright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float browinnerup = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float browouterupleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float browouterupright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float jawforward = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float jawleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float jawopen = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float jawright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float jawopenmid = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthfrownleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthfrownright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthsmileleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthsmileright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthstretchleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthstretchright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthdimpleleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthdimpleright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthrollupper = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthrolllower = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthshruglower = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthlowerdown = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthshrugupper = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthfunnel = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthpucker = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthcompress = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthupperout = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float facesneer = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float cheekpuff = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float cheeksquintleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float cheeksquintright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float nosesneerleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float nosesneerright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthupperupright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthupperupleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthlowerdownright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthlowerdownleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthpressleft = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthpressright = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float mouthclose = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "UDPCommunication")
		float tongueout = 0.0f;
	FUDPData() {
	}
};


FORCEINLINE FArchive& operator<<(FArchive& Ar, FUDPData& Structure)
{	
	Ar << Structure.eyeblinkleft;
	Ar << Structure.eyeblinkright;
	Ar << Structure.eyesquintleft;
	Ar << Structure.eyesquintright;
	Ar << Structure.eyelookdownleft;
	Ar << Structure.eyelookdownright;
	Ar << Structure.eyelookinleft;
	Ar << Structure.eyelookinright;
	Ar << Structure.eyewideleft;
	Ar << Structure.eyewideright;
	Ar << Structure.eyelookoutleft;
	Ar << Structure.eyelookoutright;
	Ar << Structure.eyelookupleft;
	Ar << Structure.eyelookupright;
	Ar << Structure.browdownleft;
	Ar << Structure.browdownright;
	Ar << Structure.browinnerup;
	Ar << Structure.browouterupleft;
	Ar << Structure.browouterupright;
	Ar << Structure.jawforward;
	Ar << Structure.jawleft;
	Ar << Structure.jawopen;
	Ar << Structure.jawright;
	Ar << Structure.jawopenmid;
	Ar << Structure.mouthleft;
	Ar << Structure.mouthright;
	Ar << Structure.mouthfrownleft;
	Ar << Structure.mouthfrownright;
	Ar << Structure.mouthsmileleft;
	Ar << Structure.mouthsmileright;
	Ar << Structure.mouthstretchleft;
	Ar << Structure.mouthstretchright;
	Ar << Structure.mouthdimpleleft;
	Ar << Structure.mouthdimpleright;
	Ar << Structure.mouthrolllower;
	Ar << Structure.mouthrollupper;
	Ar << Structure.mouthshrugupper;
	Ar << Structure.mouthlowerdown;
	Ar << Structure.mouthshruglower;
	Ar << Structure.mouthfunnel;
	Ar << Structure.mouthpucker;
	Ar << Structure.mouthcompress;
	Ar << Structure.mouthupperout;
	Ar << Structure.facesneer;
	Ar << Structure.cheekpuff;
	Ar << Structure.cheeksquintleft;
	Ar << Structure.cheeksquintright;
	Ar << Structure.nosesneerleft;
	Ar << Structure.nosesneerright;
	Ar << Structure.mouthupperupright;
	Ar << Structure.mouthupperupleft;
	Ar << Structure.mouthlowerdownright;
	Ar << Structure.mouthlowerdownleft;
	Ar << Structure.mouthpressleft;
	Ar << Structure.mouthpressright;
	Ar << Structure.mouthclose;
	Ar << Structure.tongueout;

	return Ar;

}