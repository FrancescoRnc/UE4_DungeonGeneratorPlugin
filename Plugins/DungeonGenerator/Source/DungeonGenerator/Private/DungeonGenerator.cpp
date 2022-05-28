// Copyright Epic Games, Inc. All Rights Reserved.

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
#include "SAssetDropTarget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/LayerManager/SLayerManager.h"
#include "Widgets/Layout/SSpacer.h"


#define LOCTEXT_NAMESPACE "FDungeonGeneratorModule"

DEFINE_LOG_CATEGORY(LogDunGenStartup);
DEFINE_LOG_CATEGORY(LogDunGenShutdown);
DEFINE_LOG_CATEGORY(LogDunGenBuild);
DEFINE_LOG_CATEGORY(LogDunGenBuildError);
DEFINE_LOG_CATEGORY(LogDunGenExecQueryFiles);
DEFINE_LOG_CATEGORY(LogDunGenExecQueryInfo);


// FDungeonGenerator
FDungeonGenerator::FDungeonGenerator()
{
	
}

FDungeonGenerator::~FDungeonGenerator()
{
	
}

void FDungeonGenerator::BuildDungeon(const int32 RoomsCount)
{
	OutDungeonInfo = {};
	if (RoomsCount <= 0)
	{
		UE_LOG(LogDunGenBuild, Display, TEXT("Cannot Building Dungeon! Rooms Count is no valid!"));
		return;
	}

	UE_LOG(LogDunGenBuild, LOGCOLOR, TEXT("Building Dungeon..."));

	// Here you can set whichever of your custom GeneratorMethods for your custom Dungeon Generation
	TSharedPtr<FStandardGeneratorMethod> Method = MakeShared<FStandardGeneratorMethod>();
	FDungeonGridMaker DungeonGridMaker(RoomsCount, Method.Get());

	//FGrid Grid = DungeonGridMaker.GetGrid();
	FGrid Grid = DungeonGridMaker.Make();
	const TArray<int32> Scheme = Grid.GetScheme();
	const TArray<int32> SchemePath = Grid.GetSchemePath();
	
	OutDungeonInfo.State = EDungeonInfoState::VALID;
	OutDungeonInfo.Grid = Grid;
	OutDungeonInfo.GridSize = Grid.Size;	
	OutDungeonInfo.GridScheme = Scheme;
}

void FDungeonGenerator::BuildRooms()
{
	if (OutDungeonInfo.State == EDungeonInfoState::NOVALID)
	{
		return;
	}

	const int32 PresetsCount = FDungeonUtilities::Get()->GetPresetFilesCount();
	if (PresetsCount == 0)
	{
		UE_LOG(LogDunGenBuild, Error, TEXT("Impossible to Generate a new dungeon: No Presets loaded."));
		return;
	}

	UE_LOG(LogDunGenBuild, LOGCOLOR, TEXT("Building Rooms..."));

	const int32 Length = OutDungeonInfo.Grid.PathLength;
	OutDungeonInfo.RoomsInfo.Init({}, Length);	
	
	for (int32 Index = 0; Index < Length; Index++)
	{	
		FRoomInfo Room;
		if (PresetsCount > 0)
		{
			const TArray<URoomPreset*> PresetsArray = FDungeonUtilities::Get()->GetPresetsArray();
			const int32 RandomPresetsMax = PresetsCount - 1;
			const int32 RandomPresetIndex = Index == 0 ? 0 : FMath::RandRange(1, RandomPresetsMax);
			const URoomPresetPtr RandomPreset = PresetsArray[RandomPresetIndex];
			Room.PresetPath = *RandomPreset->GetPathName();
			Room.PresetID = RandomPreset->PresetID;
		}

		const FIntVector RoomCoords =
		{
			OutDungeonInfo.Grid.PathTrack[Index].X,
			OutDungeonInfo.Grid.PathTrack[Index].Y,
			0
		};
		
		Room.RoomName = *FString::Printf(TEXT("PathOrder%02d"), (Index + 1));
		Room.CoordinateInGrid = RoomCoords;
		Room.IndexInGrid = Room.CoordinateInGrid.X +
			(Room.CoordinateInGrid.Y * OutDungeonInfo.GridSize.X);		
		
		OutDungeonInfo.RoomsInfo[Index] = Room;
	}
	
	UE_LOG(LogDunGenBuild, LOGCOLOR, TEXT("Rooms Built!"));
}

