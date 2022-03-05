// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonGenerator.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DungeonData.h"
#include "DungeonRoom.h"
#include "DungeonSchemeMaker.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "SRoomSelectorCombo.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/LayerManager/SLayerManager.h"
#include "Widgets/Layout/SSpacer.h"


#define LOCTEXT_NAMESPACE "FDungeonGeneratorModule"


// FRoomGenerator
FRoomGenerator::FRoomGenerator()
{

}

ADungeonRoom* FRoomGenerator::Generate(const FRoomInfo& Info)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();

	ADungeonRoom* Room = World->SpawnActor<ADungeonRoom>
		(ADungeonRoom::StaticClass(), FTransform::Identity);
	
	InsertData(Room, Info);

	return Room;
}

void FRoomGenerator::InsertData(ADungeonRoom* Room, const FRoomInfo& Info)
{
	Room->RoomInfo = Info;
}

void FRoomGenerator::Locate(ADungeonRoom* Room)
{
	const FVector Location = FVector(
    	
		Room->RoomInfo.CoordinateInGrid.Y * 5000.f,
		-Room->RoomInfo.CoordinateInGrid.X * 5000.f,
		0);
	Room->SetActorLocation(Location);

	if (Room->DoorsRef.Num() > 0)
	{
		for(ADoor* Door : Room->DoorsRef)
		{
			FName DirectionName;
			switch (Door->DoorInfo.Direction)
			{
				case EWorldDirection::NORTH:
					DirectionName = TEXT("North");
					break;
				case EWorldDirection::EAST:
					DirectionName = TEXT("East");
					break;
				case EWorldDirection::SOUTH:
					DirectionName = TEXT("South");
					break;
				case EWorldDirection::WEST:
					DirectionName = TEXT("West");
					break;
				default:
					break;
			}
			const FVector  DoorLocation = Room->FloorMeshComponent->
				GetSocketLocation(DirectionName);
			const FRotator DoorRotation = Room->FloorMeshComponent->
				GetSocketRotation(DirectionName);
			Door->SetActorLocationAndRotation(DoorLocation, DoorRotation);
		}
	}
}

void FRoomGenerator::Show(ADungeonRoom* Room)
{
	const FSoftObjectPath Path(Room->RoomInfo.PresetPath);
	const TSoftObjectPtr<URoomPreset> Preset(Path);
	
	Room->FloorMeshComponent->SetStaticMesh(Preset->FloorMesh);
	Room->WallsMeshComponent->SetStaticMesh(Preset->WallsMesh);
	
	if (Room->DoorsRef.Num() > 0)
	{
		for(ADoor* Door : Room->DoorsRef)
		{
			Door->DoorMesh->SetStaticMesh(Preset->DoorsMesh);
		}
	}
}


// FDungeonGenerator
FDungeonGenerator::FDungeonGenerator()
{
	
}

/*FDungeonGenerator::~FDungeonGenerator()
{
	
}*/

void FDungeonGenerator::BuildDungeon(const int32 RoomsCount)
{
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Building Dungeon..."));

	FDungeonGridMaker DungeonGridMaker(RoomsCount);

	FGrid Grid = DungeonGridMaker.GetGrid();
	const TArray<int32> Scheme = Grid.GetScheme();
	const TArray<int32> SchemePath = Grid.GetSchemePath();
	
	OutDungeonInfo = {};
	OutDungeonInfo.Grid = Grid;
	OutDungeonInfo.GridSize = Grid.Size;	
	OutDungeonInfo.GridScheme = Scheme;

	if (RoomsCount <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Rooms Count is not valid!"));
		return;
	}

	// Debug
	/*UE_LOG(LogTemp, MESSAGECOLOR, TEXT("+--------------------------------+"));
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Grid Scheme"));
	for (int32 Y = Grid.Size.Y - 1; Y >= 0; Y--)
	{
		FString Row("");
		for (int32 X = Grid.Size.X - 1; X >= 0; X--)
		{
			const int32 Index = X + (Y * Grid.Size.X);
			Row.Append(Scheme[Index] == 0 ?
				TEXT("[--]") : FString::Printf(TEXT("[%02d]"), SchemePath[Index]));
		}
		UE_LOG(LogTemp, MESSAGECOLOR, TEXT("%s"), *Row);
	}*/
}

