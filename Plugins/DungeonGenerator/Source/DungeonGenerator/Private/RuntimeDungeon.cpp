// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeDungeon.h"


// Sets default values
ARuntimeDungeon::ARuntimeDungeon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RoomInstancesComponent = CreateDefaultSubobject
		<UInstancedStaticMeshComponent>(TEXT("Room Instances Component"));
	DoorInstancesComponent = CreateDefaultSubobject
		<UInstancedStaticMeshComponent>(TEXT("Door instances Component"));

	RoomInstancesComponent->SetupAttachment(RootComponent);
	DoorInstancesComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARuntimeDungeon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARuntimeDungeon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARuntimeDungeon::BuildDungeon(const int32 RoomsCount)
{
	DungeonDataRef = FDungeonUtils::Get()->GetDungeonDataAsset();
}

void ARuntimeDungeon::BuildRooms()
{
	if (!IsValid(DungeonDataRef.Get()))
	{
		return;
	}
	
	TArray<FTransform> Transforms;
	Transforms.Init({}, DungeonDataRef->PathLength);

	//RoomInstancesComponent->SetStaticMesh();

	for (int32 Index = 0; Index < DungeonDataRef->PathLength; Index++)
	{
		
	}
	
	RoomInstancesComponent->AddInstances(Transforms, true);
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
	
	DoorInstancesComponent->AddInstances(Transforms, true);
}

