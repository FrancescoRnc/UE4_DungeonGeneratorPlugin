// Fill out your copyright notice in the Description page of Project Settings.



#include "GenerationSettings.h"


DEFINE_LOG_CATEGORY(LogGenSettings);


UGenerationSettings::UGenerationSettings()
{
	RoomPresetsPaths = {};
	RoomPresetsRefMap = {};
}

UDungeonData* UGenerationSettings::LoadDungeonDataReference()
{
	UDungeonData* DungeonData = NewObject<UDungeonData>
	(
		this, TEXT("DungeonData"), 
		 EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
	);

	TArray<uint8> RawData;
	if (FFileHelper::LoadFileToArray(RawData, *DungeonDataFilePath))
	{
		FObjectReader Archive(DungeonData, RawData);
	}
	else
	{
		FObjectWriter Archive(DungeonData, RawData);
		if (!FFileHelper::SaveArrayToFile(RawData, *DungeonDataFilePath))
		{
			UE_LOG(LogGenSettings, Error, TEXT("ERROR during DungeonData Creation!"));
		}
	}
	
	DungeonDataRef = DungeonData;

	return DungeonData;
}

TMap<int32, URoomPreset*> UGenerationSettings::LoadRoomPresetReferences()
{
	TMap<int32, URoomPreset*> RoomPresets{};

	RoomPresetsRefMap = RoomPresets;
	InitialPresetFilesCount = RoomPresets.Num();

	return RoomPresets;
}

void UGenerationSettings::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int32 PresetsCount = RoomPresetsPaths.Num();
		Ar << DungeonDataFilePath;
		Ar << RoomPresetFolderPath;
		Ar << InitialRoomsCount;
		Ar << PresetsCount;
		//Ar << InitialPresetFilesCount;
		for (FString Path : RoomPresetsPaths)
		{
			Ar << Path;
		}
		//Ar << RoomPresetsPaths;

		//UE_LOG(LogGenSettings, Warning, TEXT("Serialization Is Saving"), Ar.IsSaving());
	}
	else if (Ar.IsLoading())
	{
		FString LoadedDungeonDataFolderPath;
		FString LoadedRoomPresetFolderPath;
		uint32  LoadedRoomsFilesCount;
		int32  LoadedInitialPresetFilesCount;
		//TArray<FString> LoadedRoomPresetsPaths;

		Ar << LoadedDungeonDataFolderPath;
		Ar << LoadedRoomPresetFolderPath;
		Ar << LoadedRoomsFilesCount;
		Ar << LoadedInitialPresetFilesCount;
		for (int32 Index = 0; Index < LoadedInitialPresetFilesCount; Index++)
		{
			FString Path;
			Ar << Path;
			RoomPresetsPaths.Add(Path);
		}
		// AGGIUSTA LA SERIALIZZAZIONE
		//LoadedRoomPresetsPaths.BulkSerialize(Ar);
		//Ar.Serialize(LoadedRoomPresetsPaths, 0);
		//Ar << LoadedRoomPresetsPaths;

		DungeonDataFilePath = LoadedDungeonDataFolderPath;
		RoomPresetFolderPath = LoadedRoomPresetFolderPath;
		InitialRoomsCount = LoadedRoomsFilesCount;
		//InitialPresetFilesCount = LoadedInitialPresetFilesCount;
		//RoomPresetsPaths = LoadedRoomPresetsPaths;

		//UE_LOG(LogGenSettings, Warning, TEXT("Serialization Is Loading"), Ar.IsLoading());
	}
}