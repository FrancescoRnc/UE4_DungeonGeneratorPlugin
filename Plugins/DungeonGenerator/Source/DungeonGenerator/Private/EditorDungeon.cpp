// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorDungeon.h"
#include "Engine/StaticMeshSocket.h"

// Sets default values
AEditorDungeon::AEditorDungeon()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject
		<USceneComponent>(TEXT("Root Component"));
	DoorInstancesComponent = CreateDefaultSubobject
		<UInstancedStaticMeshComponent>(TEXT("Door instances Component"));

	DoorInstancesComponent->SetupAttachment(RootComponent);
}

void AEditorDungeon::Build()
{
	ClearInstances();

	BuildDungeon(0);
	BuildRooms();
	BuildDoors();
}

void AEditorDungeon::ClearInstances()
{
	DungeonInfo.Reset();

	for (TPair<uint32, UInstancedStaticMeshComponent*> FIMC : FloorMeshInstances)
	{
		FIMC.Value->ClearInstances();
	}

	for (TPair<uint32, UInstancedStaticMeshComponent*> WIMC : WallsMeshInstances)
	{
		WIMC.Value->ClearInstances();
	}

	DoorInstancesComponent->ClearInstances();

	UE_LOG(LogTemp, Warning, TEXT("Instances Clear Complete!"));
}

void AEditorDungeon::BuildDungeon(const int32 RoomsCount)
{
	DungeonDataRef = FDungeonUtilities::Get()->GetDungeonData();

	DungeonInfo.GridScheme = DungeonDataRef->GridScheme;
	DungeonInfo.State = EDungeonInfoState::VALID;
}

void AEditorDungeon::BuildRooms()
{
	if (!IsValid(DungeonDataRef.Get()))
	{
		return;
	}

	
	for (int32 Index = 0; Index < DungeonDataRef->PathLength; Index++)
	{
		FRoomInfo RoomInfo{};

		const FVector Position =
		{
			DungeonDataRef->RoomsCoordinate[Index].Y * InstancesSpaceGap,
			DungeonDataRef->RoomsCoordinate[Index].X * -InstancesSpaceGap,
			0
		};

		SpawnNewRoom(DungeonDataRef->RoomsPresetID[Index], Position);
		const URoomPresetPtr Preset = FDungeonUtilities::Get()->GetPresetByID(DungeonDataRef->RoomsPresetID[Index]);
		
		RoomInfo.PresetID = DungeonDataRef->RoomsPresetID[Index];
		RoomInfo.PresetPath = DungeonDataRef->RoomsPresetPaths[Index];
		RoomInfo.CoordinateInGrid = DungeonDataRef->RoomsCoordinate[Index];
		RoomInfo.IndexInGrid = DungeonDataRef->RoomsGridIndex[Index];
		RoomInfo.PresetName = *Preset->GetName();
		RoomInfo.RoomName = Preset->RoomName;
		DungeonInfo.RoomsInfo.Add(RoomInfo);
	}
}

void AEditorDungeon::BuildDoors()
{
	if (!IsValid(DungeonDataRef.Get()))
	{
		return;
	}

	TArray<FTransform> Transforms;
	const int32 DoorsCount = DungeonDataRef->DoorsDirection.Num();
	Transforms.Init({}, DoorsCount);

	for (int32 Index = 0; Index < DoorsCount; Index++)
	{
		FDoorInfo DoorInfo{};
		DoorInfo.SourceRoomIndex = DungeonDataRef->DoorsSourceRoomIndex[Index];
		DoorInfo.NextRoomIndex = DungeonDataRef->DoorsNextRoomIndex[Index];
		DoorInfo.Direction = static_cast<EWorldDirection>(DungeonDataRef->DoorsDirection[Index]);
		DoorInfo.OppositeDoorDirection = static_cast<EWorldDirection>(DungeonDataRef->DoorsOppositeDirection[Index]);

		FRoomInfo SourceRoom = DungeonInfo.RoomsInfo[DoorInfo.SourceRoomIndex];
		const URoomPresetPtr Preset = FDungeonUtilities::Get()->GetPresetByID(SourceRoom.PresetID);
		const UStaticMesh* FloorMeshRef = FloorMeshInstances[SourceRoom.PresetID]->GetStaticMesh();
		const FVector RoomLocation =
		{
			SourceRoom.CoordinateInGrid.Y * InstancesSpaceGap,
			SourceRoom.CoordinateInGrid.X * -InstancesSpaceGap,
			0
		};

		const FName DirectionName = WorldDirectionNames[DoorInfo.Direction];

		UStaticMeshSocket* SelectedSocket = FloorMeshRef->FindSocket(DirectionName);
		if (SelectedSocket)
		{
			Transforms[Index].SetLocation(SelectedSocket->RelativeLocation + RoomLocation);
			Transforms[Index].SetRotation(SelectedSocket->RelativeRotation.Quaternion());
			DoorInstancesComponent->SetStaticMesh(Preset->DoorsMesh);
		}		
	}

	DoorInstancesComponent->AddInstances(Transforms, true);
}


void AEditorDungeon::AddNewRoomInstancedMeshComponents(const int32 PresetID)
{
	if (!FDungeonUtilities::Get()->CheckRoomPresetByID(PresetID))
	{
		UE_LOG(LogTemp, Error, TEXT("Preset with ID %d does not exist!!"), PresetID);
		return;
	}

	TSoftObjectPtr<URoomPreset> Preset = FDungeonUtilities::Get()->GetPresetByID(PresetID);

	const FString NewFloorComponentName = FString::Printf(TEXT("%s Floors"), *Preset->RoomName.ToString());
	UInstancedStaticMeshComponent* NewFloorComponent =
		NewObject<UInstancedStaticMeshComponent>(this, *NewFloorComponentName);
	NewFloorComponent->RegisterComponent();
	NewFloorComponent->SetStaticMesh(Preset->FloorMesh);

	AddInstanceComponent(NewFloorComponent);
	FloorMeshInstances.Add(PresetID, NewFloorComponent);

	const FString NewWallsComponentName = FString::Printf(TEXT("%s Walls"), *Preset->RoomName.ToString());
	UInstancedStaticMeshComponent* NewWallsComponent =
		NewObject<UInstancedStaticMeshComponent>(this, *NewWallsComponentName);
	NewWallsComponent->RegisterComponent();
	NewWallsComponent->SetStaticMesh(Preset->WallsMesh);

	AddInstanceComponent(NewWallsComponent);
	WallsMeshInstances.Add(PresetID, NewWallsComponent);

}

void AEditorDungeon::SpawnNewRoom(const int32 PresetID, const FVector Position)
{
	if (!FDungeonUtilities::Get()->CheckRoomPresetByID(PresetID))
	{
		return;
	}

	if (!FloorMeshInstances.Contains(PresetID) &&
		!WallsMeshInstances.Contains(PresetID))
	{
		AddNewRoomInstancedMeshComponents(PresetID);
	}

	const FTransform Transform(Position);
	FloorMeshInstances[PresetID]->AddInstance(Transform);
	WallsMeshInstances[PresetID]->AddInstance(Transform);
}
