// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "RuntimeDungeon.generated.h"

UCLASS()
class DUNGEONGENERATOR_API ARuntimeDungeon : public AActor, public IDungeonBuilder
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARuntimeDungeon();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UInstancedStaticMeshComponent* RoomInstancesComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UInstancedStaticMeshComponent* DoorInstancesComponent = nullptr;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;
	
	//UFUNCTION(BlueprintCallable)
	//void Build();

	private:
	TSoftObjectPtr<UDungeonData> DungeonDataRef;
};
