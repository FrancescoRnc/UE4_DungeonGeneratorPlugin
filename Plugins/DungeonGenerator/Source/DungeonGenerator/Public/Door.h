// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"


/**
* 
*/
enum class DUNGEONGENERATOR_API EWorldDirection : uint8
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	MAX = 4 // DO NOT USE
};

static const TMap<EWorldDirection, FName> WorldDirectionNames =
{
	{ EWorldDirection::NORTH,	TEXT("North") },
	{ EWorldDirection::EAST,	TEXT("East") },
	{ EWorldDirection::SOUTH,	TEXT("South") },
	{ EWorldDirection::WEST,	TEXT("West") }
};

/**
* 
*/
struct DUNGEONGENERATOR_API FDoorInfo
{
	FName SourceRoomName;
	FName NextRoomName;	
	int32 SourceRoomIndex;
	int32 NextRoomIndex;
	EWorldDirection Direction;
	EWorldDirection OppositeDoorDirection;
};

// - Future Reimplementation --------------------
UCLASS()
class DUNGEONGENERATOR_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

};
