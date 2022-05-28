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
* 
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
 * 
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

	UFUNCTION(BlueprintCallable)
	void DoThings();

};


class DUNGEONGENERATOR_API FRoomPresetAssetTypeAction : public FAssetTypeActions_Base
{
	public:

	FRoomPresetAssetTypeAction();

	FRoomPresetAssetTypeAction(EAssetTypeCategories::Type Type);


	FORCEINLINE uint32 GetCategories() override
	{
		return EAssetTypeCategories::Blueprint;
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

//private:

	//static EAssetTypeCategories::Type AssetTypeCategory;
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

	FORCEINLINE FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Room Preset"));
	};

	FORCEINLINE uint32 GetMenuCategories() const override
	{
		return EAssetTypeCategories::Blueprint;
	};

	private:

	EAssetTypeCategories::Type AssetTypeCategory = EAssetTypeCategories::None;
};
