#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "Modules/ModuleManager.h"


class DUNGEONGENERATOREDITOR_API FDungeonGeneratorEditorModule : public IModuleInterface, public FSelfRegisteringExec
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** FSelfRegisteringExec implementation */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;


	private:
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator Editor Tab");


	public:
	void DGCommandGenerateDungeonData();
	void DGCommandPreview();

	void DGCommandRoomPreview();
	void DGCommandRoomSaveChanges();
	void DGCommandRoomResetChanges();
};