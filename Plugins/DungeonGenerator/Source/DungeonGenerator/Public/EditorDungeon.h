// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "EditorDungeon.generated.h"

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InstancesSpaceGap = 5000.f;

	protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	void AddNewRoomInstancedMeshComponents(const int32 PresetID);
	void SpawnNewRoom(const int32 PresetID, const FVector Position);


	public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;

	UFUNCTION(BlueprintCallable)
	void Build();

	void ClearInstances();

	private:
	TSoftObjectPtr<UDungeonData> DungeonDataRef;
	FDungeonInfo DungeonInfo{};	
};