void FDungeonGenerator::BuildDoors()
{
	if (OutDungeonInfo.State == EDungeonInfoState::NOVALID)
	{
		return;
	}

	UE_LOG(LogDunGenBuild, LOGCOLOR, TEXT("Building Doors..."));

	TArray<FRoomInfo>& OutRoomsInfo = OutDungeonInfo.RoomsInfo;

	for (int32 RoomIndex = 1; RoomIndex < OutRoomsInfo.Num(); RoomIndex++)
	{
		const int32 PrevRoomIndex = RoomIndex - 1;
		FRoomInfo& PrevRoom = OutRoomsInfo[PrevRoomIndex];
		FRoomInfo& CurrRoom = OutRoomsInfo[RoomIndex];
		FDoorInfo PrevDoor, CurrDoor;

		const FIntVector DeltaCoords =
			CurrRoom.CoordinateInGrid - PrevRoom.CoordinateInGrid;
		const int32 PatternIndex = Directions.IndexOfByKey(DeltaCoords);

		PrevDoor.SourceRoomName = PrevRoom.RoomName;
		PrevDoor.SourceRoomIndex = PrevRoomIndex;
		PrevDoor.NextRoomName = CurrRoom.RoomName;
		PrevDoor.NextRoomIndex = RoomIndex;
		const int32 CurrDirection = PatternIndex;
		PrevDoor.Direction = static_cast<EWorldDirection>(PatternIndex);

		CurrDoor.SourceRoomName = PrevRoom.RoomName;
		CurrDoor.SourceRoomIndex = RoomIndex;
		CurrDoor.NextRoomName = CurrRoom.RoomName;
		CurrDoor.NextRoomIndex = PrevRoomIndex;
		const int32 PrevDirection = IGeneratorMethod::GetOppositeIndex(PatternIndex);
		CurrDoor.Direction = static_cast<EWorldDirection>(PrevDirection);

		PrevRoom.DoorsInfo.Add(PrevDoor);
		CurrRoom.DoorsInfo.Add(CurrDoor);	
	}
	OutDungeonInfo.RoomsInfo = OutRoomsInfo;
	
	UE_LOG(LogDunGenBuild, LOGCOLOR, TEXT("Doors Built!"));
}


// FRuntimeDungeonGenerator
FRuntimeDungeonGenerator::FRuntimeDungeonGenerator()
{

}

FRuntimeDungeonGenerator::~FRuntimeDungeonGenerator()
{

}

void FRuntimeDungeonGenerator::BuildDungeon(const int32 RoomsCount)
{
	
}

void FRuntimeDungeonGenerator::BuildRooms()
{
	
}

void FRuntimeDungeonGenerator::BuildDoors()
{
	
}

TArray<ADungeonRoom*> FRuntimeDungeonGenerator::GetRooms()
{
	TArray<ADungeonRoom*> Rooms = {};
	UWorld* RuntimeWorld = GEngine->GetWorld();
	
	if (RuntimeWorld->WorldType != EWorldType::Game &&
		RuntimeWorld->WorldType != EWorldType::PIE)
	{
		return Rooms;
	}
	
	return Rooms;
}

TArray<ADoor*> FRuntimeDungeonGenerator::GetDoors()
{
	TArray<ADoor*> Doors = {};
	UWorld* RuntimeWorld = GEngine->GetWorld();
	
	if (RuntimeWorld->WorldType != EWorldType::Game &&
		RuntimeWorld->WorldType != EWorldType::PIE)
	{
		return Doors;
	}	
	
	return Doors;
}


