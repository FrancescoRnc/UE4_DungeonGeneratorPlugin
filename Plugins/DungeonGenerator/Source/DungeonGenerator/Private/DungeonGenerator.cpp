// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonGenerator.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DungeonData.h"
#include "DungeonSchemeMaker.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"
#include "RoomData.h"


#define LOCTEXT_NAMESPACE "FDungeonGeneratorModule"


// FDungeonDataGenerator
FDungeonDataGenerator::FDungeonDataGenerator()
{
	
}

FDungeonDataGenerator::~FDungeonDataGenerator()
{
	
}

void FDungeonDataGenerator::BuildDungeon(const int32 RoomsCount)
{
	OutDungeonInfo = {};
	if (RoomsCount <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot Building Dungeon! Rooms Count is no valid!"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Building Dungeon..."));

	FGrid Grid = {};

	// This Do-While loop prevents the GridMaker in making a Grid
	// with a smaller number of Rooms than the requested amount
	do
	{
		// Here you can set whichever of your custom GeneratorMethods for your custom Dungeon Generation
		FDungeonGridMaker DungeonGridMaker(RoomsCount, Method);

		Grid = DungeonGridMaker.Make();
		UE_LOG(LogTemp, Display, TEXT("Trying making a Grid..."));

	} while (Grid.PathLength < RoomsCount);
	UE_LOG(LogTemp, Display, TEXT("Grid made!"));
	
	const TArray<int32> Scheme = Grid.GetScheme();
	const TArray<int32> SchemePath = Grid.GetSchemePath();
	
	OutDungeonInfo.State = EDungeonInfoState::VALID;
	OutDungeonInfo.RoomsCount = Grid.PathLength;
	OutDungeonInfo.Grid = Grid;
	OutDungeonInfo.GridSize = Grid.Size;	
	OutDungeonInfo.GridScheme = Scheme;
	OutDungeonInfo.GridPathOrder = SchemePath;
}

void FDungeonDataGenerator::BuildRooms()
{
	if (OutDungeonInfo.State == EDungeonInfoState::NOVALID)
	{
		return;
	}

	const TMap<int32, URoomPreset*> RoomPresetsRef = Settings->LoadRoomPresetReferences();
	const int32 PresetsCount = RoomPresetsRef.Num(); //DungeonUtils->GetPresetFilesCount();
	const int32 PresetPathsCount = Settings->RoomPresetsPaths.Num();
	if (PresetsCount <= 0 || PresetPathsCount <= 0)
	{
		OutDungeonInfo.State = EDungeonInfoState::NOVALID;
		UE_LOG(LogTemp, Error, TEXT("Impossible to Generate a new dungeon: No Presets loaded."));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Building Rooms..."));
	
	const int32 Length = OutDungeonInfo.Grid.PathLength;
	OutDungeonInfo.RoomsInfo.Init({}, Length);

	TArray<URoomPreset*> RoomPresetsArray;
	RoomPresetsRef.GenerateValueArray(RoomPresetsArray);
	
	for (int32 Index = 0; Index < Length; Index++)
	{	
		FRoomInfo Room;		
		
		const int32 RandomPresetsMax = PresetsCount - 1;
		const int32 RandomPresetIndex = Index == 0 ? 0 : FMath::RandRange(1, RandomPresetsMax);
		const URoomPresetPtr RandomPreset = RoomPresetsArray[RandomPresetIndex];
		Room.PresetPath = *RandomPreset->GetPathName();
		Room.PresetID = RandomPreset->PresetID;

		const FIntVector RoomCoords =
		{
			OutDungeonInfo.Grid.PathTrack[Index].X,
			OutDungeonInfo.Grid.PathTrack[Index].Y,
			0
		};
		
		Room.RoomName = *FString::Printf(TEXT("PathOrder%02d"), (Index + 1));
		Room.CoordinateInGrid = RoomCoords;
		Room.IndexInGrid = Room.CoordinateInGrid.X +
			(Room.CoordinateInGrid.Y * OutDungeonInfo.GridSize.X);		
		
		OutDungeonInfo.RoomsInfo[Index] = Room;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Rooms Built!"));
}

void FDungeonDataGenerator::BuildDoors()
{
	if (OutDungeonInfo.State == EDungeonInfoState::NOVALID)
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Building Doors..."));

	TArray<FRoomInfo>& OutRoomsInfo = OutDungeonInfo.RoomsInfo;

	for (int32 RoomIndex = 1; RoomIndex < OutRoomsInfo.Num(); RoomIndex++)
	{
		const int32 PrevRoomIndex = RoomIndex - 1;
		FRoomInfo& PrevRoom = OutRoomsInfo[PrevRoomIndex];
		FRoomInfo& CurrRoom = OutRoomsInfo[RoomIndex];
		FDoorInfo PrevDoor, CurrDoor;

		const FIntVector DeltaCoords =
			CurrRoom.CoordinateInGrid - PrevRoom.CoordinateInGrid;
		const int32 PatternIndex = Directions.IndexOfByKey(DeltaCoords);

		PrevDoor.SourceRoomName = PrevRoom.RoomName;
		PrevDoor.SourceRoomIndex = PrevRoomIndex;
		PrevDoor.NextRoomName = CurrRoom.RoomName;
		PrevDoor.NextRoomIndex = RoomIndex;
		const int32 CurrDirection = PatternIndex;
		PrevDoor.Direction = static_cast<EWorldDirection>(PatternIndex);

		CurrDoor.SourceRoomName = PrevRoom.RoomName;
		CurrDoor.SourceRoomIndex = RoomIndex;
		CurrDoor.NextRoomName = CurrRoom.RoomName;
		CurrDoor.NextRoomIndex = PrevRoomIndex;
		const int32 PrevDirection = IGeneratorMethod::GetOppositeIndex(PatternIndex);
		CurrDoor.Direction = static_cast<EWorldDirection>(PrevDirection);

		PrevDoor.OppositeDoorDirection = CurrDoor.Direction;
		CurrDoor.OppositeDoorDirection = PrevDoor.Direction;

		PrevRoom.DoorsInfo.Add(PrevDoor);
		CurrRoom.DoorsInfo.Add(CurrDoor);	
	}
	OutDungeonInfo.RoomsInfo = OutRoomsInfo;
	
	UE_LOG(LogTemp, Display , TEXT("Doors Built!"));
}


// FDungeonGeneratorModule

void FDungeonGeneratorModule::StartupModule()
{
	DungeonUtils = MakeShared<FDungeonUtilities>();
	DungeonUtils->CreateGenerationSettings();

	FAssetRegistryModule::GetRegistry().OnFilesLoaded()
		.AddRaw(DungeonUtils.Get(), &FDungeonUtilities::GetPresetsOnLoad);
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetDeleted()
		.AddLambda([this](UObject* Object)
    {    	
    	if (const URoomPresetPtr Preset = Cast<URoomPreset>(Object))
    	{
			DungeonUtils->DeletePresetReference(Preset);
    		//UE_LOG(LogDunGenStartup, Display, TEXT("In Memory Asset Deleted: %s"), *Object->GetName());
    	}    	
    });
	
	//UE_LOG(LogDunGenStartup, Display, TEXT("Dungeon Generator Startup"));
}

void FDungeonGeneratorModule::ShutdownModule()
{
	//UE_LOG(LogDunGenShutdown, Display, TEXT("Dungeon Generator Shutdown"));
}

bool FDungeonGeneratorModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{

	return false;
}




#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDungeonGeneratorModule, DungeonGenerator)