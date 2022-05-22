// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenerationSettings.h"
#include "RoomData.h"


using URoomPresetPtr = class URoomPreset*;
//using URoomPresetPtr = TSoftObjectPtr<class URoomPreset>;
using URoomPresetRef = TSharedRef<URoomPreset>;

using FDungeonUtilities = class FDungeonUtils2;


enum class ECommandStatusType : uint8
{
	ERROR		= 0,
	VALID		= 1,
	WARNING		= 2,
	MAX			= 3
};

enum class EFileResultStatus : uint8
{
	NONE		= 0,
	CREATED		= 1,
	LOADED		= 2,
	MODIFIED	= 3,
	SAVED		= 4,
	DELETED		= 5,
	MAX			= 6
};

struct FFileReport
{
	ECommandStatusType CommandStatus	= ECommandStatusType::ERROR;
	EFileResultStatus ResultStatus		= EFileResultStatus::NONE;
};


/**
 * IDungeonBuilder Interface
 */
class IDungeonBuilder
{
public:
	//virtual ~IDungeonBuilder() = 0;

	virtual void BuildDungeon(const int32 RoomsCount) = 0;
	virtual void BuildRooms() = 0;
	virtual void BuildDoors() = 0;
	
};


/**
 * FDungeonUtils Class
 */
class FDungeonUtils
{
public:	
	FDungeonUtils();	

	//FName PrefabricsPath = TEXT("/Game/Prefabrics");
	//FName DungeonDataPath = TEXT("/Game/DungeonData");
	//int32 DungeonRoomsCount = 0;

	UGenerationSettings* CurrentSettings = nullptr;

	//TMap<int32, URoomPresetPtr> RoomPresetsMap{};
	TMap<FName, URoomPresetPtr> RoomPresetAssetsMap{};
	TMap<int32, FName> RoomPresetsMap{};
	TArray<URoomPresetPtr> RoomPresets{};
	TArray<FName> RoomPresetPaths{};
	URoomPresetPtr SelectedPreset = nullptr;

	class UDungeonData* CurrentDungeonFile = nullptr;
	FName CurrentDungeonPath = NAME_None;
	//UPackage* DungeonPackage = nullptr;
	

	void GetPresetsOnLoad();
	void AddAssetReference(URoomPresetPtr NewPreset);
	void DeleteAssetReference(URoomPresetPtr NewPreset);

	
	const bool RescanAssetReferences() const;
	bool RGCommandMakeRoom(const FString AssetName);
	UDungeonData* SaveDungeonData(const struct FDungeonInfo& Info);
	UDungeonData* GetDungeonDataAsset() const;
	
	URoomPresetPtr GetRoomPresetByName(const FName Name) const;
	URoomPresetPtr GetRoomPresetByPath(const FString Path) const;

	FORCEINLINE FName GetRoomPresetPathByID(const int32 ID) const
	{
		return RoomPresetsMap[ID];
	}

	FORCEINLINE bool CheckRoomPresetByID(const int32 ID) const
	{
		return RoomPresetsMap.Contains(ID);
	}
	
	FORCEINLINE TArray<URoomPresetPtr> GetRoomPresets() const
	{
		return RoomPresets;
	}

	FORCEINLINE URoomPresetPtr GetSelectedPreset() const
	{
		return SelectedPreset;
	}