// FDungeonGeneratorModule
void FDungeonGeneratorModule::StartupModule()
{
	DungeonUtils = MakeShared<FDungeonUtilities>();


	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	DungeonAssetTypeCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Dungeon Elements")), LOCTEXT("DungeonElementsText", "Dungeon Elements"));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FRoomPresetAssetTypeAction()));

	DungeonUtils->CreateGenerationSettings();

	FAssetRegistryModule::GetRegistry().OnFilesLoaded()
		.AddRaw(DungeonUtils.Get(), &FDungeonUtilities::GetPresetsOnLoad);
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetDeleted()
		.AddLambda([this](UObject* Object)
    {    	
    	if (const URoomPresetPtr Preset = Cast<URoomPreset>(Object))
    	{
			DungeonUtils->DeletePresetReference(Preset);
    		UE_LOG(LogDunGenStartup, LOGCOLOR, TEXT("In Memory Asset Deleted: %s"), *Object->GetName());
    	}    	
    });

	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw
		(this, &FDungeonGeneratorModule::ExtendContentBrowser));

	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	FTabSpawnerEntry SpawnerEntry = TabManager->RegisterNomadTabSpawner(DungeonGeneratorTabName,
		FOnSpawnTab::CreateRaw(this, &FDungeonGeneratorModule::SpawnNomadTab));

	// Uncomment only if tab is not on Editor, then re-comment as well as it is.
	//TabManager->TryInvokeTab(DungeonGeneratorTabName);
	
	UE_LOG(LogDunGenStartup, LOGCOLOR, TEXT("Dungeon Generator Startup"));
}

void FDungeonGeneratorModule::ShutdownModule()
{
	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	TabManager->UnregisterNomadTabSpawner(DungeonGeneratorTabName);
	
	UE_LOG(LogDunGenShutdown, LOGCOLOR, TEXT("Dungeon Generator Shutdown"));
}

bool FDungeonGeneratorModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	const FString CmdAsString = Cmd;
	
	if (CmdAsString == "querypresets")
	{
		UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Number of Asset Files: %d"),
			DungeonUtils->GetPresetFilesCount());
		//for (const URoomPresetPtr Data : DungeonUtils->GetRoomPresets())
		for (const TPair<int32, TSoftObjectPtr<URoomPreset>> Data : DungeonUtils->GetPresets())
		{
			const FString AssetName = Data.Value->GetName();
			const FString AssetPath = Data.Value->GetPathName();
			//UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Asset File: %s"), *Data->GetName());
			UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Registered Asset File: %s | At: %s"), *AssetName, *AssetPath);
		}
		for (const FString Path : DungeonUtils->GetPresetPaths())
		{
			UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Registered Asset File Path: %s"), *Path);
		}

	 	//UE_LOG(LogDunGenExecQueryFiles, Display, TEXT("Scan status: %d"), DungeonUtils->RescanAssetReferences());
		
		return true;
	}
	else if (CmdAsString == "querydungeon")
	{
		UE_LOG(LogTemp, Display, TEXT("/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-"));
		UE_LOG(LogTemp, Display, TEXT("Querying Dungeon Information..."));
		
		UE_LOG(LogTemp, Display, TEXT("Dungeon Grid Size:							%s"),
			*CurrentDungeonInfo.GridSize.ToString());
		
		const int32 RoomsCount = CurrentDungeonInfo.RoomsInfo.Num();
		UE_LOG(LogTemp, Display, TEXT("Dungeon Rooms Number:						%d"),
			RoomsCount);
		
		for (int32 Index = 0; Index < RoomsCount; Index++)
		{
			const FRoomInfo CurrentRoom = CurrentDungeonInfo.RoomsInfo[Index];			
			
			//const URoomPresetPtr GetPreset = DungeonUtils->GetRoomPresetByPath(CurrentRoom.PresetPath);
			const URoomPresetPtr GetPreset = DungeonUtils->GetPresetByID(CurrentRoom.PresetID);
			UE_LOG(LogTemp, Display, TEXT("-|- Room of index %d Preset Used:			%s"),
				Index, *GetPreset->GetName());

			UE_LOG(LogTemp, Display, TEXT("-|- Room of index %d Name:					%s"),
				Index, *CurrentRoom.RoomName.ToString());

			UE_LOG(LogTemp, Display, TEXT("-|- Room of index %d Coords in Grid:			%s"),
				Index, *CurrentRoom.CoordinateInGrid.ToString());

			UE_LOG(LogTemp, Display, TEXT("-|- Room of index %d Index in Grid:			%d"),
				Index, CurrentRoom.IndexInGrid);

			const int32 DoorsCount = CurrentRoom.DoorsInfo.Num();
			UE_LOG(LogTemp, Display, TEXT("-|- Room of index %d Number of Doors:		%d"),
				Index, DoorsCount);

			for (int32 JIndex = 0; JIndex < DoorsCount; JIndex++)
			{
				const FDoorInfo CurrentDoor = CurrentRoom.DoorsInfo[JIndex];

				FString DirectionName;
				switch (CurrentDoor.Direction)
				{
					case EWorldDirection::NORTH:
						DirectionName = TEXT("NORTH");
						break;
					case EWorldDirection::EAST:
                    	DirectionName = TEXT("EAST");
                    	break;
                    case EWorldDirection::SOUTH:
                    	DirectionName = TEXT("SOUTH");
                    	break;
					case EWorldDirection::WEST:
						DirectionName = TEXT("WEST");
						break;
					
					default:
						DirectionName = TEXT("NONE");
						break;
				}
				
				UE_LOG(LogTemp, Display, TEXT("-|- -|- Door at %s Destination Room:		%s"),
				*DirectionName, *CurrentDoor.NextRoomName.ToString());
			}
		}
		UE_LOG(LogTemp, Display, TEXT("/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-"));
		
		return true;
	}

	return false;
}


