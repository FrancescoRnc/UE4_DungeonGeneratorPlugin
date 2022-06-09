// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDunGenStartup,			All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenShutdown,			All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenBuild,				All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenBuildError,		All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenExecQueryFiles,	All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenExecQueryInfo,		All, All);


/**
* Standard class that generates a Dungeon given some Information like a UDungeonData Object and an FRoomInfo Object.
*/
class DUNGEONGENERATOR_API FDungeonDataGenerator : public IDungeonBuilder
{
public:
	FDungeonDataGenerator();
	FDungeonDataGenerator(IGeneratorMethod* InMethod) :
		Method{InMethod} { }
	virtual ~FDungeonDataGenerator() override;

	/** IDungeonBuilder implementation */
	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;
	
	FORCEINLINE const FDungeonInfo& GetDungeonInfo() const
	{
		return OutDungeonInfo;
	}

private:
	FDungeonInfo OutDungeonInfo{};

	IGeneratorMethod* Method = nullptr;
};




/**
* Main Module of this DungeonGenerator Plugin.
*/
class DUNGEONGENERATOR_API FDungeonGeneratorModule : public IModuleInterface, public FSelfRegisteringExec
{
public:	

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** FSelfRegisteringExec implementation */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	// Functions used as Delegates to extend the Content Browser
	void RegisterRoomPresetAssetPath(FMenuBuilder& MenuBuilder, const TArray<FAssetData> Data);
	TSharedRef<FExtender> ExtendContentBrowser(const TArray<FAssetData>& Data);

	// Function used as Delegate to spawn the Plugin Tab
	TSharedRef<class SDockTab> SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs);


	FORCEINLINE TWeakPtr<FDungeonUtilities> GetDungeonUtilities() const
	{
		return DungeonUtils;
	};


private:	
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator");
	
	TSharedPtr<FDungeonUtilities> DungeonUtils;
	
	// Local variables used to track the current generation actions.
	FDungeonInfo CurrentDungeonInfo{};
	UDungeonData* CurrentDungeonData = nullptr;
	

public:
	// Here are some Functions used by the Plugin's Toolbar to manage your Dungeon and Room customization.
	// * DG stands for Dungeon Generator, in order to mark specific Features as Commands.
	// ** Commands related to "Preview" give an on World representation (on Editor World in this case) of our Dungeon or Room

	void DGCommandGenerateDungeonData();
	void DGCommandPreview();
	
	void DGCommandRoomPreview();
	void DGCommandRoomSaveChanges();
	void DGCommandRoomResetChanges();
	
	/* Future Reimplementation
    void DGCommandSave();
	void DGCommandReset();
	*/
};
