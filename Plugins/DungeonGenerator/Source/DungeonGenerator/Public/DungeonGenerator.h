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

#define LOGCOLOR Display



/**
* FRoomGenerator - This class makes a new ADungeonRoom Instance,
* with 
* 
*/
class FRoomGenerator
{
public:
	FRoomGenerator();
	
	class ADungeonRoom* Generate(const FRoomInfo& Info);
	void InsertData(ADungeonRoom* Room, const FRoomInfo& Info);	
	void Locate(ADungeonRoom* Room);
	void Show(ADungeonRoom* Room);	
};


/**
* FDungeonGenerator CLass
* 
*/
class FDungeonGenerator : public IDungeonBuilder
{
public:
	FDungeonGenerator();
	//virtual ~FDungeonGenerator() override;

	/** IDungeonBuilder implementation */
	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;
	
	FORCEINLINE const FDungeonInfo& GetDungeonInfo() const
	{
		return OutDungeonInfo;
	}

private:
	FDungeonInfo OutDungeonInfo;
};

class FRuntimeDungeonGenerator : public IDungeonBuilder
{
public:
	FRuntimeDungeonGenerator();
	//virtual ~FDungeonGenerator() override;

	/** IDungeonBuilder implementation */
	virtual void BuildDungeon(const int32 RoomsCount) override;
	virtual void BuildRooms() override;
	virtual void BuildDoors() override;
	
	UDungeonData* InDungeonData = nullptr;

private:
	TArray<ADungeonRoom*> GetRooms();
	TArray<ADoor*> GetDoors();
};


/**
* FDungeonGeneratorModule Module Class
*/
class FDungeonGeneratorModule : public IModuleInterface, public FSelfRegisteringExec
{
public:	

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** FSelfRegisteringExec implementation */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	void RegisterRoomPresetAssetPath(FMenuBuilder& MenuBuilder, const TArray<FAssetData> Data);
	void DGCommandRegisterPresetsPath(const TArray<FAssetData>& Data);
	TSharedRef<FExtender> ExtendContentBrowser(const TArray<FAssetData>& Data);
	TSharedRef<class SDockTab> SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs);


private:	
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator");
	
	//TSharedPtr<FDungeonGenerator> DungeonGenerator;
	//TSharedPtr<FRoomGenerator> RoomGenerator;
	TSharedPtr<FDungeonUtilities> DungeonUtils;
	
	FDungeonInfo CurrentDungeonInfo{};
	UDungeonData* CurrentDungeonData = nullptr;
	TArray<FRoomInfo> RoomsInfo{};
	TArray<FDoorInfo> DoorsInfo{};

	TArray<ADungeonRoom*> RoomsRef{};
	TArray<class ADoor*> DoorsRef{};
	

public:
	void DGCommandGenerate();
    void DGCommandSave();
	void DGCommandReset();
	void DGCommandPreview();
	void DGCommandRoomPreview();
	void DGCommandRoomSave();
	void DGCommandRoomReset();
	
	void RGCommandMakeRoom(const FText& InText, ETextCommit::Type InCommitType);

};
