// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonData.h"
#include "RoomData.h"
#include "DungeonUtilities.h"
#include "GenerationSettings.h"


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
	GridPathOrder.Empty();
		
	RoomsName.Empty();
	RoomsPresetName.Empty();
	RoomsPresetID.Empty();
	RoomsPresetPaths.Empty();
	RoomsCoordinate.Empty();
	RoomsGridIndex.Empty();
		
	DoorsSourceRoomIndex.Empty();
	DoorsNextRoomIndex.Empty();
	DoorsDirection.Empty();
	DoorsOppositeDirection.Empty();
}

void UDungeonData::SaveData(const FDungeonInfo& Info)
{
	GridSize = Info.GridSize;
	PathLength = Info.RoomsInfo.Num();
	GridScheme = Info.GridScheme;
	GridPathOrder = Info.GridPathOrder;
	RoomsName.Init(TEXT(""), PathLength);
	RoomsPresetName.Init(TEXT(""), PathLength);
	RoomsPresetID.Init(-1, PathLength);
	RoomsPresetPaths.Init(TEXT(""), PathLength);
	RoomsCoordinate.Init({ 0,0,0 }, PathLength);
	RoomsGridIndex.Init(-1, PathLength);

	for (int32 Index = 0; Index < PathLength; Index++)
	{
		const FRoomInfo RoomInfo = Info.RoomsInfo[Index];
		RoomsName[Index] = RoomInfo.RoomName;
		RoomsPresetName[Index] = RoomInfo.PresetName;
		RoomsPresetID[Index] = RoomInfo.PresetID;
		RoomsPresetPaths[Index] = RoomInfo.PresetPath;
		RoomsCoordinate[Index] = RoomInfo.CoordinateInGrid;
		RoomsGridIndex[Index] = RoomInfo.IndexInGrid;
		const int32 DoorsCount = RoomInfo.DoorsInfo.Num();
		for (int32 DoorIndex = 0; DoorIndex < DoorsCount; DoorIndex++)
		{
			const int32 IntDirection = static_cast<int32>
				(RoomInfo.DoorsInfo[DoorIndex].Direction);
			const int32 IntOppositeDirection = static_cast<int32>
				(RoomInfo.DoorsInfo[DoorIndex].OppositeDoorDirection);
			DoorsDirection.Add(IntDirection);
			DoorsOppositeDirection.Add(IntOppositeDirection);
			DoorsSourceRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].SourceRoomIndex);
			DoorsNextRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].NextRoomIndex);
		}
	}
}

const FDungeonInfo UDungeonData::ExtractDungeonInfo(const UGenerationSettings* InSettings) const
{
	FDungeonInfo OutInfo{};

	OutInfo.State = EDungeonInfoState::VALID;
	OutInfo.GridSize = GridSize;
	OutInfo.Grid = FGrid(GridSize, GridScheme, GridPathOrder);
	OutInfo.GridScheme = GridScheme;
	OutInfo.GridPathOrder = GridPathOrder;
	OutInfo.RoomsInfo.Init({}, PathLength);
	for (int32 Index = 0; Index < PathLength; Index++)
	{
		const TSoftObjectPtr<URoomPreset> Preset =
			//FDungeonUtilities::Get()->GetPresetByID(RoomsPresetID[Index]);
			InSettings->RoomPresetsRefMap[RoomsPresetID[Index]];
		FRoomInfo& Room = OutInfo.RoomsInfo[Index];

		Room.RoomName = RoomsName[Index];
		Room.IndexInGrid = RoomsGridIndex[Index];
		Room.CoordinateInGrid = RoomsCoordinate[Index];
		Room.PresetID = RoomsPresetID[Index];
		Room.PresetName = RoomsPresetName[Index];
		Room.PresetPath = RoomsPresetPaths[Index];
	}
	for (int32 Index = 0; Index < DoorsDirection.Num(); Index++)
	{
		FDoorInfo Door;
		Door.Direction = static_cast<EWorldDirection>(DoorsDirection[Index]);
		Door.OppositeDoorDirection = static_cast<EWorldDirection>(DoorsOppositeDirection[Index]);
		Door.SourceRoomIndex = DoorsSourceRoomIndex[Index];
		Door.NextRoomIndex = DoorsNextRoomIndex[Index];
		OutInfo.RoomsInfo[DoorsSourceRoomIndex[Index]].DoorsInfo.Add(Door);
	}

	return OutInfo;
}

void UDungeonData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	/*if (Ar.IsSaving())
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

		//GridScheme.BulkSerialize(Ar);
		//Ar << PathLength;
		//Ar << GridSize;
		//RoomsPresetID.BulkSerialize(Ar);
		//RoomsPresetPaths.BulkSerialize(Ar);
		//RoomsCoordinate.BulkSerialize(Ar);
		//RoomsGridIndex.BulkSerialize(Ar);
		//DoorsSourceRoomIndex.BulkSerialize(Ar);
		//DoorsNextRoomIndex.BulkSerialize(Ar);
		//DoorsDirection.BulkSerialize(Ar);
	}*/
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
