// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeDungeonExample.h"

// Sets default values
ARuntimeDungeonExample::ARuntimeDungeonExample()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARuntimeDungeonExample::BeginPlay()
{
	Super::BeginPlay();
	Build();
}

void ARuntimeDungeonExample::Build()
{
	TSharedPtr<FDefaultGeneratorMethod> Method = MakeShared<FDefaultGeneratorMethod>();
	FDungeonDataGenerator DungeonGenerator(Method.Get());
	DungeonGenerator.BuildDungeon(10);
	DungeonGenerator.BuildRooms();
	DungeonGenerator.BuildDoors();
}

