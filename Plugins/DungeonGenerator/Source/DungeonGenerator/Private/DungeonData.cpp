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
	RoomsPresetPaths.Empty();
	RoomsCoordinate.Empty();
	RoomsGridIndex.Empty();
		
	DoorsSourceRoomIndex.Empty();
	DoorsNextRoomIndex.Empty();
	DoorsDirection.Empty();
}

void UDungeonData::SaveData(const FDungeonInfo& Info)
{
	GridSize = Info.GridSize;
	PathLength = Info.RoomsInfo.Num();
	GridScheme = Info.GridScheme;
	RoomsPresetID.Init(-1, PathLength);
	RoomsPresetPaths.Init(TEXT(""), PathLength);
	RoomsCoordinate.Init({ 0,0,0 }, PathLength);
	RoomsGridIndex.Init(-1, PathLength);

	for (int32 Index = 0; Index < PathLength; Index++)
	{
		const FRoomInfo RoomInfo = Info.RoomsInfo[Index];
		RoomsPresetID[Index] = RoomInfo.PresetID;
		RoomsPresetPaths[Index] = RoomInfo.PresetPath;
		RoomsCoordinate[Index] = RoomInfo.CoordinateInGrid;
		RoomsGridIndex[Index] = RoomInfo.IndexInGrid;
		const int32 RoomsCount = RoomInfo.DoorsInfo.Num();
		for (int32 DoorIndex = 0; DoorIndex < RoomsCount; DoorIndex++)
		{
			const int32 IntDirection = static_cast<int32>
				(RoomInfo.DoorsInfo[DoorIndex].Direction);
			DoorsDirection.Add(IntDirection);
			DoorsSourceRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].SourceRoomIndex);
			DoorsNextRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].NextRoomIndex);
		}
	}
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
