// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeDungeon.h"


// Sets default values
ARuntimeDungeon::ARuntimeDungeon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	
}

void ARuntimeDungeon::BuildDoors()
{
	
}