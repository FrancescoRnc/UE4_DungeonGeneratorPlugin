// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenerationSettings.generated.h"

/**
 * This Object File will be loaded at Plugin Startup and saved at Shutdown, 
 * and its parameters will be used by all Plugin Features.
 */
UCLASS()
class DUNGEONGENERATOR_API UGenerationSettings : public UObject
{
	GENERATED_BODY()

	public:

	UPROPERTY(VisibleAnywhere)
	FName DungeonDataFolderPath = NAME_None;

	UPROPERTY(VisibleAnywhere)
	FName PresetFolderPath = NAME_None;

	UPROPERTY(VisibleAnywhere)
	uint32 InitialPresetFilesCount = 0;

	/*UFUNCTION()
	void SetDungeonDataFolderPath(const FName Path);

	UFUNCTION()
	void SetPresetFolderPath(const FName Path);

	UFUNCTION()
	FName GetDungeonDataFolderPath() const;

	UFUNCTION()
	FName GetPresetFolderPath() const;*/

	void Serialize(FArchive& Ar) override;
};
