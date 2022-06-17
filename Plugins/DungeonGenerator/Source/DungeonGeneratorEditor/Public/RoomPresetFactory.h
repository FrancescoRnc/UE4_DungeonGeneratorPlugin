// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"
#include "CoreMinimal.h"
#include "RoomData.h"
#include "Factories/Factory.h"
#include "RoomPresetFactory.generated.h"

/**
 * 
 */
 static EAssetTypeCategories::Type DungeonAssetTypeCategory;

 class DUNGEONGENERATOREDITOR_API FRoomPresetAssetTypeAction : public FAssetTypeActions_Base
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
 class DUNGEONGENERATOREDITOR_API URoomPresetFactory : public UFactory
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
