#pragma once

#include "CoreMinimal.h"
#include "DungeonData.h"
#include "DungeonUtilities.h"
#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDunGenStartup, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenShutdown, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenBuild, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenBuildError, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenExecQueryFiles, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenExecQueryInfo, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDunGenCommands, All, All);


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


class DUNGEONGENERATOREDITOR_API FDungeonGeneratorEditorModule : public IModuleInterface, public FSelfRegisteringExec
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;

	/** FSelfRegisteringExec implementation */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	// Functions used as Delegates to extend the Content Browser
	void RegisterRoomPresetAssetPath(FMenuBuilder& MenuBuilder, const TArray<FAssetData> Data);
	TSharedRef<FExtender> ExtendContentBrowser(const TArray<FAssetData>& Data);

	// Function used as Delegate to spawn the Plugin Tab
	TSharedRef<class SDockTab> SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs);


	private:
	const FName DungeonGeneratorTabName = TEXT("Dungeon Generator Editor Tab");

	// Local variables used to track the current generation actions.
	FDungeonInfo CurrentDungeonInfo{};
	UDungeonData* CurrentDungeonData = nullptr;

	FORCEINLINE void SetSelectedPreset(UObject* Selection)
	{
		if (URoomPreset* CastObject = Cast<URoomPreset>(Selection))
		{
			FDungeonUtilities::Get()->SelectedPreset = CastObject;
		}
	}


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