// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"
#include "CoreMinimal.h"
#include "Door.h"
#include "UObject/NoExportTypes.h"
#include "RoomData.generated.h"


static EAssetTypeCategories::Type DungeonAssetTypeCategory;


/**
* Struct used to store data for a Room in a generated Dungeon
*/
struct FRoomInfo
{
	FName RoomName = NAME_None;
	int32 IndexInGrid = -1;
	FIntVector CoordinateInGrid{0,0,0};
	TArray<FDoorInfo> DoorsInfo{};
	int32 PresetID = -1;
	FName PresetName = NAME_None;
	FString PresetPath = TEXT("");
};


/**
 * UObject Asset to be used for custom Editing of Rooms, to be used for the Dungeon generation
 */
UCLASS()
class DUNGEONGENERATOR_API URoomPreset : public UObject
{
	GENERATED_BODY()

	public:

	URoomPreset();

	UPROPERTY(VisibleAnywhere)
	int32 PresetID = -1;
	
	UPROPERTY(EditAnywhere)
	FName RoomName;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* FloorMesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallsMesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* DoorsMesh = nullptr;
};


class DUNGEONGENERATOR_API FRoomPresetAssetTypeAction : public FAssetTypeActions_Base
{
	public:

	FRoomPresetAssetTypeAction();

	FRoomPresetAssetTypeAction(EAssetTypeCategories::Type Type);


	FORCEINLINE uint32 GetCategories() override
	{
		return AssetTypeCategory;
	}

	FORCEINLINE FText GetName() const override
	{
		return FText::FromString(TEXT("Room Preset"));
	}

	FORCEINLINE UClass* GetSupportedClass() const override
	{
		return URoomPreset::StaticClass();
	}

	FORCEINLINE FColor GetTypeColor() const override
	{
		return FColor::Red;
	}

	private:

	EAssetTypeCategories::Type AssetTypeCategory = EAssetTypeCategories::None;
};


UCLASS()
class DUNGEONGENERATOR_API URoomPresetFactory : public UFactory
{
	GENERATED_BODY()

	public:

	URoomPresetFactory();

	UObject* FactoryCreateNew
	(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
	);
};