	FORCEINLINE int32 GetPresetsCount() const
	{
		if (RoomPresets.Num() != RoomPresetsMap.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("ERROR: Presets Count between Array and Map containers is not equal!"));
		}
		return RoomPresets.Num();
	}

	//class UGenerationSettings* CreateSettingsFile(const FName FileDirectoryPath, EFileResultStatus& Result);	
	//UGenerationSettings* LoadSettings(const FName FilePath, EFileResultStatus& Result) const;

	FFileReport CreateSettingsFile(const FName FileDirectoryPath, class UGenerationSettings* OutSettingsFile);
	FFileReport LoadSettings(const FName FilePath, UGenerationSettings* OutSettingsFile) const;

	FFileReport CreateGenerationSettings();
	FFileReport LoadGenerationSettings();
	FFileReport SaveGenerationSettings();

	FFileReport SaveAsset(UObject* Asset);
	FFileReport DeleteAsset(UObject* Asset);

	
	template<class NewObjectClass>
	FFileReport CreateSerializedObject(NewObjectClass*& OutObjectRef, const FName& ObjectName, const FString& FilePath)
	{
		FFileReport Report;

		OutObjectRef = NewObject<NewObjectClass>
		(
			GetTransientPackage(), ObjectName,
			EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
		);

		Report = LoadSerializedObject(OutObjectRef, FilePath);
		if (Report.CommandStatus == ECommandStatusType::ERROR)
		{
			Report = SaveSerializedObject(OutObjectRef, FilePath);
		}

		/*TArray<uint8> RawData;
		if (FFileHelper::LoadFileToArray(RawData, *FilePath))
		{
			FObjectReader Archive(OutObjectRef, RawData);

			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::LOADED
			};
		}
		else
		{
			FObjectWriter Archive(OutObjectRef, RawData);
			if (FFileHelper::SaveArrayToFile(RawData, *FilePath))
			{
				Report = FFileReport
				{
					ECommandStatusType::VALID,
					EFileResultStatus::CREATED
				};
			}
			else
			{
				UE_LOG(LogGenSettings, Error, TEXT("ERROR during Object Creation!"));
			}						
		}*/

		return Report;
	}

	template<class NewObjectClass>
	FFileReport LoadSerializedObject(NewObjectClass*& ObjectToLoad, const FString& FilePath)
	{
		FFileReport Report;

		TArray<uint8> Data;

		if (FFileHelper::LoadFileToArray(Data, *FilePath))
		{
			FObjectReader Archive(ObjectToLoad, Data);

			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::LOADED
			};
		}

		return Report;
	}

	template<class NewObjectClass>
	FFileReport SaveSerializedObject(NewObjectClass*& ObjectToSave, const FString& FilePath)
	{
		FFileReport Report;

		TArray<uint8> Data;
		FObjectWriter Archive(ObjectToSave, Data);

		if (FFileHelper::SaveArrayToFile(Data, *FilePath))
		{
			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::SAVED
			};
		}

		return Report;
	}


private:	
	TMap<FName, UPackage*> PresetPackages;
	FString CurrentSettingsDefaultFilePath = TEXT("/DungeonGenerator/Settings/Settings.bin");
	

public:
	FORCEINLINE static FDungeonUtils* Get()
	{
		return Instance;
	}

private:	
	static FDungeonUtils* Instance;
	
};


/**
* Second version of DungeonUtils
*/
class FDungeonUtils2
{
public:
	FDungeonUtils2();

	URoomPresetPtr SelectedPreset = nullptr;

	FFileReport CreateGenerationSettings();
	FFileReport LoadGenerationSettings();
	FFileReport SaveGenerationSettings();

	FORCEINLINE UGenerationSettings* GetGenerationSettings() const
	{
		return CurrentSettings;
	}

	FORCEINLINE UDungeonData* GetDungeonData() const
	{
		return CurrentSettings->DungeonDataRef;
	}

	FORCEINLINE const FString GetDungeonDataPath() const
	{
		return CurrentSettings->DungeonDataFilePath;
	}

	FORCEINLINE const int32 GetDungeonRoomsCount() const
	{
		return CurrentSettings->InitialRoomsCount;
	}

	FORCEINLINE void SetDungeonDataPath(const FString& InPath) const
	{
		const FString DataFolderPath = InPath;
		CurrentSettings->DungeonDataFilePath = DataFolderPath + TEXT("DungeonData.bin");
	}