void FDungeonGenerator::BuildRooms()
{
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Building Rooms..."));
	
	const int32 Length = OutDungeonInfo.Grid.PathLength;
	OutDungeonInfo.RoomsInfo.Init({}, Length);

	
	
	for (int32 Index = 0; Index < Length; Index++)
	{	
		FRoomInfo Room;
		if (FDungeonUtils::Get()->GetPresetsCount() > 0)
		{
			const int32 RandomPresetsMax = FDungeonUtils::Get()->GetPresetsCount() - 1;
			const int32 RandomPresetIndex = Index == 0 ? 0 : FMath::RandRange(1, RandomPresetsMax);
			const URoomPresetPtr RandomPreset = FDungeonUtils::Get()->GetRoomPresets()[RandomPresetIndex];
			Room.PresetPath = *RandomPreset->GetPathName();
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
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Rooms Built!"));
}

void FDungeonGenerator::BuildDoors()
{
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Building Doors..."));
	TArray<FRoomInfo>& OutRoomsInfo = OutDungeonInfo.RoomsInfo;
	
	for (int32 RoomIndex = 1; RoomIndex < OutRoomsInfo.Num(); RoomIndex++)
	{
		const int32 PrevRoomIndex = RoomIndex - 1;
		//UE_LOG(LogTemp, Display, TEXT("Index: %d | PrevIndex: %d"), RoomIndex, PrevRoomIndex);
		FRoomInfo& PrevRoom = OutRoomsInfo[PrevRoomIndex];
		FRoomInfo& CurrRoom = OutRoomsInfo[RoomIndex];
		FDoorInfo PrevDoor, CurrDoor;

		const FIntVector DeltaCoords =
			CurrRoom.CoordinateInGrid - PrevRoom.CoordinateInGrid;
		const int32 PatternIndex = Directions.IndexOfByKey(DeltaCoords);

		//UE_LOG(LogTemp, Display, TEXT("Index: %d | Delta: %s"), PatternIndex, *DeltaCoords.ToString());

		PrevDoor.SourceRoomName = PrevRoom.RoomName;
		PrevDoor.SourceRoomIndex = PrevRoomIndex;
		PrevDoor.NextRoomName = CurrRoom.RoomName;
		PrevDoor.NextRoomIndex = RoomIndex;
		const int32 CurrDirection = PatternIndex;
		//	FDungeonGridMaker::GetPatternFromIndex(PatternIndex);
		PrevDoor.Direction = static_cast<EWorldDirection>(PatternIndex);

		CurrDoor.SourceRoomName = PrevRoom.RoomName;
		CurrDoor.SourceRoomIndex = RoomIndex;
		CurrDoor.NextRoomName = CurrRoom.RoomName;
		CurrDoor.NextRoomIndex = PrevRoomIndex;
		const int32 PrevDirection = FDungeonGridMaker::GetOppositeIndex(PatternIndex);
		//	FDungeonGridMaker::GetPatternFromIndex(
		//		FDungeonGridMaker::GetOppositeIndex(PatternIndex));
		CurrDoor.Direction = static_cast<EWorldDirection>(PrevDirection);

		PrevRoom.DoorsInfo.Add(PrevDoor);
		CurrRoom.DoorsInfo.Add(CurrDoor);

		//OutDoorsInfo.Add(PrevDoor);
		//OutDoorsInfo.Add(CurrDoor);		
	}
	OutDungeonInfo.RoomsInfo = OutRoomsInfo;
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Doors Built!"));
}


// FRuntimeDungeonGenerator
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

	FRoomGenerator Generator;
	
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
	DungeonUtils		= MakeShared<FDungeonUtils>();

	FAssetRegistryModule::GetRegistry().OnFilesLoaded()
		.AddRaw(DungeonUtils.Get(), &FDungeonUtils::GetPresetsOnLoad);
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetDeleted()
		.AddLambda([this](UObject* Object)
    {    	
    	if (const URoomPresetPtr Preset = Cast<URoomPreset>(Object))
    	{
    		DungeonUtils.Get()->DeleteAssetReference(Preset);
    		UE_LOG(LogTemp, MESSAGECOLOR, TEXT("In Memory Asset Deleted: %s"), *Object->GetName());
    	}    	
    });
	
	/*// DEBUG AssetRegistry Events
	FAssetRegistryModule::GetRegistry().OnAssetAdded().AddLambda([](const FAssetData& Data)
		{UE_LOG(LogTemp, Warning, TEXT("On Asset Added: %s"), *Data.AssetName.ToString());});
	FAssetRegistryModule::GetRegistry().OnAssetRemoved().AddLambda([](const FAssetData& Data)
		{UE_LOG(LogTemp, Warning, TEXT("On Asset Removed: %s"), *Data.AssetName.ToString());});
	FAssetRegistryModule::GetRegistry().OnAssetUpdated().AddLambda([](const FAssetData& Data)
		{UE_LOG(LogTemp, Warning, TEXT("On Asset Updated: %s"), *Data.AssetName.ToString());});
	FAssetRegistryModule::GetRegistry().OnAssetRenamed().AddLambda([](const FAssetData& Data,  const FString& NewName)
		{UE_LOG(LogTemp, Warning, TEXT("On Asset Renamed: %s -> %s"), *Data.AssetName.ToString(), *NewName);});
	FAssetRegistryModule::GetRegistry().OnFilesLoaded().AddLambda([]()
		{UE_LOG(LogTemp, Warning, TEXT("On Files Loaded") );});
	FAssetRegistryModule::GetRegistry().OnPathAdded().AddLambda([](const FString& Path)
		{UE_LOG(LogTemp, Warning, TEXT("On Path Added: %s"), *Path);});
	FAssetRegistryModule::GetRegistry().OnPathRemoved().AddLambda([](const FString& Path)
		{UE_LOG(LogTemp, Warning, TEXT("On Path Removed: %s"), *Path);});
	//FAssetRegistryModule::GetRegistry().OnFileLoadProgressUpdated().AddLambda([](const FFileLoadProgressUpdateData&)
	//	{});
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetCreated().AddLambda([](UObject* Object)
		{UE_LOG(LogTemp, Warning, TEXT("On In Memory Asset Created: %s"), *Object->GetName());});
	FAssetRegistryModule::GetRegistry().OnInMemoryAssetDeleted().AddLambda([](UObject* Object)
		{UE_LOG(LogTemp, Warning, TEXT("On In Memory Asset Deleted: %s"), *Object->GetName());});
	// END DEBUG*/
	
	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	FTabSpawnerEntry SpawnerEntry = TabManager->RegisterNomadTabSpawner(DungeonGeneratorTabName,
		FOnSpawnTab::CreateRaw(this, &FDungeonGeneratorModule::SpawnNomadTab));

	// Uncomment only if tab is not on Editor, then re-comment as well as it is.
	//TabManager->TryInvokeTab(DungeonGeneratorTabName);
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Dungeon Generator Startup"));
}

void FDungeonGeneratorModule::ShutdownModule()
{
	const TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	TabManager->UnregisterNomadTabSpawner(DungeonGeneratorTabName);
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("Dungeon Generator Shutdown"));
}

