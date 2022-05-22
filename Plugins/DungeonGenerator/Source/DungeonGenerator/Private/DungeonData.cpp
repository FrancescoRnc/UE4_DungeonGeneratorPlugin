// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonData.h"
#include "RoomData.h"

void FDungeonInfo::Reset()
{
	Grid = {};
	GridSize = {0,0,0};
	GridScheme.Empty();
	RoomsInfo.Empty();
}


void UDungeonData::Reset()
{
	GridSize = {0,0,0};
	PathLength = 0;
	GridScheme.Empty();
		
	RoomsPresetID.Empty();
	RoomsCoordinate.Empty();
	RoomsGridIndex.Empty();
		
	DoorsSourceRoomIndex.Empty();
	DoorsNextRoomIndex.Empty();
	DoorsDirection.Empty();
}

void UDungeonData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		//Ar << GridScheme;
		//Ar << PathLength;
		//Ar << GridSize;
		//Ar << RoomsPresetID;
		//Ar << RoomsPresetPaths;
		//Ar << RoomsCoordinate;
		//Ar << RoomsGridIndex;
		//Ar << DoorsSourceRoomIndex;
		//Ar << DoorsNextRoomIndex;
		//Ar << DoorsDirection;

		GridScheme.BulkSerialize(Ar);
		Ar << PathLength;
		Ar << GridSize;
		RoomsPresetID.BulkSerialize(Ar);
		RoomsPresetPaths.BulkSerialize(Ar);
		RoomsCoordinate.BulkSerialize(Ar);
		RoomsGridIndex.BulkSerialize(Ar);
		DoorsSourceRoomIndex.BulkSerialize(Ar);
		DoorsNextRoomIndex.BulkSerialize(Ar);
		DoorsDirection.BulkSerialize(Ar);
	}
	/*else if (Ar.IsLoading())
	{
		int32				LoadedPathLength;
		FIntVector			LoadedGridSize;
		TArray<int32>		LoadedGridScheme;
		TArray<int32>		LoadedRoomsPresetID;
		TArray<FString>		LoadedRoomsPresetPaths;
		TArray<FIntVector>	LoadedRoomsCoordinate;
		TArray<int32>		LoadedRoomsGridIndex;
		TArray<int32>		LoadedDoorsSourceRoomIndex;
		TArray<int32>		LoadedDoorsNextRoomIndex;
		TArray<int32>		LoadedDoorsDirection;

		LoadedGridScheme.BulkSerialize(Ar);
		Ar << LoadedPathLength;
		Ar << LoadedGridSize;
		LoadedRoomsPresetID.BulkSerialize(Ar);
		LoadedRoomsPresetPaths.BulkSerialize(Ar);
		LoadedRoomsCoordinate.BulkSerialize(Ar);
		LoadedRoomsGridIndex.BulkSerialize(Ar);
		LoadedDoorsSourceRoomIndex.BulkSerialize(Ar);
		LoadedDoorsNextRoomIndex.BulkSerialize(Ar);
		LoadedDoorsDirection.BulkSerialize(Ar);

		PathLength = LoadedPathLength;
		GridSize = LoadedGridSize;
		GridScheme = LoadedGridScheme;
		RoomsPresetID = LoadedRoomsPresetID;
		RoomsPresetPaths = LoadedRoomsPresetPaths;
		RoomsCoordinate = LoadedRoomsCoordinate;
		RoomsGridIndex = LoadedRoomsGridIndex;
		DoorsSourceRoomIndex = LoadedDoorsSourceRoomIndex;
		DoorsNextRoomIndex = LoadedDoorsNextRoomIndex;
		DoorsDirection = LoadedDoorsDirection;
	}*/
}
