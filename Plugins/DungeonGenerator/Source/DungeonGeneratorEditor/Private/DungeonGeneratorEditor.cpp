#include "DungeonGeneratorEditor.h"
#include "DungeonGenerator.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "ContentBrowserModule.h"
#include "DungeonData.h"
#include "DungeonSchemeMaker.h"
#include "Editor.h"
#include "EditorDungeon.h"
#include "EditorRoom.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"
#include "RoomData.h"
#include "RoomPresetFactory.h"
#include "SAssetDropTarget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/LayerManager/SLayerManager.h"
#include "Widgets/Layout/SSpacer.h"


#define LOCTEXT_NAMESPACE "FDungeonGeneratorEditorModule"

DEFINE_LOG_CATEGORY(LogDunGenStartup);
DEFINE_LOG_CATEGORY(LogDunGenShutdown);
DEFINE_LOG_CATEGORY(LogDunGenBuild);
DEFINE_LOG_CATEGORY(LogDunGenBuildError);
DEFINE_LOG_CATEGORY(LogDunGenExecQueryFiles);
DEFINE_LOG_CATEGORY(LogDunGenExecQueryInfo);
DEFINE_LOG_CATEGORY(LogDunGenCommands);


void FDungeonGeneratorEditorModule::StartupModule()
{
	//DungeonUtils = MakeShared<FDungeonUtilities>();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	DungeonAssetTypeCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Dungeon Elements")), LOCTEXT("DungeonElementsText", "Dungeon Elements"));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FRoomPresetAssetTypeAction(DungeonAssetTypeCategory)));

	//DungeonUtils->CreateGenerationSettings();

	/*FAssetRegistryModule::GetRegistry().OnFilesLoaded()
		.AddRaw(DungeonUtils.Get(), &FDungeonUtilities::GetPresetsOnLoad);
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetDeleted()
		.AddLambda([this](UObject* Object)
	{
		if (const URoomPresetPtr Preset = Cast<URoomPreset>(Object))
		{
			DungeonUtils->DeletePresetReference(Preset);
			UE_LOG(LogDunGenStartup, Display, TEXT("In Memory Asset Deleted: %s"), *Object->GetName());
		}
	});*/

	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw
		(
			this, 
			&FDungeonGeneratorEditorModule::ExtendContentBrowser
		)
	);

	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	FTabSpawnerEntry SpawnerEntry = TabManager->RegisterNomadTabSpawner(DungeonGeneratorTabName, FOnSpawnTab::CreateRaw
		(
			this, 
			&FDungeonGeneratorEditorModule::SpawnNomadTab
		)
	);

	// Uncomment only if tab is not on Editor, then re-comment as well as it is.
	//TabManager->TryInvokeTab(DungeonGeneratorTabName);

	UE_LOG(LogDunGenStartup, Display, TEXT("Dungeon Generator Startup"));
}

void FDungeonGeneratorEditorModule::ShutdownModule()
{
	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	TabManager->UnregisterNomadTabSpawner(DungeonGeneratorTabName);

	UE_LOG(LogDunGenShutdown, Display, TEXT("Dungeon Generator Shutdown"));
}

bool FDungeonGeneratorEditorModule::SupportsDynamicReloading()
{
	return true;
}

bool FDungeonGeneratorEditorModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	const FString CmdAsString = Cmd;

	if (CmdAsString == "querypresets")
	{
		UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Number of Asset Files: %d"),
			   FDungeonUtilities::Get()->GetPresetFilesCount());
		for (const TPair<int32, TSoftObjectPtr<URoomPreset>> Data : FDungeonUtilities::Get()->GetPresets())
		{
			const FString AssetName = Data.Value->GetName();
			const FString AssetPath = Data.Value->GetPathName();
			UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Registered Asset File: %s | At: %s"), *AssetName, *AssetPath);
		}
		for (const FString Path : FDungeonUtilities::Get()->GetPresetPaths())
		{
			UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Registered Asset File Path: %s"), *Path);
		}

		return true;
	}
	else if (CmdAsString == "querydungeon")
	{
		UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-"));
		UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("Querying Dungeon Information..."));

		UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("Dungeon Grid Size:							%s"),
			   *CurrentDungeonInfo.GridSize.ToString());

		const int32 RoomsCount = CurrentDungeonInfo.RoomsInfo.Num();
		UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("Dungeon Rooms Number:						%d"),
			   RoomsCount);

		for (int32 Index = 0; Index < RoomsCount; Index++)
		{
			const FRoomInfo CurrentRoom = CurrentDungeonInfo.RoomsInfo[Index];

			//const URoomPresetPtr GetPreset = DungeonUtils->GetRoomPresetByPath(CurrentRoom.PresetPath);
			const URoomPresetPtr GetPreset = FDungeonUtilities::Get()->GetPresetByID(CurrentRoom.PresetID);
			UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- Room of index %d Preset Used:			%s"),
				   Index, *GetPreset->GetName());

			UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- Room of index %d Name:					%s"),
				   Index, *CurrentRoom.RoomName.ToString());

			UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- Room of index %d Coords in Grid:			%s"),
				   Index, *CurrentRoom.CoordinateInGrid.ToString());

			UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- Room of index %d Index in Grid:			%d"),
				   Index, CurrentRoom.IndexInGrid);

			const int32 DoorsCount = CurrentRoom.DoorsInfo.Num();
			UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- Room of index %d Number of Doors:		%d"),
				   Index, DoorsCount);

			for (int32 JIndex = 0; JIndex < DoorsCount; JIndex++)
			{
				const FDoorInfo& CurrentDoor = CurrentRoom.DoorsInfo[JIndex];

				const FName DirectionName = WorldDirectionNames[CurrentDoor.Direction];
				const FName OppositeDirectionName = WorldDirectionNames[CurrentDoor.OppositeDoorDirection];

				UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("-|- -|- Door at %s Destination Room:		%s"),
					   *DirectionName.ToString(), *CurrentDoor.NextRoomName.ToString());
			}
		}
		UE_LOG(LogDunGenExecQueryInfo, Display, TEXT("/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-"));

		return true;
	}

	return false;
}

void FDungeonGeneratorEditorModule::DGCommandGenerateDungeonData()
{
	CurrentDungeonInfo.Reset();

	const int32 RoomsCount = FDungeonUtilities::Get()->GetDungeonRoomsCount();
	if (RoomsCount <= 0)
	{
		UE_LOG(LogDunGenCommands, Error, TEXT("ERROR: Rooms Count value is not valid! Please try with another value."));
		return;
	}

	TSharedPtr<FDefaultGeneratorMethod> Method = MakeShared<FDefaultGeneratorMethod>();
	FDungeonDataGenerator DungeonGenerator(Method.Get());
	DungeonGenerator.BuildDungeon(RoomsCount);
	DungeonGenerator.BuildRooms();
	DungeonGenerator.BuildDoors();
	CurrentDungeonInfo = DungeonGenerator.GetDungeonInfo();

	CurrentDungeonData = FDungeonUtilities::Get()->SaveDungeonData(CurrentDungeonInfo);

	UE_LOG(LogDunGenCommands, Display, TEXT("A NEW DUNGEON HAS BEEN GENERATED!"));
}

void FDungeonGeneratorEditorModule::DGCommandPreview()
{
	// Preview Operations

	UWorld* World = GEditor->GetEditorWorldContext().World();
	USelection* Selection = GEditor->GetSelectedActors();

	AEditorDungeon* EditorDungeon = GetSelectedActor<AEditorDungeon>(World, Selection);

	EditorDungeon->Build();

	UE_LOG(LogDunGenCommands, Display, TEXT("HERE'S A PREVIEW OF THE MAP!"));
}

