// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomData.h"
#include "Door.h"
#include "EditorRoom.generated.h"


/**
* Actor used for in World representation and Editing purpose of a Room, by using a URoomPreset Object.
*/
UCLASS()
class DUNGEONGENERATOR_API AEditorRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEditorRoom();

	UPROPERTY(BlueprintReadWrite, EditAnywhere,   Category="Dungeon Room Editing")
	FName RoomName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dungeon Room Editing")
	UStaticMeshComponent* FloorMeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dungeon Room Editing")
	UStaticMeshComponent* WallsMeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dungeon Room Editing")
	UInstancedStaticMeshComponent* DoorsMeshComponent;


protected:
	FRoomInfo RoomInfo;


public:
	FVector GetCardinalLocation(const EWorldDirection Direction) const;

	void Preview(const FRoomInfo& Info);

	void OverwritePresetData(URoomPreset* InPreset) const;
};
