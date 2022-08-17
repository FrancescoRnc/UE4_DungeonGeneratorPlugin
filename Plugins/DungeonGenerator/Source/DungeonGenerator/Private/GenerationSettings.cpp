// Fill out your copyright notice in the Description page of Project Settings.

#include "GenerationSettings.h"
#include "DungeonUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"


DEFINE_LOG_CATEGORY(LogGenSettings);


UGenerationSettings::UGenerationSettings()
{
	RoomPresetsPaths = {};
	RoomPresetsRefMap = {};
}

UDungeonData* UGenerationSettings::LoadDungeonDataReference()
{
	//DungeonData* DungeonData = NewObject<UDungeonData>
	DungeonDataRef = NewObject<UDungeonData>
	(
		this, TEXT("DungeonData001"), 
		 EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
	);

	FDungeonUtilities::LoadSerializedObject<UDungeonData>(DungeonDataRef, DungeonDataFilePath);

	//TArray<uint8> RawData;
	//if (FFileHelper::LoadFileToArray(RawData, *DungeonDataFilePath))
	//{
	//	FObjectReader Archive(DungeonData, RawData);
	//}
	//else
	//{
	//	FObjectWriter Archive(DungeonData, RawData);
	//	if (!FFileHelper::SaveArrayToFile(RawData, *DungeonDataFilePath))
	//	{
	//		UE_LOG(LogGenSettings, Error, TEXT("ERROR during DungeonData Creation!"));
	//	}
	//}
	
	//DungeonDataRef = DungeonData;

	return DungeonDataRef;
}

TMap<int32, URoomPreset*> UGenerationSettings::LoadRoomPresetReferences()
{
	TMap<int32, URoomPreset*> RoomPresets{};
	IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();

	const int32 PathsLength = RoomPresetsPaths.Num();
	if (PathsLength <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Presets to load!"));
		return RoomPresets;
	}

	RoomPresetsRefMap.Empty();
	for (const FString Path : RoomPresetsPaths)
	{
		const FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(*Path);
		if (URoomPreset* Asset = Cast<URoomPreset>(AssetData.GetAsset()))
		{
			RoomPresets.Add(Asset->PresetID, Asset);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Preset not found at %s: %d"), *Path);
		}
	}
	
	RoomPresetsRefMap = RoomPresets;
	InitialPresetFilesCount = RoomPresets.Num();

	return RoomPresets;
}

URoomPreset* UGenerationSettings::GetPresetByPath(const FString& Path)
{
	IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();
	const FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(*Path);
	if (URoomPreset* Asset = Cast<URoomPreset>(AssetData.GetAsset()))
	{
		return Asset;
	}

	return nullptr;
}

void UGenerationSettings::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	// Fix: sometimes this gives an access violation
	if (Ar.IsSaving())
	{
		int32 PresetsCount = RoomPresetsPaths.Num();
		Ar << DungeonDataFilePath;
		Ar << RoomPresetFolderPath;
		Ar << InitialRoomsCount;
		Ar << PresetsCount;
		for (FString Path : RoomPresetsPaths)
		{
			Ar << Path;
		}
	}
	else if (Ar.IsLoading())
	{
		FString LoadedDungeonDataFolderPath;
		FString LoadedRoomPresetFolderPath;
		uint32  LoadedRoomsFilesCount;
		int32  LoadedInitialPresetFilesCount;

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

		DungeonDataFilePath = LoadedDungeonDataFolderPath;
		RoomPresetFolderPath = LoadedRoomPresetFolderPath;
		InitialRoomsCount = LoadedRoomsFilesCount;
	}
	else if (Ar.IsCooking())
	{
		UE_LOG(LogTemp, Warning, TEXT("Generation Settings is Cooking..."));
	}
}