// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerationSettings.h"

void UGenerationSettings::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << InitialPresetFilesCount;
	Ar << PresetFolderPath;
	Ar << DungeonDataFolderPath;

	UE_LOG(LogTemp, Warning, TEXT("Is Saving? [%d] | Is Loading? [%d]"), Ar.IsSaving(), Ar.IsLoading());
}