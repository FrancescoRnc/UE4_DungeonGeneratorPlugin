// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomData.h"
#include "Door.h"
#include "DungeonRoom.generated.h"


/**
* 
*/
UCLASS(BlueprintType)
class DUNGEONGENERATOR_API ADungeonRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonRoom();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* FloorMeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* WallsMeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UInstancedStaticMeshComponent* DoorsMeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<ADoor*> DoorsRef;
	
	FRoomInfo RoomInfo;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	FVector GetCardinalLocation(const EWorldDirection Direction) const;
};
