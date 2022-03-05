// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "Modules/ModuleManager.h"


#define MESSAGECOLOR Display

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

	TSharedRef<class SDockTab> SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs);


private:	
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator");
    const int32 DGSpacerSize = 10;
    const int32 HPadding = 10;
    const int32 VPadding = 20;
	
	//TSharedPtr<FDungeonGenerator> DungeonGenerator;
	//TSharedPtr<FRoomGenerator> RoomGenerator;
	TSharedPtr<FDungeonUtils> DungeonUtils;
	
	FDungeonInfo CurrentDungeonInfo{};
	UDungeonData* CurrentDungeonData = nullptr;
	TArray<FRoomInfo> RoomsInfo{};
	TArray<FDoorInfo> DoorsInfo{};

	TArray<ADungeonRoom*> RoomsRef{};
	TArray<class ADoor*> DoorsRef{};
	

public:
	FReply DGCommandGenerate();
    FReply DGCommandSave();
	FReply DGCommandReset();
	FReply DGCommandPreview();
	FReply DGCommandRoomPreview();
	
	void RGCommandMakeRoom(const FText& InText, ETextCommit::Type InCommitType);

};