// Commands
void FDungeonGeneratorModule::DGCommandGenerateDungeonData()
{
	CurrentDungeonInfo.Reset();

	const int32 RoomsCount = DungeonUtils->GetDungeonRoomsCount();
	if (RoomsCount <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Rooms Count value is not valid! Please try with another value."));
		return;
	}
	
	FDungeonGenerator DungeonGenerator;
	DungeonGenerator.BuildDungeon(RoomsCount);
	DungeonGenerator.BuildRooms();
	DungeonGenerator.BuildDoors();
	CurrentDungeonInfo = DungeonGenerator.GetDungeonInfo();

	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);

	UE_LOG(LogTemp, LOGCOLOR, TEXT("A NEW DUNGEON HAS BEEN GENERATED!"));
}

void FDungeonGeneratorModule::DGCommandPreview()
{
	// Preview Operations

	UWorld* World = GEditor->GetEditorWorldContext().World();
	USelection* Selection = GEditor->GetSelectedActors();

	AEditorDungeon* EditorDungeon = DungeonUtils->GetSelectedActor<AEditorDungeon>(World, Selection);

	EditorDungeon->Build();
	
	UE_LOG(LogTemp, LOGCOLOR, TEXT("HERE'S A PREVIEW OF THE MAP!"));
}

/* Future Reimplementation
void FDungeonGeneratorModule::DGCommandSave()
{
	// Save Operations

	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);

	UE_LOG(LogTemp, LOGCOLOR, TEXT("CURRENT DUNGEON HAS BEEN SAVED!"));
}

void FDungeonGeneratorModule::DGCommandReset()
{
	// Reset Operations

	CurrentDungeonInfo.Reset();
	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);

	UE_LOG(LogTemp, LOGCOLOR, TEXT("CURRENT DUNGEON RESET TO PREVIOUS STATE!"));
}
*/

void FDungeonGeneratorModule::DGCommandRoomPreview()
{
	USelection* Selection = GEditor->GetSelectedActors();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = DungeonUtils->GetSelectedActor<AEditorRoom>(World, Selection);

	const URoomPresetPtr SelectedPreset = FDungeonUtilities::Get()->GetSelectedPreset();
	if (SelectedPreset)
	{
		FRoomInfo Info;
		Info.PresetID = SelectedPreset->PresetID;
		Info.PresetPath = *SelectedPreset->GetPathName();
		Info.PresetName = *SelectedPreset->GetName();
		Info.RoomName = SelectedPreset->RoomName;

		SelectedRoom->Preview(Info);
		
		UE_LOG(LogTemp, LOGCOLOR, TEXT("HERE'S A PREVIEW OF THE SELECTED ROOM!"));
	}
}

void FDungeonGeneratorModule::DGCommandRoomSaveChanges()
{
	USelection* Selection = GEditor->GetSelectedActors();
	
	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = DungeonUtils->GetSelectedActor<AEditorRoom>(World, Selection);

	URoomPresetPtr SelectedPreset = DungeonUtils->GetSelectedPreset();
	if (SelectedPreset)
	{
		SelectedRoom->OverwritePresetData(SelectedPreset);
		UE_LOG(LogTemp, LOGCOLOR, TEXT("SELECTED ROOM SAVED!"));
	}
}

