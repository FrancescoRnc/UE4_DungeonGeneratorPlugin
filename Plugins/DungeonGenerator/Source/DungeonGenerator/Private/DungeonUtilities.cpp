// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Door.h"
#include "DungeonData.h"


DEFINE_LOG_CATEGORY(LogDunGenUtilities);


// IDungeonBuilder
IDungeonBuilder::~IDungeonBuilder()
{

}


// FDungeonUtils
FDungeonUtilities* FDungeonUtilities::Instance = nullptr;

FDungeonUtilities::FDungeonUtilities()
{
	Instance = this;
}

FFileReport FDungeonUtilities::CreateGenerationSettings()
{
	FFileReport Report;

	Report = CreateSerializedObject<UGenerationSettings>(CurrentSettings, TEXT("GenerationSettings"), CurrentSettingsDefaultFilePath);
	Report = CreateSerializedObject<UDungeonData>(CurrentSettings->DungeonDataRef, TEXT("DungeonData"), CurrentSettings->DungeonDataFilePath);

	return Report;
}

FFileReport FDungeonUtilities::LoadGenerationSettings(UGenerationSettings*& OutSettings)
{
	FFileReport Report = LoadSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);
	
	OutSettings = CurrentSettings;

	return Report;
}

UGenerationSettings* FDungeonUtilities::LoadGenerationSettings()
{
	UGenerationSettings* Settings = NewObject<UGenerationSettings>
		(
			GetTransientPackage(), TEXT("LoadedSettings"),
			EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
		);
	FFileReport Report = LoadSerializedObject<UGenerationSettings>(Settings, CurrentSettingsDefaultFilePath);

	return Settings;
}

FFileReport FDungeonUtilities::SaveGenerationSettings(UGenerationSettings*& OutSettings)
{
	FFileReport Report = SaveSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);;

	OutSettings = CurrentSettings;

	return Report;
}

TArray<URoomPreset*> FDungeonUtilities::GetPresetsArray() const
{
	TArray<URoomPreset*> OutArray;
	CurrentSettings->RoomPresetsRefMap.GenerateValueArray(OutArray);
	return OutArray;
}

void FDungeonUtilities::AddPresetReference(URoomPresetPtr NewPreset)
{
	NewPreset->PresetID = FMath::Rand();
	if (!CurrentSettings->RoomPresetsRefMap.Contains(NewPreset->PresetID))
	{
		CurrentSettings->RoomPresetsRefMap.Add(NewPreset->PresetID, NewPreset);
	}	
	if (!CurrentSettings->RoomPresetsPaths.Contains<FString>(NewPreset->GetPathName()))
	{
		CurrentSettings->RoomPresetsPaths.Add(NewPreset->GetPathName());
	}	

	SaveSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);
}

void FDungeonUtilities::AddPresetPath(const FString& NewPath)
{
	if (!CurrentSettings->RoomPresetsPaths.Contains<FString>(NewPath))
	{
		CurrentSettings->RoomPresetsPaths.Add(NewPath);
	}

	SaveSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);
}

void FDungeonUtilities::DeletePresetReference(const URoomPresetPtr NewPreset)
{	
	if (CurrentSettings->RoomPresetsRefMap.Contains(NewPreset->PresetID))
	{
		CurrentSettings->RoomPresetsRefMap.Remove(NewPreset->PresetID);
	}
	if (CurrentSettings->RoomPresetsPaths.Contains<FString>(NewPreset->GetPathName()))
	{
		CurrentSettings->RoomPresetsPaths.Remove(NewPreset->GetPathName());
	}

	SaveSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);
}

void FDungeonUtilities::DeletePresetPath(const FString& Path)
{
	if (CurrentSettings->RoomPresetsPaths.Contains<FString>(Path))
	{
		CurrentSettings->RoomPresetsPaths.Remove(Path);
	}

	SaveSerializedObject<UGenerationSettings>(CurrentSettings, CurrentSettingsDefaultFilePath);
}

void FDungeonUtilities::GetPresetsOnLoad()
{
	IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();

	// Let's load Presets from our already serialized PathArray.
	const TArray<FString> Paths = CurrentSettings->RoomPresetsPaths;
	const int32 Pathslength = Paths.Num();
	if (Pathslength == 0)
	{
		UE_LOG(LogDunGenUtilities, Error, TEXT("ERROR: There are no Presets to Load!"));
		return;
	}
	
	for (int32 Index = 0; Index < Pathslength; Index++)
	{
		const FAssetData Asset = AssetRegistry.GetAssetByObjectPath(*Paths[Index]);
		URoomPreset* NewPreset = Cast<URoomPreset>(Asset.GetAsset());

		if (!NewPreset)
		{
			UE_LOG(LogDunGenUtilities, Error, TEXT("ERROR: Preset File at %s not found!"), *Paths[Index]);
			CurrentSettings->RoomPresetsPaths.Remove(*Paths[Index]);
			continue;
		}

		AddPresetReference(NewPreset);
		UE_LOG(LogDunGenUtilities, Display, TEXT("Preset %s at %s succesfully registered!"),
			   *NewPreset->GetName(), *Paths[Index]);
	}

	UE_LOG(LogDunGenUtilities, Display, TEXT("Presets Load process Complete!"));
}

/*UDungeonData* FDungeonUtilities::SaveDungeonData(UGenerationSettings*& InSettings, const FDungeonInfo& Info)
{
	if (Info.State == EDungeonInfoState::NOVALID)
	{
		return nullptr;
	}

	InSettings->DungeonDataRef->Reset();
	InSettings->DungeonDataRef->SaveData(Info);

	SaveSerializedObject(InSettings->DungeonDataRef, InSettings->DungeonDataFilePath);

	return InSettings->DungeonDataRef;
}*/

