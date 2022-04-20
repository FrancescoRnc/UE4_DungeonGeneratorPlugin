// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomData.h"


using URoomPresetPtr = class URoomPreset*;
//using URoomPresetPtr = TSoftObjectPtr<class URoomPreset>;
using URoomPresetRef = TSharedRef<URoomPreset>;


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

	FName PrefabricsPath = TEXT("/Game/Prefabrics");
	FName DungeonDataPath = TEXT("/Game/DungeonData");
	int32 DungeonRoomsCount = 0;

	//UGenerationSettings* CurrentSettings = nullptr;

	TMap<int32, URoomPresetPtr> RoomPresetsMap{};
	TArray<URoomPresetPtr> RoomPresets{};
	TArray<FName> RoomPresetPaths{};
	URoomPresetPtr SelectedPreset = nullptr;

	class UDungeonData* CurrentDungeonFile = nullptr;
	FName CurrentDungeonPath = NAME_None;
	UPackage* DungeonPackage = nullptr;
	

	void GetPresetsOnLoad();
	void AddAssetReference(URoomPresetPtr NewPreset);
	void DeleteAssetReference(URoomPresetPtr NewPreset);

	
	const bool RescanAssetReferences() const;
	bool RGCommandMakeRoom(const FString AssetName);
	UDungeonData* SaveDungeonData(const struct FDungeonInfo& Info);
	UDungeonData* GetDungeonDataAsset() const;
	
	URoomPresetPtr GetRoomPresetByName(const FName Name) const;
	URoomPresetPtr GetRoomPresetByPath(const FName Path) const;
	
	FORCEINLINE URoomPresetPtr GetRoomPresetByID(const int32 ID) const
	{
		return RoomPresetsMap[ID];
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

	FFileReport SaveAsset(UObject* Asset);
	FFileReport DeleteAsset(UObject* Asset);


private:	
	TMap<FName, UPackage*> PresetPackages;
	

public:
	FORCEINLINE static FDungeonUtils* Get()
	{
		return Instance;
	}

private:	
	static FDungeonUtils* Instance;
	
};