void FDungeonGeneratorModule::DGCommandRoomResetChanges()
{
	USelection* Selection = GEditor->GetSelectedActors();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	AEditorRoom* SelectedRoom = DungeonUtils->GetSelectedActor<AEditorRoom>(World, Selection);

	// Reset Implementation

}


void FDungeonGeneratorModule::RegisterRoomPresetAssetPath(FMenuBuilder& MenuBuilder, const TArray<FAssetData> Data)
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

TSharedRef<FExtender> FDungeonGeneratorModule::ExtendContentBrowser(const TArray<FAssetData>& Data)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	Extender->AddMenuExtension(FName("CommonAssetActions"), EExtensionHook::After, nullptr, 
							   FMenuExtensionDelegate::CreateRaw(this, &FDungeonGeneratorModule::RegisterRoomPresetAssetPath, Data));
	return Extender;
}

TSharedRef<SDockTab> FDungeonGeneratorModule::SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs)
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
	(this, &FDungeonGeneratorModule::DGCommandGenerateDungeonData)), NAME_None, DGenerateText);
	
	// Dungeon Generation Features
	const FText DShowPreviewText = LOCTEXT("ToolbarShowPreviewText", "Preview");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorModule::DGCommandPreview)), NAME_None, DShowPreviewText);
	
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
	(this, &FDungeonGeneratorModule::DGCommandRoomPreview)), NAME_None, RShowPreviewText);

	const FText RSaveChangesText = LOCTEXT("ToolbarSaveText", "Save");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorModule::DGCommandRoomSaveChanges)), NAME_None, RSaveChangesText);

	const FText RResetText = LOCTEXT("ToolbarResetText", "Reset");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateRaw
	(this, &FDungeonGeneratorModule::DGCommandRoomResetChanges)), NAME_None, RResetText);

	ToolbarBuilder.AddSeparator();
	
	const TSharedRef<SWidget> Toolbar = ToolbarBuilder.MakeWidget();


	const int32 DGSpacerSize = 10;
	const int32 HPadding = 10;
	const int32 VPadding = 10;


	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SVerticalBox)

		// Toolbar
		+SVerticalBox::Slot().AutoHeight()
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
									DungeonUtils->SetDungeonDataPath(*InText.ToString());
									DungeonUtils->SaveGenerationSettings();
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
									DungeonUtils->SetDungeonRoomsCount(FCString::Atoi(*InText.ToString()));
									DungeonUtils->SaveGenerationSettings();
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
										SNew(SAssetDropTarget).OnAssetDropped_Lambda([this](UObject* Selection)
										{
											URoomPreset* CastObject = Cast<URoomPreset>(Selection);
											if (CastObject)
											{
												DungeonUtils->SelectedPreset = Cast<URoomPreset>(Selection);
											}
										})
										[
											SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
											[
												SNew(STextBlock).Text_Lambda([this]() -> FText
												{
													return DungeonUtils->SelectedPreset ?
														FText::FromString(DungeonUtils->SelectedPreset->GetName())
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
		+SVerticalBox::Slot().VAlign(VAlign_Top)
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
				+SVerticalBox::Slot().AutoHeight()
				.Padding(HPadding, VPadding, HPadding, 0)
				[
					SNew(SVerticalBox)

					// Query Dungeon File Path
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0, VPadding, 0, 0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryDungeonFile Path", "Dungeon File saved at: {0}"),
									FText::FromString(*DungeonUtils->GetDungeonDataPath()));
						})
					]

					// Query Dungeon Rooms Count
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0, VPadding, 0, 0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(LOCTEXT("QueryDungeonRoomsText", "Dungeon Rooms Count: {0}"),
												 FText::FromString(FString::FromInt(DungeonUtils->GetDungeonRoomsCount())));
						})
					]

					// Query Preset Files Count
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0, VPadding, 0, 0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(LOCTEXT("QueryNumOfAssetFiles", "Preset Asset Files: {0}"),
												 DungeonUtils->GetPresetFilesCount());
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
	
IMPLEMENT_MODULE(FDungeonGeneratorModule, DungeonGenerator)