void FDungeonGeneratorEditorModule::DGCommandRoomPreview()
{
	USelection* Selection = GEditor->GetSelectedActors();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = GetSelectedActor<AEditorRoom>(World, Selection);

	const URoomPresetPtr SelectedPreset = FDungeonUtilities::Get()->GetSelectedPreset();
	if (SelectedPreset)
	{
		FRoomInfo Info;
		Info.PresetID = SelectedPreset->PresetID;
		Info.PresetPath = *SelectedPreset->GetPathName();
		Info.PresetName = *SelectedPreset->GetName();
		Info.RoomName = SelectedPreset->RoomName;

		SelectedRoom->Preview(Info);

		UE_LOG(LogDunGenCommands, Display, TEXT("HERE'S A PREVIEW OF THE SELECTED ROOM!"));
	}
}

void FDungeonGeneratorEditorModule::DGCommandRoomSaveChanges()
{
	USelection* Selection = GEditor->GetSelectedActors();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = GetSelectedActor<AEditorRoom>(World, Selection);

	URoomPresetPtr SelectedPreset = FDungeonUtilities::Get()->GetSelectedPreset();
	if (SelectedPreset)
	{
		SelectedRoom->OverwritePresetData(SelectedPreset);
		UE_LOG(LogDunGenCommands, Display, TEXT("SELECTED ROOM SAVED!"));
	}
}

void FDungeonGeneratorEditorModule::DGCommandRoomResetChanges()
{
	USelection* Selection = GEditor->GetSelectedActors();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = GetSelectedActor<AEditorRoom>(World, Selection);

	URoomPresetPtr SelectedPreset = FDungeonUtilities::Get()->GetSelectedPreset();
	if (SelectedRoom)
	{
		SelectedRoom->ResetPreviewStatus(SelectedPreset);
		UE_LOG(LogDunGenCommands, Display, TEXT("SELECTED ROOM RESET!"));
	}
	// Reset Implementation

}

void FDungeonGeneratorEditorModule::RegisterRoomPresetAssetPath(FMenuBuilder& MenuBuilder, const TArray<FAssetData> Data)
{
	MenuBuilder.BeginSection(FName("Dungeon Assets Actions"), FText::FromString(""));
	MenuBuilder.AddMenuEntry(FText::FromString("Register Room Presets Path"), FText::FromString(""),
							 FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([this, Data]()
	{
		for (const FAssetData Asset : Data)
		{
			const URoomPresetPtr Preset = Cast<URoomPreset>(Asset.GetAsset());
			if (Preset)
			{
				FDungeonUtilities::Get()->AddPresetReference(Preset);
				UE_LOG(LogTemp, Error, TEXT("RegisterRoomPresetAssetPath Command!!!"));
			}
		}
	})));
	MenuBuilder.AddMenuEntry(FText::FromString("Unegister Room Presets Path"), FText::FromString(""),
							 FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([this, Data]()
	{
		for (const FAssetData Asset : Data)
		{
			const URoomPresetPtr Preset = Cast<URoomPreset>(Asset.GetAsset());
			if (Preset)
			{
				FDungeonUtilities::Get()->DeletePresetReference(Preset);
				UE_LOG(LogTemp, Error, TEXT("UnegisterRoomPresetAssetPath Command!!!"));
			}
		}
	})));
	MenuBuilder.EndSection();
}

TSharedRef<FExtender> FDungeonGeneratorEditorModule::ExtendContentBrowser(const TArray<FAssetData>& Data)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	Extender->AddMenuExtension(FName("CommonAssetActions"), EExtensionHook::After, nullptr,
							   FMenuExtensionDelegate::CreateRaw(this, &FDungeonGeneratorEditorModule::RegisterRoomPresetAssetPath, Data));
	return Extender;
}

