// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonUtilities.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Door.h"
#include "DungeonData.h"
#include "GenerationSettings.h"



// FDungeonUtils
FDungeonUtils* FDungeonUtils::Instance = nullptr;

FDungeonUtils::FDungeonUtils()
{
	Instance = this;
}

void FDungeonUtils::GetPresetsOnLoad()
{
	RoomPresets.Empty();
	
	TArray<FAssetData> Assets;
	FAssetRegistryModule::GetRegistry().GetAssetsByPath(PrefabricsPath, Assets);
	for (FAssetData Asset : Assets)
	{
		URoomPresetPtr NewPreset = Cast<URoomPreset>(Asset.GetAsset());
		AddAssetReference(NewPreset);		
	}

	TArray<FAssetData> DungeonAssets;
	FAssetRegistryModule::GetRegistry().GetAssetsByPath(DungeonDataPath, DungeonAssets);
	if (DungeonAssets.Num() <= 0)
	{
		return;
	}

	CurrentDungeonFile = Cast<UDungeonData>(DungeonAssets[0].GetAsset());
	if (IsValid(CurrentDungeonFile))
	{
		CurrentDungeonPath = *CurrentDungeonFile->GetPathName();
		DungeonPackage = CurrentDungeonFile->GetPackage();
	}
	
	const bool Rescan = RescanAssetReferences();
	UE_LOG(LogTemp, Warning, TEXT("Asset References Got? %d"), Rescan);
}

void FDungeonUtils::AddAssetReference(URoomPresetPtr NewPreset)
{	
	NewPreset->PresetID = FMath::Rand();
	RoomPresets.Add(NewPreset);
	RoomPresetsMap.Add(NewPreset->PresetID, NewPreset);
	RoomPresetPaths.Add(*NewPreset->GetPathName());
	PresetPackages.Add(*NewPreset->GetPackage()->GetName(),
				NewPreset->GetPackage());
}

void FDungeonUtils::DeleteAssetReference(URoomPresetPtr NewPreset)
{
	if (RoomPresets.Contains(NewPreset) && RoomPresetsMap.Contains(NewPreset->PresetID))
	{
		RoomPresets.Remove(NewPreset);
		RoomPresetsMap.Remove(NewPreset->PresetID);
		RoomPresetPaths.Remove(*NewPreset->GetPathName());
		PresetPackages.Remove(*NewPreset->GetPackage()->GetName());
	}
}

const bool FDungeonUtils::RescanAssetReferences() const
{
	TArray<FAssetData> Assets;
	if (FAssetRegistryModule::GetRegistry().GetAssetsByPath(PrefabricsPath, Assets, true))
	{
		for (const FAssetData& Asset : Assets)
		{
			if (const URoomPresetPtr Preset = Cast<URoomPreset>(Asset.GetAsset()))
			{
				if (!RoomPresets.Contains(Preset))
				{
					return false;
				}
			}
		}
	}
	TArray<FAssetData> DungeonAssets;
	if (FAssetRegistryModule::GetRegistry().GetAssetsByPath(PrefabricsPath, DungeonAssets, true))
	{
		if (DungeonAssets.Num() > 0)
		{
			const URoomPresetPtr Dungeon = Cast<URoomPreset>(DungeonAssets[0].GetAsset());
			if (IsValid(Dungeon))
			{
				return true;
			}
		}		
	}
	return false;
}

URoomPresetPtr FDungeonUtils::GetRoomPresetByName(const FName Name) const
{
	for (const URoomPresetPtr Preset : RoomPresets)
	{
		if (Preset->GetFName() == Name)
		{
			return Preset;
		}
	}
	return nullptr;
}

URoomPresetPtr FDungeonUtils::GetRoomPresetByPath(const FName Path) const
{
	const FAssetData Asset = FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(Path);
	if (Asset.IsValid())
	{
		if (const URoomPresetPtr Preset = Cast<URoomPreset>(Asset.GetAsset()))
		{
			return Preset;
		}
	}		
	return nullptr;
}

/*UGenerationSettings* FDungeonUtils::CreateSettingsFile(const FName FileDirectoryPath, EFileResultStatus& Result)
{
	if (FileDirectoryPath == "")
	{
		Result = EFileResultStatus::NONE;
		return nullptr;
	}

	const FString AssetName = TEXT("Settings");
	const FString Path = FileDirectoryPath.ToString();
	const FString AssetPath = FString::Printf(TEXT("%s/%s"), *Path, *AssetName);
	const FString PackageName = FString::Printf(TEXT("%s_Pkg"), *AssetName);
	const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Path, *PackageName);
	const FString FullPathName = FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);

	UGenerationSettings* Object = nullptr;

	if (FPackageName::DoesPackageExist(FullPathName))
	{
		Object = LoadSettings(*FullPathName, Result);
		return Object;
	}

	UPackage* Package = CreatePackage(*PackagePath);
	Object = NewObject<UGenerationSettings>(Package, *AssetName, RF_Public | RF_Standalone);

	SaveAsset(Object, Result);	

	Result = EFileResultStatus::CREATED;
	return Object;
}*/