	FORCEINLINE void SetDungeonRoomsCount(const int32 InCount) const
	{		
		CurrentSettings->InitialRoomsCount = FMath::Max(0, InCount);
	}

	FORCEINLINE TMap<int32, URoomPresetPtr> GetPresets() const
	{
		return CurrentSettings->RoomPresetsRefMap;
	}

	FORCEINLINE TArray<FString>& GetPresetPaths() const
	{
		return CurrentSettings->RoomPresetsPaths;
	}

	FORCEINLINE URoomPresetPtr GetPresetByID(const int32 ID) const
	{
		return CurrentSettings->RoomPresetsRefMap[ID];
	}

	TArray<URoomPresetPtr> GetPresetsArray() const;

	FORCEINLINE URoomPresetPtr GetPresetByIndex(const int32 Index) const
	{
		TArray<URoomPresetPtr> Array = GetPresetsArray();
		return Array[Index];
	}

	FORCEINLINE URoomPresetPtr GetSelectedPreset() const
	{
		return SelectedPreset;
	}

	FORCEINLINE void SetSelectedPreset(UObject* Selection)
	{
		URoomPreset* CastObject = Cast<URoomPreset>(Selection);
		SelectedPreset = CastObject;
	}

	FORCEINLINE const FString GetPresetPathByID(const int32 ID) const
	{
		return CurrentSettings->RoomPresetsRefMap[ID]->GetPathName();
	}

	FORCEINLINE int32 GetPresetFilesCount() const
	{
		//return CurrentSettings->RoomPresetsRefMap.Num();
		return CurrentSettings->RoomPresetsPaths.Num();
	}

	FORCEINLINE bool CheckRoomPresetByID(const int32 ID) const
	{
		return CurrentSettings->RoomPresetsRefMap.Contains(ID);
	}

	void AddPresetReference(URoomPresetPtr NewPreset);
	void AddPresetPath(const FString& NewPath);
	void DeletePresetReference(URoomPresetPtr NewPreset);
	void DeletePresetPath(const FString& Path);

	void GetPresetsOnLoad();

	UDungeonData* SaveDungeonData(const struct FDungeonInfo& Info);


	template<class NewObjectClass>
	static FFileReport CreateSerializedObject(NewObjectClass*& OutObjectRef, const FName& ObjectName, const FString& FilePath)
	{
		FFileReport Report;

		OutObjectRef = NewObject<NewObjectClass>
			(
				GetTransientPackage(), ObjectName,
				EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
			);

		Report = LoadSerializedObject(OutObjectRef, FilePath);
		if (Report.CommandStatus == ECommandStatusType::ERROR)
		{
			Report = SaveSerializedObject(OutObjectRef, FilePath);
		}

		return Report;
	}

	template<class NewObjectClass>
	static FFileReport LoadSerializedObject(NewObjectClass*& ObjectToLoad, const FString& FilePath)
	{
		FFileReport Report;

		TArray<uint8> Data;

		if (FFileHelper::LoadFileToArray(Data, *FilePath))
		{
			FObjectReader Archive(ObjectToLoad, Data);

			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::LOADED
			};
		}

		return Report;
	}

	template<class NewObjectClass>
	static FFileReport SaveSerializedObject(NewObjectClass*& ObjectToSave, const FString& FilePath)
	{
		FFileReport Report;

		TArray<uint8> Data;
		FObjectWriter Archive(ObjectToSave, Data);

		if (FFileHelper::SaveArrayToFile(Data, *FilePath))
		{
			Report = FFileReport
			{
				ECommandStatusType::VALID,
				EFileResultStatus::SAVED
			};
		}

		return Report;
	}


private:
	FString CurrentSettingsDefaultFilePath = TEXT("/DungeonGenerator/Settings/Settings.bin");
	UGenerationSettings* CurrentSettings = nullptr;


public:
	FORCEINLINE static FDungeonUtils2* Get()
	{
		return Instance;
	}

private:
	static FDungeonUtils2* Instance;
};