bool FDungeonGeneratorModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	const FString CmdAsString = Cmd;
	
	if (CmdAsString == "queryfiles")
	{
		UE_LOG(LogTemp, Display, TEXT("Number of Asset Files: %d | Path: %s"),
			DungeonUtils->GetPresetsCount(), *DungeonUtils->PrefabricsPath.ToString());
		for (const URoomPresetPtr Data : DungeonUtils->GetRoomPresets())
		{
			UE_LOG(LogTemp, Display, TEXT("Asset File: %s"), *Data->GetName());
		}
	 	UE_LOG(LogTemp, Display, TEXT("Scan status: %d"), DungeonUtils->RescanAssetReferences());
		
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
			
			
			const URoomPresetPtr GetPreset = DungeonUtils->GetRoomPresetByPath(CurrentRoom.PresetPath);
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
FReply FDungeonGeneratorModule::DGCommandGenerate()
{
	CurrentDungeonInfo.Reset();
	
	FDungeonGenerator DungeonGenerator;
	DungeonGenerator.BuildDungeon(FDungeonUtils::Get()->DungeonRoomsCount);
	DungeonGenerator.BuildRooms();
	DungeonGenerator.BuildDoors();
	CurrentDungeonInfo = DungeonGenerator.GetDungeonInfo();

	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);	

	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("A NEW DUNGEON HAS BEEN GENERATED!"));
	
	return FReply::Handled();
}

FReply FDungeonGeneratorModule::DGCommandSave()
{
	// Save Operations

	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("CURRENT DUNGEON HAS BEEN SAVED!"));

	return FReply::Handled();
}