TSharedRef<SDockTab> FDungeonGeneratorEditorModule::SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs)
{
	FToolBarBuilder ToolbarBuilder(MakeShared<FUICommandList>(), FMultiBoxCustomization::None);

	TSharedRef<SWidget> DungeonCommandsTitle =
		SNew(SBox).VAlign(VAlign_Fill).HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("DungeonCommandsText", "Dungeon \nCommands:"))
		];
	TSharedRef<SWidget> RoomCommandsTitle =
		SNew(SBox).VAlign(VAlign_Fill).HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("RoomCommandsText", "Room \nCommands:"))
		];

	ToolbarBuilder.AddWidget(DungeonCommandsTitle, NAME_None);
	const FText DGenerateText = LOCTEXT("ToolbarGenerate", "Generate");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorEditorModule::DGCommandGenerateDungeonData)), NAME_None, DGenerateText);

	// Dungeon Generation Features
	const FText DShowPreviewText = LOCTEXT("ToolbarShowPreviewText", "Preview");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorEditorModule::DGCommandPreview)), NAME_None, DShowPreviewText);

	// Future Implementation
	//const FText DSaveChangesText = LOCTEXT("ToolbarSaveText", "Save");
	//ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	//(this, &FDungeonGeneratorModule::DGCommandSave)), NAME_None, DSaveChangesText);
	//const FText DResetText = LOCTEXT("ToolbarResetText", "Reset");
	//ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	//(this, &FDungeonGeneratorModule::DGCommandReset)), NAME_None, DResetText);

	ToolbarBuilder.AddSeparator();

	// Room Generation Features
	ToolbarBuilder.AddWidget(RoomCommandsTitle, NAME_None);
	const FText RShowPreviewText = LOCTEXT("ToolbarShowPreviewText", "Preview");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorEditorModule::DGCommandRoomPreview)), NAME_None, RShowPreviewText);

	const FText RSaveChangesText = LOCTEXT("ToolbarSaveText", "Save");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorEditorModule::DGCommandRoomSaveChanges)), NAME_None, RSaveChangesText);

	const FText RResetText = LOCTEXT("ToolbarResetText", "Reset");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorEditorModule::DGCommandRoomResetChanges)), NAME_None, RResetText);

	ToolbarBuilder.AddSeparator();

	const TSharedRef<SWidget> Toolbar = ToolbarBuilder.MakeWidget();


	const int32 DGSpacerSize = 10;
	const int32 HPadding = 10;
	const int32 VPadding = 10;


	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)

			// Toolbar
		+ SVerticalBox::Slot().AutoHeight()
		.Padding(HPadding, VPadding, 0, 0)
		[
			Toolbar
		]

	// Commands
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(HPadding, VPadding, HPadding, 0)
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(HPadding, VPadding, 0, 0)
		[
			SNew(STextBlock).Text(LOCTEXT("ConstructYourOwnDungeonHereText", "Construct your own Dungeon here:"))
		]
	+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SSpacer).Size(VPadding)
		]
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(HPadding, 0, 0, 0)
		[
			SNew(SVerticalBox)

			// Set Dungeon Data Folder Path
		+ SVerticalBox::Slot().AutoHeight()
		.Padding(0, VPadding, 0, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
		.Padding(0, 0, HPadding, 0)
		[
			SNew(STextBlock).Text(LOCTEXT("SetDungeonFolderPathText", "Set Dungeon Folder Path:"))
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.Padding(0, 0, HPadding, 0)
		[
			SNew(SEditableTextBox).HintText(LOCTEXT("PathText", "Path"))
			.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
	{
		if (InCommitType == ETextCommit::OnEnter)
		{
			FDungeonUtilities::Get()->SetDungeonDataPath(*InText.ToString());
			FDungeonUtilities::Get()->SaveGenerationSettings();
		}
	})
		]
		]

	// Set Rooms count for your next Dungeon
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(0, VPadding, 0, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
		.Padding(0, 0, HPadding, 0)
		[
			SNew(STextBlock).Text(LOCTEXT("SetRoomsCount", "Set Dungeon Rooms Count:"))
		]
	+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Left)
		.Padding(0, 0, HPadding, 0)
		[
			SNew(SEditableTextBox).HintText(LOCTEXT("CountText", "Count"))
			.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
	{
		if (InCommitType == ETextCommit::OnEnter)
		{
			FDungeonUtilities::Get()->SetDungeonRoomsCount(FCString::Atoi(*InText.ToString()));
			FDungeonUtilities::Get()->SaveGenerationSettings();
		}
	})
		]
		]

	// Drag and Drop Room Preset Asset
	+ SVerticalBox::Slot()
		.Padding(0, VPadding, HPadding, 0)
		[
			SNew(SBox).HeightOverride(50).HAlign(HAlign_Fill)
			[
				SNew(SSplitter).Orientation(EOrientation::Orient_Horizontal)
				+ SSplitter::Slot()
		[
			SNew(SBox).HAlign(HAlign_Fill).VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("DragAndDropText", "Drop your Room Preset Asset here ->"))
			]
		]
	+ SSplitter::Slot()
		[
			SNew(SBorder)
			[
				SNew(SBox).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SAssetDropTarget)//.OnAssetDropped_Lambda([this](UObject* Selection)
					//{
					//	URoomPreset* CastObject = Cast<URoomPreset>(Selection);
					//	if (CastObject)
					//	{
					//		FDungeonUtilities::Get()->SelectedPreset = Cast<URoomPreset>(Selection);
					//	}
					//})
					.OnAssetDropped_Raw(this, &FDungeonGeneratorEditorModule::SetSelectedPreset)
		[
			SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text_Lambda([this]() -> FText
	{
		return FDungeonUtilities::Get()->SelectedPreset ?
			FText::FromString(FDungeonUtilities::Get()->SelectedPreset->GetName())
			: LOCTEXT("None", "None");
	})
			]
		]
				]
			]
		]
			]
		]
		]
	+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SSpacer).Size(VPadding)
		]
			]
		]
	+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SSpacer).Size(VPadding)
		]

	// Queries
	+ SVerticalBox::Slot().VAlign(VAlign_Top)
		.Padding(HPadding, 0, HPadding, 0)
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
		.Padding(HPadding, VPadding, 0, 0)
		[
			SNew(STextBlock).Text(LOCTEXT("QueriesText", "Here are some Queries:"))
		]

	// Dungeon
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(HPadding, VPadding, HPadding, 0)
		[
			SNew(SVerticalBox)

			// Query Dungeon File Path
		+ SVerticalBox::Slot().AutoHeight()
		.Padding(0, VPadding, 0, 0)
		[
			SNew(STextBlock).Text_Lambda([this]() -> FText
	{
		return FText::Format(
			LOCTEXT("QueryDungeonFile Path", "Dungeon File saved at: {0}"),
			FText::FromString(*FDungeonUtilities::Get()->GetDungeonDataPath()));
	})
		]

	// Query Dungeon Rooms Count
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(0, VPadding, 0, 0)
		[
			SNew(STextBlock).Text_Lambda([this]() -> FText
	{
		return FText::Format(LOCTEXT("QueryDungeonRoomsText", "Dungeon Rooms Count: {0}"),
							 FText::FromString(FString::FromInt(FDungeonUtilities::Get()->GetDungeonRoomsCount())));
	})
		]

	// Query Preset Files Count
	+ SVerticalBox::Slot().AutoHeight()
		.Padding(0, VPadding, 0, 0)
		[
			SNew(STextBlock).Text_Lambda([this]() -> FText
	{
		return FText::Format(LOCTEXT("QueryNumOfAssetFiles", "Preset Asset Files: {0}"),
							 FDungeonUtilities::Get()->GetPresetFilesCount());
	})
		]
	+ SVerticalBox::Slot()
		[
			SNew(SSpacer).Size(VPadding)
		]

		]
			]
		]
		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonGeneratorEditorModule, DungeonGeneratorEditorModule);