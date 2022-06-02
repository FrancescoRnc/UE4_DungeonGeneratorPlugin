// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "RoomData.h"
#include "Misc/FileHelper.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/ObjectReader.h"
#include "Serialization/ObjectWriter.h"
#include "UObject/NoExportTypes.h"
#include "GenerationSettings.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGenSettings, All, All);

using URoomPresetPtr = URoomPreset*;


/**
 * This Object will be loaded at Plugin Startup, 
 * and its parameters will be used by all Plugin Features.
 */
UCLASS()
class DUNGEONGENERATOR_API UGenerationSettings : public UObject
{
	GENERATED_BODY()


public:
	UGenerationSettings();

	UPROPERTY(VisibleAnywhere)
	FString DungeonDataFilePath = TEXT("/DungeonGenerator/Dungeon/Data/DungeonData.bin");

	UPROPERTY(VisibleAnywhere)
	FString RoomPresetFolderPath = TEXT("/DungeonGenerator/Dungeon/Presets/");

	UPROPERTY(VisibleAnywhere)
	uint32 InitialRoomsCount = 0;

	UPROPERTY(VisibleAnywhere)
	uint32 InitialPresetFilesCount = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> RoomPresetsPaths{};

	UPROPERTY(VisibleAnywhere)
	UDungeonData* DungeonDataRef = nullptr;

	UPROPERTY(VisibleAnywhere)
	URoomPreset* SelectedPresetRef = nullptr;

	UPROPERTY(VisibleAnywhere)
	TMap<int32, URoomPreset*> RoomPresetsRefMap{};


	void Serialize(FArchive& Ar) override;


private:
	UDungeonData* LoadDungeonDataReference();
	TMap<int32, URoomPreset*> LoadRoomPresetReferences();
};