FFileReport FDungeonUtils::CreateSettingsFile(const FName FileDirectoryPath, UGenerationSettings* OutSettingsFile)
{
	FFileReport Report;

	if (FileDirectoryPath == "")
	{
		return Report;
	}

	const FString AssetName = TEXT("Settings");
	const FString Path = FileDirectoryPath.ToString();
	const FString AssetPath = FString::Printf(TEXT("%s/%s"), *Path, *AssetName);
	const FString PackageName = FString::Printf(TEXT("%s_Pkg"), *AssetName);
	const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Path, *PackageName);
	const FString FullPathName = FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);

	//UGenerationSettings* Object = nullptr;

	if (FPackageName::DoesPackageExist(FullPathName))
	{
		Report = LoadSettings(*FullPathName, OutSettingsFile);
		//OutSettingsFile = Object;
		return Report;
	}

	UPackage* Package = CreatePackage(*PackagePath);
	OutSettingsFile = NewObject<UGenerationSettings>(Package, *AssetName, RF_Public | RF_Standalone);

	SaveAsset(OutSettingsFile);

	//OutSettingsFile = Object;

	Report = FFileReport
	{
		ECommandStatusType::VALID,
		EFileResultStatus::CREATED
	};
	return Report;
}

FFileReport FDungeonUtils::SaveAsset(UObject* Asset)
{
	FFileReport Report;

	if (!IsValid(Asset))
	{
		return Report;
	}

	UPackage* Package = Asset->GetPackage();
	const FString Filename = FPackageName::LongPackageNameToFilename(
		Package->GetPathName(), FPackageName::GetAssetPackageExtension());

	UPackage::SavePackage(Package, Asset, RF_Public | RF_Standalone, *Filename);
	FAssetRegistryModule::AssetCreated(Asset);
	Package->MarkPackageDirty();

	//FArchive Archive;
	//FMemoryWriter Writer = FMemoryWriter(Asset.Data);
	//Asset->Serialize(Writer);
	//UPackage::SavePackage(Package, Asset, RF_Public | RF_Standalone, *Filename);

	Report = FFileReport
	{
		ECommandStatusType::VALID,
		EFileResultStatus::SAVED
	};
	return Report;
}

/*UGenerationSettings* FDungeonUtils::LoadSettings(const FName FilePath, EFileResultStatus& Result) const
{
	const FAssetData Asset = FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(FilePath);
	if (Asset.IsValid())
	{
		UGenerationSettings* Settings = Cast<UGenerationSettings>(Asset.GetAsset());
		if (Settings)
		{
			Result = EFileResultStatus::LOADED;
			return Settings;
		}
	}

	Result = EFileResultStatus::NONE;
	return nullptr;
}*/

FFileReport FDungeonUtils::LoadSettings(const FName FilePath, UGenerationSettings* OutSettingsFile) const
{
	FFileReport Report;

	//LoadObject<UGenerationSettings>(nullptr, TEXT(""), TEXT(""));
	const FAssetData Asset = FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(FilePath);
	if (Asset.IsValid())
	{
		UGenerationSettings* Settings = Cast<UGenerationSettings>(Asset.GetAsset());
		if (Settings)
		{
			OutSettingsFile = Settings;
			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::LOADED
			};
			return Report;			
		}
	}

	return Report;
}

FFileReport FDungeonUtils::DeleteAsset(UObject* Asset)
{
	FFileReport Report;

	FString PkgName = Asset->GetPackage()->GetName();
	bool FileNotExists = !FPackageName::DoesPackageExist(PkgName);
	if (!IsValid(Asset) || FileNotExists)
	{
		return Report;
	}

	UPackage* Package = Asset->GetPackage();
	Asset->MarkPendingKill();
	Package->MarkPendingKill();
	GEngine->ForceGarbageCollection(true);
	FAssetRegistryModule::AssetDeleted(Asset);
	FAssetRegistryModule::PackageDeleted(Package);

	// FIX DELETE PROCESS

	Report = FFileReport
	{
		ECommandStatusType::VALID,
		EFileResultStatus::DELETED
	};
	return Report;
}

