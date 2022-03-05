// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomData.h"


using URoomPresetPtr = class URoomPreset*;
//using URoomPresetPtr = TSoftObjectPtr<class URoomPreset>;
using URoomPresetRef = TSharedRef<URoomPreset>;


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
 * 
 */
class FDungeonUtils
{
public:	
	FDungeonUtils();	

	FName PrefabricsPath = TEXT("/Game/Prefabrics");
	FName DungeonDataPath = TEXT("/Game/DungeonData");
	int32 DungeonRoomsCount = 0;

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
			UE_LOG(LogTemp, Error, TEXT("ERROR: Presets Count between Array and map containers is not equal!"));
		}
		return RoomPresets.Num();
	}
	

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
