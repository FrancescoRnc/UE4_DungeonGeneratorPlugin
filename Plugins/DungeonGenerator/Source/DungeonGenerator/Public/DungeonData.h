// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonSchemeMaker.h"
#include "UObject/NoExportTypes.h"
#include "DungeonData.generated.h"


enum class EDungeonInfoState : uint8
{
	NOVALID = 0,
	VALID = 1,
	MAX = 2
};

/**
*  FDungeonInfo Struct
*/
struct FDungeonInfo
{
	EDungeonInfoState State = EDungeonInfoState::NOVALID;
	FGrid Grid{};
	FIntVector GridSize{ 0,0,0 };
	TArray<int32> GridScheme{};
	
	TArray<struct FRoomInfo> RoomsInfo{};

	void Reset();
};


/**
 * Object File used to build your Dungeon on World after generation:
 * - GridScheme: A 1D representation of our Grid, in which any non-zero number stands for its Cell Pattern;
 * - PathLength: The length of the Path expressed by the Rooms count;
 * - GridSize: The full 2D size (Z is not used) of the stored Grid;
 * - RoomsPresetID: An Array that tracks the ID of all Presets used for the Dungeon;
 * - RoomsPresetPaths: An Array that tracks the File Path of all Presets used for the Dungeon;
 * - RoomsCoordinate: An Array that tracks Coordinates of all Rooms;
 * - RoomsGridIndex: An Array that tracks the Grid Index if all Rooms;
 * - DoorsSourceRoomIndex: An Array that tracks the Path Index of the Source Room of any specific Door;
 * - DoorsNextRoomIndex: An Array that tracks the Path Index of the Next Room of any specific Door;
 * - DoorsDirection: An Array that tracks the Direction of any specific Door;
 */
UCLASS()
class DUNGEONGENERATOR_API UDungeonData : public UObject
{
	GENERATED_BODY()

	public:

	// Grid Information
	UPROPERTY(VisibleAnywhere)
	TArray<int32> GridScheme;
	
	UPROPERTY(VisibleAnywhere)
	int32 PathLength;

	UPROPERTY(VisibleAnywhere)
	FIntVector GridSize;

	
	// Rooms Information
	UPROPERTY(VisibleAnywhere)
	TArray<int32> RoomsPresetID;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> RoomsPresetPaths;
	
	UPROPERTY(VisibleAnywhere)
	TArray<FIntVector> RoomsCoordinate;

	UPROPERTY(VisibleAnywhere)
	TArray<int32> RoomsGridIndex;

	
	// Doors Information	
	UPROPERTY(VisibleAnywhere)
	TArray<int32> DoorsSourceRoomIndex;
	
	UPROPERTY(VisibleAnywhere)
	TArray<int32> DoorsNextRoomIndex;
	
	UPROPERTY(VisibleAnywhere)
	TArray<int32> DoorsDirection;

	
	UFUNCTION(BlueprintCallable)
	void Reset();

	void SaveData(const FDungeonInfo& Info);

	void Serialize(FArchive& Ar) override;
};
