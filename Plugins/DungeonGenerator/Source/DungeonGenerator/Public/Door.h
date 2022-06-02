// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"


/**
* 
*/
enum class EWorldDirection : uint8
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	MAX = 4 // DO NOT USE
};

/**
* 
*/
struct FDoorInfo
{
	FName SourceRoomName;
	FName NextRoomName;	
	int32 SourceRoomIndex;
	int32 NextRoomIndex;
	EWorldDirection Direction;
};

// - Future Reimplementation --------------------
UCLASS()
class DUNGEONGENERATOR_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UBoxComponent* InteractionCollider;

	UPROPERTY(VisibleAnywhere)
	class ADungeonRoom* LinkedRoom;

	FDoorInfo DoorInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	void Teleport(class ACharacter* Character);
};