bool FDungeonUtils::RGCommandMakeRoom(const FString AssetName)
{
	if (AssetName == "")
	{
		UE_LOG(LogTemp, Display, TEXT("THIS NAME IS NO VALID"));
		return false;
	}
	if (RoomPresets.Contains(GetRoomPresetByName(*AssetName)))
	{
		UE_LOG(LogTemp, Display, TEXT("ASSET OF NAME %s ALREADY CREATED. PLEASE TRY WITH A DIFFERENT NAME"), *AssetName);
		return false;
	}

	const FString Path = PrefabricsPath.ToString();
	const FString PackageName = FString::Printf(TEXT("%s_Pkg"), *AssetName);
	const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Path, *PackageName);
	const FString FullPathName = FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);

	UPackage* Package = CreatePackage(*PackagePath);
	URoomPreset* NewRoom = NewObject<URoomPreset>(Package, *AssetName, RF_Public | RF_Standalone);
	//NewRoom->AddToRoot();
	
	AddAssetReference(NewRoom);

	const FString Filename = FPackageName::LongPackageNameToFilename(
		Package->GetName(), FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(Package, NewRoom, RF_Public | RF_Standalone, *Filename);
	FAssetRegistryModule::AssetCreated(NewRoom);

	Package->MarkPackageDirty();

	return true;
}

UDungeonData* FDungeonUtils::SaveDungeonData(const FDungeonInfo& Info)
{
	if (Info.State == EDungeonInfoState::NOVALID)
	{
		return nullptr;
	}

	const FString Path = DungeonDataPath.ToString();
	const FString AssetName = TEXT("CurrentDungeon");
	const FString PackageName = FString::Printf(TEXT("%s_Pkg"), *AssetName);
	const FString PackagePath = FString::Printf(TEXT("%s/%s"), *Path, *PackageName);
	const FString FullPathName = FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);

	UDungeonData* Data;
	UPackage* Package;
	if (FAssetRegistryModule::GetRegistry().PathExists(FullPathName))
	{
		const FAssetData AssetData = FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(*PackagePath);
		Package = AssetData.GetPackage();
		Data = Cast<UDungeonData>(AssetData.GetAsset());
	}
	else
	{
		Package = CreatePackage(*PackagePath);
		Data = NewObject<UDungeonData>(Package, *AssetName, RF_Public | RF_Standalone);
	}

	// Generate Data
	Data->GridSize = Info.GridSize;
	Data->PathLength = Info.RoomsInfo.Num();
	Data->GridScheme = Info.GridScheme;
	Data->RoomsPresetID.Init(-1, Data->PathLength);
	Data->RoomsPresetPaths.Init(NAME_None, Data->PathLength);
	Data->RoomsCoordinate.Init({0,0,0}, Data->PathLength);
	Data->RoomsGridIndex.Init(-1, Data->PathLength);
	for (int32 Index = 0; Index < Data->PathLength; Index++)
	{
		const FRoomInfo RoomInfo = Info.RoomsInfo[Index];
		Data->RoomsPresetID[Index] = RoomInfo.PresetID;
		Data->RoomsPresetPaths[Index] = RoomInfo.PresetPath;
		Data->RoomsCoordinate[Index] = RoomInfo.CoordinateInGrid;
		Data->RoomsGridIndex[Index] = RoomInfo.IndexInGrid;
		const int32 RoomsCount = RoomInfo.DoorsInfo.Num();
		for (int32 DoorIndex = 0; DoorIndex < RoomsCount; DoorIndex++)
		{
			const int32 IntDirection = static_cast<int32>
				(RoomInfo.DoorsInfo[DoorIndex].Direction);
			Data->DoorsDirection.Add(IntDirection);
			Data->DoorsSourceRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].SourceRoomIndex);
			Data->DoorsNextRoomIndex.Add(RoomInfo.DoorsInfo[DoorIndex].NextRoomIndex);
		}		
	}
	// ----------------
	 
	CurrentDungeonFile = Data;
	CurrentDungeonPath = *Data->GetPathName();
	DungeonPackage = Package;
	
	const FString Filename = FPackageName::LongPackageNameToFilename(
		Package->GetName(), FPackageName::GetAssetPackageExtension());
    UPackage::SavePackage(Package, Data, RF_Public | RF_Standalone, *Filename);
	FAssetRegistryModule::AssetCreated(Data);

	Package->MarkPackageDirty();
	
	return Data;
}

UDungeonData* FDungeonUtils::GetDungeonDataAsset() const
{
	const FAssetData AssetData = FAssetRegistryModule::GetRegistry()
		.GetAssetByObjectPath(CurrentDungeonPath);
	if (UDungeonData* Data = Cast<UDungeonData>(AssetData.GetAsset()))
	{
		return Data;
	}
	return nullptr;
}
