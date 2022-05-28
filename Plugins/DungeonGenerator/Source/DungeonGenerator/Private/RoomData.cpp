// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomData.h"
#include "DungeonUtilities.h"


URoomPreset::URoomPreset()
{
	
}

void URoomPreset::DoThings()
{
	UE_LOG(LogTemp, Warning, TEXT("I am Doing Things"))
}


// Room Preset Factory
URoomPresetFactory::URoomPresetFactory()
{
	SupportedClass = URoomPreset::StaticClass();
	Formats.Add(TEXT("RoomPreset;RoomPreset file fortmat"));
	bCreateNew = true;
}

UObject* URoomPresetFactory::FactoryCreateNew
(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
)
{
	URoomPreset* NewPreset = NewObject<URoomPreset>(InParent, InName, Flags);
	NewPreset->PresetID = FMath::Rand();
	FDungeonUtilities::Get()->AddPresetReference(NewPreset);
	return NewPreset;
}

FRoomPresetAssetTypeAction::FRoomPresetAssetTypeAction()
{

}

FRoomPresetAssetTypeAction::FRoomPresetAssetTypeAction(EAssetTypeCategories::Type Type)
{
	//AssetTypeCategory = Type;
}
