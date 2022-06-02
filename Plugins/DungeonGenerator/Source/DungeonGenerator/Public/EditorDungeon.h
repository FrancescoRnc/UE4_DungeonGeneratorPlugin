// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "EditorDungeon.generated.h"


/**
* Actor used to edit an URoomPreset Asset on Editor World 
*/
UCLASS()
class DUNGEONGENERATOR_API AEditorDungeon : public AActor, public IDungeonBuilder
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditorDungeon();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UInstancedStaticMeshComponent* DoorInstancesComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<int32, UInstancedStaticMeshComponent*> FloorMeshInstances{};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<int32, UInstancedStaticMeshComponent*> WallsMeshInstances{};
	
	/**
	* Used to separate all Floor and Walls Mesh instances
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dungeon Editing")
	float InstancesSpaceGap = 5000.f;


protected:
	/**
	* @param PresetID: Given an ID of a RoomPreset, Add a new Instanced Mesh Component with this RoomPreset's Static Mesh.
	*/
	void AddNewRoomInstancedMeshComponents(const int32 PresetID);

	/**
	* @param PresetID: ID used to search, or add if no exists, the right Instanced Mesh Component for instancing a new Mesh;
	* @param Position: Location to spawn the new Mesh instance.
	*/
	void SpawnNewRoom(const int32 PresetID, const FVector Position);


public:
	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;

	UFUNCTION(BlueprintCallable)
	void Build();

	/**
	* Function used to clear all already spawned Mesh instances of all Instanced Mesh Components.
	*/
	void ClearInstances();


private:
	TSoftObjectPtr<UDungeonData> DungeonDataRef;
	FDungeonInfo DungeonInfo{};	
};
