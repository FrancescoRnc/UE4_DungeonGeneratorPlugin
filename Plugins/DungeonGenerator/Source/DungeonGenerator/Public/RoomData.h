// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "UObject/NoExportTypes.h"
#include "RoomData.generated.h"




/**
* 
*/
struct FRoomInfo
{
	FName RoomName = NAME_None;
	int32 IndexInGrid = -1;
	FIntVector CoordinateInGrid{0,0,0};
	TArray<FDoorInfo> DoorsInfo{};
	int32 PresetID = -1;
	FName PresetName = NAME_None;
	FName PresetPath = NAME_None;
};


/**
 * 
 */
UCLASS()
class DUNGEONGENERATOR_API URoomPreset : public UObject
{
	GENERATED_BODY()

	public:

	URoomPreset();

	UPROPERTY(VisibleAnywhere)
	int32 PresetID = -1;
	
	UPROPERTY(EditAnywhere)
	FString RoomName;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* FloorMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallsMesh;

	UPROPERTY(EditAnywhere)
	UStaticMesh* DoorsMesh;

	UFUNCTION(BlueprintCallable)
	void DoThings();
};
