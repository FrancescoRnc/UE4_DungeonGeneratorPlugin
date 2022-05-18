// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeDungeon.h"


// Sets default values
ARuntimeDungeon::ARuntimeDungeon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	//RoomInstancesComponent = CreateDefaultSubobject
	//	<UInstancedStaticMeshComponent>(TEXT("Room Instances Component"));
	DoorInstancesComponent = CreateDefaultSubobject
		<UInstancedStaticMeshComponent>(TEXT("Door instances Component"));

	//RoomInstancesComponent->SetupAttachment(RootComponent);
	DoorInstancesComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARuntimeDungeon::BeginPlay()
{
	Super::BeginPlay();
	
	//Build();
}

// Called every frame
//void ARuntimeDungeon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}


void ARuntimeDungeon::Build()
{
	BuildDungeon(0);
	BuildRooms();
	BuildDoors();
}

void ARuntimeDungeon::BuildDungeon(const int32 RoomsCount)
{
	//DungeonDataRef = FDungeonUtilities::Get()->GetDungeonDataAsset();
	DungeonDataRef = FDungeonUtilities::Get()->GetDungeonData();
}

void ARuntimeDungeon::BuildRooms()
{
	if (!IsValid(DungeonDataRef.Get()))
	{
		return;
	}
	
	TArray<FTransform> Transforms;
	//Transforms.Init({}, DungeonDataRef->PathLength);

	//RoomInstancesComponent->SetStaticMesh();

	for (int32 Index = 0; Index < DungeonDataRef->PathLength; Index++)
	{
		FVector Position =
		{
			DungeonDataRef->RoomsCoordinate[Index].X * 5000.f,
			DungeonDataRef->RoomsCoordinate[Index].Y * 5000.f,
			DungeonDataRef->RoomsCoordinate[Index].Z * 5000.f
		};

		SpawnNewRoom(DungeonDataRef->RoomsPresetID[Index], Position);
	}
	
	//RoomInstancesComponent->AddInstances(Transforms, true);
}

void ARuntimeDungeon::BuildDoors()
{
	if (!IsValid(DungeonDataRef.Get()))
	{
		return;
	}
	
	TArray<FTransform> Transforms;
	const int32 DoorsCount = DungeonDataRef->DoorsDirection.Num();
	Transforms.Init({}, DoorsCount);
	
	//RoomInstancesComponent->SetStaticMesh();
	
	for (int32 Index = 0; Index < DoorsCount; Index++)
	{
		
	}
	
	//DoorInstancesComponent->AddInstances(Transforms, true);
}


void ARuntimeDungeon::AddNewRoomInstanceMeshComponent(const int32 PresetID)
{
	TSoftObjectPtr<URoomPreset> Preset = nullptr;// FDungeonUtilities::Get()->GetRoomPresetByID(PresetID);

	FString NewFloorComponentName = FString::Printf(TEXT("%s Floors"), *Preset->RoomName);
	UInstancedStaticMeshComponent* NewFloorComponent = 
		NewObject<UInstancedStaticMeshComponent>(this, *NewFloorComponentName);
	NewFloorComponent->RegisterComponent();
	NewFloorComponent->SetStaticMesh(Preset->FloorMesh);

	AddInstanceComponent(NewFloorComponent);
	FloorMeshInstances.Add(PresetID, NewFloorComponent);


	FString NewWallsComponentName = FString::Printf(TEXT("%s Walls"), *Preset->RoomName);
	UInstancedStaticMeshComponent* NewWallsComponent = 
		NewObject<UInstancedStaticMeshComponent>(this, *NewWallsComponentName);
	NewWallsComponent->RegisterComponent();
	NewWallsComponent->SetStaticMesh(Preset->WallsMesh);

	AddInstanceComponent(NewWallsComponent);
	WallsMeshInstances.Add(PresetID, NewWallsComponent);

}

void ARuntimeDungeon::SpawnNewRoom(const int32 PresetID, const FVector Position)
{
	//if (!FDungeonUtilities::Get()->CheckRoomPresetByID(PresetID))
	if (!FDungeonUtilities::Get()->CheckRoomPresetByID(PresetID))
	{
		return;
	}

	if (!FloorMeshInstances.Contains(PresetID) &&
		!WallsMeshInstances.Contains(PresetID))
	{
		AddNewRoomInstanceMeshComponent(PresetID);
	}

	
	FTransform Transform(Position);
	WallsMeshInstances[PresetID]->AddInstance(Transform);
}