FReply FDungeonGeneratorModule::DGCommandReset()
{
	// Reset Operations

	CurrentDungeonInfo.Reset();
	//CurrentDungeonData->Reset();
	CurrentDungeonData = DungeonUtils->SaveDungeonData(CurrentDungeonInfo);
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("CURRENT DUNGEON RESET TO PREVIOUS STATE!"));
	
	return FReply::Handled();
}

FReply FDungeonGeneratorModule::DGCommandPreview()
{
	// Preview Operations
	UWorld* World = GEditor->GetEditorWorldContext().World();
	FRoomGenerator RoomGenerator;
	
	if (RoomsRef.Num() > 0)
	{
		for (ADungeonRoom* Room : RoomsRef)
        {
        	World->DestroyActor(Room);	
        }
	}
	if (DoorsRef.Num() > 0)
	{
		for (ADoor* Door : DoorsRef)
        {
        	World->DestroyActor(Door);
        }
	}
	
	RoomsRef.Empty();
	DoorsRef.Empty();

	const UDungeonData* DungeonData = DungeonUtils->GetDungeonDataAsset();

	const int32 RoomsCount = DungeonData->PathLength;
	RoomsRef.Init(nullptr, RoomsCount);
	for (int32 RoomIndex = 0; RoomIndex < RoomsCount; RoomIndex++)
	{
		FRoomInfo RoomInfo;
		RoomInfo.PresetPath = DungeonData->RoomsPresetPaths[RoomIndex];
		RoomInfo.CoordinateInGrid = DungeonData->RoomsCoordinate[RoomIndex];
		RoomInfo.IndexInGrid = DungeonData->RoomsGridIndex[RoomIndex];

		ADungeonRoom* Room = RoomGenerator.Generate(RoomInfo);
		RoomsRef[RoomIndex] = Room;

		RoomGenerator.Show(Room);
		RoomGenerator.Locate(Room);
	}
	
	const int32 DoorsCount = DungeonData->DoorsDirection.Num();
    DoorsRef.Init(nullptr, DoorsCount);
	for (int32 DoorIndex = 0; DoorIndex < DoorsCount; DoorIndex++)
	{
		FDoorInfo DoorInfo;		
		DoorInfo.SourceRoomIndex = DungeonData->DoorsSourceRoomIndex[DoorIndex];
		DoorInfo.NextRoomIndex = DungeonData->DoorsNextRoomIndex[DoorIndex];
		DoorInfo.Direction = static_cast<EWorldDirection>
			(DungeonData->DoorsDirection[DoorIndex]);
		
		ADoor* Door = World->SpawnActor<ADoor>
				(ADoor::StaticClass(), FTransform::Identity);
		Door->DoorInfo = DoorInfo;
		Door->LinkedRoom = RoomsRef[DoorInfo.NextRoomIndex];

		RoomsRef[DoorInfo.SourceRoomIndex]->DoorsRef.Add(Door);
		DoorsRef[DoorIndex] = Door;
	}
	for (int32 RoomIndex = 0; RoomIndex < RoomsCount; RoomIndex++)
	{
		RoomGenerator.Show(RoomsRef[RoomIndex]);
		RoomGenerator.Locate(RoomsRef[RoomIndex]);
	}
	
	UE_LOG(LogTemp, MESSAGECOLOR, TEXT("HERE'S A PREVIEW OF THE MAP!"));
	
	return FReply::Handled();
}

FReply FDungeonGeneratorModule::DGCommandRoomPreview()
{
	USelection* Selection = GEditor->GetSelectedActors();
	//UObject* SelectedObject = Selection->GetTop(ADungeonRoom::StaticClass());
	FRoomGenerator RoomGenerator{};
	ADungeonRoom* SelectedRoom = nullptr;
	
	if (UObject* SelectedObject = Selection->GetTop(ADungeonRoom::StaticClass()))
	{
		SelectedRoom = Cast<ADungeonRoom>(SelectedObject);
	}
	else
	{
		UWorld * World = GEditor->GetEditorWorldContext().World();
		SelectedRoom = World->SpawnActor<ADungeonRoom>(ADungeonRoom::StaticClass(), FTransform::Identity);
	}

	const URoomPresetPtr SelectedPreset = FDungeonUtils::Get()->GetSelectedPreset();
	if (SelectedPreset)
	{
		FRoomInfo Info;
		Info.PresetID = SelectedPreset->PresetID;
		Info.PresetPath = *SelectedPreset->GetPathName();
		
		RoomGenerator.InsertData(SelectedRoom, Info);
		RoomGenerator.Show(SelectedRoom);
		
		UE_LOG(LogTemp, MESSAGECOLOR, TEXT("HERE'S A PREVIEW OF THE SELECTED ROOM!"));
	}	
	
	return FReply::Handled();
}

