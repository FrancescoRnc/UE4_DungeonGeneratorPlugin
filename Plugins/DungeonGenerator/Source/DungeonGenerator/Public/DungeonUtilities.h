// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenerationSettings.h"
#include "RoomData.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDunGenUtilities, All, All);


using URoomPresetPtr = class URoomPreset*;

/**
* Enumeration that tells the Status of a Command.
*/
enum class DUNGEONGENERATOR_API ECommandStatusType : uint8
{
	ERROR		= 0,
	VALID		= 1,
	WARNING		= 2,
	MAX			= 3
};

/**
* Enumeration that tells which type of Command has been used.
*/
enum class DUNGEONGENERATOR_API EFileCommandType : uint8
{
	NONE		= 0,
	CREATED		= 1,
	LOADED		= 2,
	MODIFIED	= 3,
	SAVED		= 4,
	DELETED		= 5,
	MAX			= 6
};

/**
* This Struct is used to know which type of Command has been used, and if a Command has been succesfully resolved.
* Use is recommended for File Managing operations.
*/
struct DUNGEONGENERATOR_API FFileReport
{
	ECommandStatusType CommandStatus	= ECommandStatusType::ERROR;
	EFileCommandType ResultStatus		= EFileCommandType::NONE;
};


/**
 * IDungeonBuilder Interface.
 */
class DUNGEONGENERATOR_API IDungeonBuilder
{
public:
	virtual ~IDungeonBuilder();

	/**
	* This step builds the fundamentals of the Target Dungeon.
	* @param RoomsCount: The amount of Rooms you want for your Dungeon;
	*/
	virtual void BuildDungeon(const int32 RoomsCount) = 0;

	/**
	* This step builds all the Rooms to form your Dungeon.
	*/
	virtual void BuildRooms() = 0;

	/**
	* This step builds all the Doors that link your Rooms;
	*/
	virtual void BuildDoors() = 0;
	
};


/**
* Singleton Class that exposes usefull functions for your environment
*/
class DUNGEONGENERATOR_API FDungeonUtilities
{
public:
	FDungeonUtilities();

	URoomPresetPtr SelectedPreset = nullptr;

	// Basic Functions used for managing the Generation Settings Object.
	FFileReport CreateGenerationSettings();
	FFileReport LoadGenerationSettings();
	FFileReport SaveGenerationSettings();


// Here's a series of simple Functions as Getters or Setters

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
		CurrentSettings->DungeonDataFilePath = InPath + TEXT("DungeonData.bin");
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
		if (CurrentSettings->RoomPresetsRefMap.Contains(ID))
		{
			return CurrentSettings->RoomPresetsRefMap[ID];
		}
		return nullptr;
	}

	TArray<URoomPresetPtr> GetPresetsArray() const;

	FORCEINLINE URoomPresetPtr GetPresetByIndex(const int32 Index) const
	{
		TArray<URoomPresetPtr> Array = GetPresetsArray();
		if (Index >= 0 && Index < Array.Num())
		{
			return Array[Index];
		}
		return nullptr;
	}

	FORCEINLINE URoomPresetPtr GetSelectedPreset() const
	{
		return SelectedPreset;
	}

	FORCEINLINE void SetSelectedPreset(UObject* Selection)
	{		
		if (URoomPreset* CastObject = Cast<URoomPreset>(Selection))
		{
			SelectedPreset = CastObject;
		}		
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

	// Functions related to URoomPreset Files references or paths management.
	void AddPresetReference(URoomPresetPtr NewPreset);
	void AddPresetPath(const FString& NewPath);
	void DeletePresetReference(const URoomPresetPtr NewPreset);
	void DeletePresetPath(const FString& Path);

	// This function gets all URoomPreset Files to be loaded after loading all Assets.
	void GetPresetsOnLoad();

	UDungeonData* SaveDungeonData(const struct FDungeonInfo& Info);


// Here are some Static Template Functions used to manage generic UObjects.

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
				EFileCommandType::LOADED
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
				EFileCommandType::SAVED
			};
		}

		return Report;
	}

	template<class ChosenActor>
	static ChosenActor* GetSelectedActor(UWorld* World, USelection* Selection)
	{
		ChosenActor* SelectedActor = nullptr;

		if (UObject* SelectedObject = Selection->GetTop(ChosenActor::StaticClass()))
		{
			SelectedActor = Cast<ChosenActor>(SelectedObject);
		}
		else
		{
			TArray<ChosenActor*> Actors;
			for (TActorIterator<ChosenActor> It(World); It; ++It)
			{
				ChosenActor* Actor = *It;
				Actors.Add(Actor);
			}
			if (Actors.Num() > 0)
			{
				SelectedActor = Actors.Pop(true);
				for (ChosenActor* Actor : Actors)
				{
					World->DestroyActor(Actor);
				}
				Actors.Empty();
			}
			else
			{
				SelectedActor = World->SpawnActor<ChosenActor>
					(ChosenActor::StaticClass(), FTransform::Identity);
			}
		}

		return SelectedActor;
	}


private:
	FString CurrentSettingsDefaultFilePath = TEXT("/DungeonGenerator/Settings/Settings.bin");
	UGenerationSettings* CurrentSettings = nullptr;


public:
	FORCEINLINE static FDungeonUtilities* Get()
	{
		return Instance;
	}

private:
	static FDungeonUtilities* Instance;
};