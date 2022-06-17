// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "Modules/ModuleManager.h"


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


private:	
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator");
	
	TSharedPtr<FDungeonUtilities> DungeonUtils;


public:

};