void FDungeonGeneratorModule::RGCommandMakeRoom(const FText& InText, ETextCommit::Type InCommitType)
{
	if (InCommitType == ETextCommit::OnEnter)
	{
		DungeonUtils->RGCommandMakeRoom(InText.ToString());
	}
}


TSharedRef<SDockTab> FDungeonGeneratorModule::SpawnNomadTab(const FSpawnTabArgs& TabSpawnArgs)
{		
	FToolBarBuilder ToolbarBuilder(MakeShared<FUICommandList>(), FMultiBoxCustomization::None);

	const FText DGenerateText = LOCTEXT("ToolbarGenerate", "Generate");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		DGCommandGenerate();
	})), NAME_None, DGenerateText);
	
	ToolbarBuilder.AddSeparator();
	
	// Dungeon Generation Features
	const FText DShowPreviewText = LOCTEXT("ToolbarShowPreview", "Preview");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		DGCommandPreview();
	})), NAME_None, DShowPreviewText);
	const FText DSaveChangesText = LOCTEXT("ToolbarSaveText", "Save");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		DGCommandSave();
	})), NAME_None, DSaveChangesText);
	const FText DResetText = LOCTEXT("ToolbarResetText", "Reset");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		DGCommandReset();
	})), NAME_None, DResetText);
	
	ToolbarBuilder.AddSeparator();
	
	// Room Generation Features
	const FText RShowPreviewText = LOCTEXT("ToolbarShowPreview", "Preview");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		DGCommandRoomPreview();
	})), NAME_None, RShowPreviewText);
	const FText RSaveChangesText = LOCTEXT("ToolbarSaveText", "Save");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		
	})), NAME_None, RSaveChangesText);
	const FText RResetText = LOCTEXT("ToolbarResetText", "Reset");
	ToolbarBuilder.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([this]()
	{
		
	})), NAME_None, RResetText);

	ToolbarBuilder.AddSeparator();
	
	const TSharedRef<SWidget> Toolbar = ToolbarBuilder.MakeWidget();	

	const TSharedRef<SRoomSelectorCombo<URoomPresetPtr>> RoomSelectorCombo =
		SNew(SRoomSelectorCombo<URoomPresetPtr>).Source(&DungeonUtils->RoomPresets);
	RoomSelectorCombo->OnOptionSelected.BindLambda([this](const URoomPresetPtr Preset)
	{
		DungeonUtils->SelectedPreset = Preset;
	});

	
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SVerticalBox)
		// Toolbar
		+SVerticalBox::Slot().AutoHeight()
		[
			Toolbar
		]
		//+SVerticalBox::Slot()
		//[
		//	SNew(SSpacer).Size(DGSpacerSize)
		//]
		/*+SVerticalBox::Slot()
		[		
			SNew(SHorizontalBox)
			// Dungeon Generation Features
			+SHorizontalBox::Slot().AutoWidth().Padding(HPadding, 0, HPadding, 0)
			[
				SNew(SBox)
        	    .HAlign(HAlign_Left)
        	    .VAlign(VAlign_Top)
        	    [
        	    	SNew(SVerticalBox)
        	    	+SVerticalBox::Slot().AutoHeight().Padding(0,VPadding,0,0)//.HAlign(HAlign_Left)
        	    	[
        	    		SNew(STextBlock).Text(LOCTEXT("DungeonCommandsTitle", "Dungeon Generator Commands:"))
        	    	]
        	    	+SVerticalBox::Slot()
        	    	[
        	    		SNew(SSpacer).Size(DGSpacerSize)
        	    	]
        	    	+SVerticalBox::Slot().AutoHeight().Padding(0,0,0,0)//.HAlign(HAlign_Left)
        	    	[
        	    		SNew(SButton).HAlign(HAlign_Left).Text(LOCTEXT("GenerateNewDungeon", "Generate New Dungeon"))
        	    		.OnClicked_Raw(this, &FDungeonGeneratorModule::DGCommandGenerate)
        	    	]
        	    	+SVerticalBox::Slot()
        	    	[
        	    		SNew(SSpacer).Size(DGSpacerSize)
        	    	]
        	    	+SVerticalBox::Slot().AutoHeight().Padding(0,0,0,0)//.HAlign(HAlign_Left)
        	    	[
        	    		SNew(SButton).HAlign(HAlign_Left).Text(LOCTEXT("SaveCurrentDungeon", "Save Current Dungeon"))
        	    		.OnClicked_Raw(this, &FDungeonGeneratorModule::DGCommandSave)
        	    	]
        	    	+SVerticalBox::Slot()
					[
						SNew(SSpacer).Size(DGSpacerSize)
					]
					+SVerticalBox::Slot().AutoHeight().Padding(0,0,0,0)//.HAlign(HAlign_Left)
					[
						SNew(SButton).HAlign(HAlign_Left).Text(LOCTEXT("ResetCurrentDungeon", "Reset Current Dungeon"))
						.OnClicked_Raw(this, &FDungeonGeneratorModule::DGCommandReset)
					]
        	    ]
			]
			// Room Generation Features
			+SHorizontalBox::Slot().AutoWidth().Padding(HPadding, 0, HPadding, 0)
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot().AutoHeight()//.HAlign(HAlign_Left)
					.Padding(0,VPadding,0,0)
					[
						SNew(STextBlock).Text(LOCTEXT("RoomCommandsTitle", "Room Generator Commands:"))
					]
					+SVerticalBox::Slot()
					[
						SNew(SSpacer).Size(DGSpacerSize)
					]
					+SVerticalBox::Slot().AutoHeight()//.HAlign(HAlign_Left)
					.Padding(0,0,0,0)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot().AutoWidth()
						.Padding(0,0,HPadding,0)
						[
							SNew(STextBlock).Text(LOCTEXT("MakeRoomCommandTitle", "New File:"))
						]
						+SHorizontalBox::Slot().HAlign(HAlign_Fill)
						.Padding(0,0,0,0)
						[
							SNew(SEditableTextBox).HintText(LOCTEXT("NewFileName", "New File Name"))
							.OnTextCommitted_Raw(this, &FDungeonGeneratorModule::RGCommandMakeRoom)
						]					
					]
					+SVerticalBox::Slot()
					[
						SNew(SSpacer).Size(DGSpacerSize)
					]
					+SVerticalBox::Slot().AutoHeight()//.HAlign(HAlign_Left)
					.Padding(0,0,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(LOCTEXT("NumOfAssetFiles", "Asset Files: {0}"),
																DungeonUtils->GetPresetsCount());
						})
					]
					+SVerticalBox::Slot()
					[
						SNew(SSpacer).Size(DGSpacerSize)
					]
					+SVerticalBox::Slot().AutoHeight()//.HAlign(HAlign_Left)
					.Padding(0,0,0,0)
					[
						RoomSelectorCombo
					]
				]
			]
		]*/

		// Menu
		+SVerticalBox::Slot().AutoHeight()
		//.Padding(0,VPadding,0,0)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				// Dungeon
				+SHorizontalBox::Slot().AutoWidth()
				.Padding(HPadding, 0, HPadding, 0)
				[
					SNew(SVerticalBox)				
					
					// Set Dungeon Data Folder Path
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot().AutoWidth()
						.Padding(0,0,HPadding,0)
						[
							SNew(STextBlock).Text(LOCTEXT("DungeonFilePathText", "Dungeon File Path:"))
						]
						+SHorizontalBox::Slot().HAlign(HAlign_Fill)
						.Padding(0,0,0,0)
						[
							SNew(SEditableTextBox).HintText(LOCTEXT("PathText", "Path"))
							//.OnTextCommitted_Raw(this, &FDungeonGeneratorModule::RGCommandMakeRoom)
							.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
							{
								DungeonUtils->DungeonDataPath = *InText.ToString();
							})
						]
					]
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
	
					// Set Rooms count for your next Dungeon
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot().AutoWidth()
						.Padding(0,0,HPadding,0)
						[
							SNew(STextBlock).Text(LOCTEXT("SetRoomsCount", "Dungeon Rooms Count:"))
						]
						+SHorizontalBox::Slot().HAlign(HAlign_Fill)
						.Padding(0,0,0,0)
						[
							SNew(SEditableTextBox).HintText(LOCTEXT("CountText", "Count"))
							//.OnTextCommitted_Raw(this, &FDungeonGeneratorModule::RGCommandMakeRoom)
							.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
							{
								DungeonUtils->DungeonRoomsCount = FCString::Atoi(*InText.ToString());
							})
						]
					]				
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
				]
				// Room
				+SHorizontalBox::Slot().AutoWidth()
				.Padding(HPadding, 0, HPadding, 0)
				[
					SNew(SVerticalBox)
					// Set Presets Folder Path
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot().AutoWidth()
						.Padding(0,0,HPadding,0)
						[
							SNew(STextBlock).Text(LOCTEXT("PresetFilesPath", "Preset Files Path:"))
						]
						+SHorizontalBox::Slot().HAlign(HAlign_Fill)
						.Padding(0,0,0,0)
						[
							SNew(SEditableTextBox).HintText(LOCTEXT("PathText", "Path"))
							//.OnTextCommitted_Raw(this, &FDungeonGeneratorModule::RGCommandMakeRoom)
							.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType)
							{
								DungeonUtils->PrefabricsPath = *InText.ToString();
							})
						]
					]
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
					
					// Create new File
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(SHorizontalBox)
            	        +SHorizontalBox::Slot().AutoWidth()
            	        .Padding(0,0,HPadding,0)
            	        [
            	        	SNew(STextBlock).Text(LOCTEXT("MakeRoomCommandTitle", "New File:"))
            	        ]
            	        +SHorizontalBox::Slot().HAlign(HAlign_Fill)
            	        .Padding(0,0,0,0)
            	        [
            	        	SNew(SEditableTextBox).HintText(LOCTEXT("NewFileNameText", "New File Name"))
            	        	.OnTextCommitted_Raw(this, &FDungeonGeneratorModule::RGCommandMakeRoom)
            	        ]
					]
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
	
					// Select Preset File
					+SVerticalBox::Slot().AutoHeight()//.HAlign(HAlign_Left)
					.Padding(0,VPadding,0,0)
					[
						RoomSelectorCombo
					]
				]
			]
		]
		//+SVerticalBox::Slot()
		//[
		//	SNew(SSpacer).Size(DGSpacerSize)
		//]
		
		// Queries
		+SVerticalBox::Slot()
		.Padding(0,VPadding,0,0)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				// Dungeon
				+SVerticalBox::Slot().AutoHeight()
				.Padding(HPadding, 0, HPadding, 0)
				[
					SNew(SVerticalBox)
					// Query Dungeon File Path
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryDungeonFile Path", "Dungeon File saved at: {0}"),
											  FText::FromString(*DungeonUtils->DungeonDataPath.ToString()));
						})
					]
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
					// Query Dungeon Rooms Count
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding / 2,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							//return FText::Format(
							//	LOCTEXT("DungeonRoomsText", "Dungeon Rooms Count: {0}"),
							//				  DungeonUtils->CurrentDungeonFile ?
							//				  DungeonUtils->CurrentDungeonFile->PathLength : 0);
							return LOCTEXT("QueryDungeonRoomsText", "Dungeon Rooms Count: 0");
						})
					]

					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryPresetFilesPath", "Preset Files saved at: {0}"),
											FText::FromString(*DungeonUtils->PrefabricsPath.ToString()));
						})
					]

					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding / 2,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryNumOfAssetFiles", "Asset Files: {0}"),
											DungeonUtils->GetPresetsCount());
						})
					]
				]
				// Room
				/*+SVerticalBox::Slot().AutoHeight()
				.Padding(0, VPadding, HPadding, 0)
				[
					SNew(SVerticalBox)
					// Query Room Presets Path
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryPresetFilesPath", "Preset Files saved at: \n{0}"),
											  FText::FromString(*DungeonUtils->PrefabricsPath.ToString()));
						})
					]
					//+SVerticalBox::Slot()
					//[
					//	SNew(SSpacer).Size(DGSpacerSize)
					//]
					// Query Room Presets Count
					+SVerticalBox::Slot().AutoHeight()
					.Padding(0,VPadding,0,0)
					[
						SNew(STextBlock).Text_Lambda([this]() -> FText
						{
							return FText::Format(
								LOCTEXT("QueryNumOfAssetFiles", "Asset Files: {0}"),
											  DungeonUtils->GetPresetsCount());
						})
					]
				]*/
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDungeonGeneratorModule, DungeonGenerator)