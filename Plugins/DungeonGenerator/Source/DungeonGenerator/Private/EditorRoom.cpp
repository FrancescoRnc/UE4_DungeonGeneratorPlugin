// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorRoom.h"
#include "DungeonUtilities.h"


// Sets default values
AEditorRoom::AEditorRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	FloorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Mesh"));
	WallsMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Walls Mesh"));
	DoorsMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Doors Mesh"));

	FloorMeshComponent->SetupAttachment(RootComponent);
	WallsMeshComponent->SetupAttachment(RootComponent);
	DoorsMeshComponent->SetupAttachment(RootComponent);
}


FVector AEditorRoom::GetCardinalLocation(const EWorldDirection Direction) const
{
	FName DirectionName(NAME_None);
	switch (Direction)
	{
		case EWorldDirection::NORTH:
		DirectionName = TEXT("North");
		break;
		case EWorldDirection::EAST:
		DirectionName = TEXT("East");
		break;
		case EWorldDirection::SOUTH:
		DirectionName = TEXT("South");
		break;
		case EWorldDirection::WEST:
		DirectionName = TEXT("West");
		break;
		default:
		break;
	}

	return FloorMeshComponent->GetSocketLocation(DirectionName);
}

void AEditorRoom::Preview(const FRoomInfo& Info)
{
	// Remove Doors Instances from the Scene before starting this Preview
	DoorsMeshComponent->ClearInstances();

	const URoomPreset* SelectedPreset = FDungeonUtilities::Get()->GetSelectedPreset();
	if (!SelectedPreset)
	{
		UE_LOG(LogTemp, Error, TEXT("No Preset selected! Please select one of your Room Preset Files."));
		return;
	}

	// Setting Static Meshes and other data from the URoomPreset reference to this Actor.
	RoomName = SelectedPreset->RoomName;
	FloorMeshComponent->SetStaticMesh(SelectedPreset->FloorMesh);
	WallsMeshComponent->SetStaticMesh(SelectedPreset->WallsMesh);
	DoorsMeshComponent->SetStaticMesh(SelectedPreset->DoorsMesh);

	RoomInfo = Info;

	// Check if the main Mesh contains any socket and set all Door Instances in every location got.
	if (FloorMeshComponent->HasAnySockets())
	{
		const TArray<FName> SocketNames = FloorMeshComponent->GetAllSocketNames();
		TArray<FTransform> Transforms;
		Transforms.Init({}, SocketNames.Num());
		for (int32 Index = 0; Index < SocketNames.Num(); Index++)
		{
			const FVector SocketLocation = FloorMeshComponent->GetSocketLocation(SocketNames[Index]);
			const FRotator SocketRotation = FloorMeshComponent->GetSocketRotation(SocketNames[Index]);
			const FTransform NewTransform(SocketRotation, SocketLocation, FVector::OneVector);
			Transforms[Index] = NewTransform;
		}
		DoorsMeshComponent->AddInstances(Transforms, false);
	}
}

void AEditorRoom::OverwritePresetData(URoomPreset* InPreset) const
{
	InPreset->FloorMesh = FloorMeshComponent->GetStaticMesh();
	InPreset->WallsMesh = WallsMeshComponent->GetStaticMesh();
	InPreset->DoorsMesh = DoorsMeshComponent->GetStaticMesh();
	InPreset->RoomName = RoomName;
}
