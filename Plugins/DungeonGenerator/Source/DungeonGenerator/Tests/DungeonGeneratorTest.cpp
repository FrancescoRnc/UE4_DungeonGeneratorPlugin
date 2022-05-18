// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetRegistry/AssetRegistryModule.h"
#include "DungeonGenerator.h"
#include "GenerationSettings.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "Tests/AutomationEditorCommon.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorNoValidRoomsCount, "Generator.Commands.Generate.Dungeon.RoomsCount.ZeroNoValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorOneRoomInput, "Generator.Commands.Generate.Dungeon.RoomsCount.OneInputTwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorTwoDoorsLinkedCorrectly, "Generator.Commands.Generate.Door.TwoDoorrs.CorrectLinking.TwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorMoreRoomsFirstTwoDoorsLinkedCorrectly, "Generator.Commands.Generate.Door.TwoDoorrs.CorrectLinking.MoreRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorResetDungeonData, "Generator.Commands.Reset.Dungeon.WholeFileIsEmpty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSaveDungeonEmptyNoValid, "Generator.Commands.Save.Dungeon.EmptyInfoNoValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSaveDungeonValid, "Generator.Commands.Save.Dungeon.Valid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
//IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSettingStartupShutdown, "Generator.Files.Settings.VerifyStartupAndShutdown", EAutomationTestFlags::EditorContext | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSettingFileCreationAndDelete, "Generator.Files.Settings.CreateAndDeleteFile", EAutomationTestFlags::EditorContext | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSettingFileSave, "Generator.Files.Settings.SaveFile", EAutomationTestFlags::EditorContext | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorSettingFileLoad, "Generator.Files.Settings.LoadFile", EAutomationTestFlags::EditorContext | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


class FMockGeneratorMethod : public IGeneratorMethod
{
	public:

	FMockGeneratorMethod()
	{

	}

	virtual FGrid Generate(const FGrid EmptyGridSample, FGridMakerInfo& MakerInfo) override
	{
		FGrid NewGrid = EmptyGridSample;
		TArray<FIntVector> PathTrack;

		const FIntVector GridCenter = { MakerInfo.GridRadius, MakerInfo.GridRadius, 0 };
		FIntVector NextCoordinate = GridCenter;
		int32 NextGridIndex = NextCoordinate.X + (NextCoordinate.Y * MakerInfo.Size.X);
		int32 NextPatternIndex = 0;

		// First Cell setup
		NewGrid.StartPoint = GridCenter;
		PathTrack.Add(NewGrid.StartPoint);
		NewGrid[NextGridIndex].PathOrder = 1;
		NewGrid.CheckMinMaxCoordinate(GridCenter);

		// Now we are forcing the next Cell to go Up, by excluding Left, right and Down later

		for (int32 Index = 0; Index < MakerInfo.RoomsCount; Index++)
		{
			const int32 OppositePatternIndex = GetOppositeIndex(NextPatternIndex);

			// Next Cell Direction must be Up, so Next Direction Index must be 0
			int32 NewPatternIndex = 0;
			FIntVector NewCoordinate = NextCoordinate + Directions[NewPatternIndex];

			NextPatternIndex = NewPatternIndex;

			NextCoordinate = NewCoordinate;
			NewGrid.CheckMinMaxCoordinate(NextCoordinate);
			NextGridIndex = NextCoordinate.X + (NextCoordinate.Y * MakerInfo.Size.X);

			PathTrack.Add(NextCoordinate);
			NewGrid[NextGridIndex].PathOrder = PathTrack.Num();
			NewGrid[NextGridIndex].CellPattern =
				GetPatternFromIndex(OppositePatternIndex) |
				GetPatternFromIndex(NewPatternIndex);
		}

		NewGrid.EndPoint = NextCoordinate;

		NewGrid.PathTrack = PathTrack;
		NewGrid.PathLength = PathTrack.Num();

		return NewGrid;
	}
};

class FMemoryTestArchive : FMemoryArchive
{

};


bool FGeneratorNoValidRoomsCount::RunTest(const FString& Parameters)
{
	//FDungeonGeneratorModule& GeneratorModule = FModuleManager::LoadModuleChecked<FDungeonGeneratorModule>("DungeonGenerator");
		
	FDungeonGenerator Generator;

	Generator.BuildDungeon(0);
	FDungeonInfo Info = Generator.GetDungeonInfo();

	TestEqual(TEXT("Grid with Rooms Count equal to 0 should be empty"), Info.RoomsInfo.Num(), 0);	

	return true;
}

bool FGeneratorOneRoomInput::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	TestEqual(TEXT("This Grid with Rooms Count == 1 must have a Grid Size 1x2"), Info.GridSize, { 1,2,0 });
	TestEqual(TEXT("Every Grid with a Rooms Count greater than 0 will have an Extra Room (A Starting Room)"), Info.RoomsInfo.Num(), 2);	

	return true;
}

bool FGeneratorTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	FDoorInfo FirstRoomDoor = Info.RoomsInfo[0].DoorsInfo[0];
	FDoorInfo SecondRoomDoor = Info.RoomsInfo[1].DoorsInfo[0];

	TestEqual(TEXT("First Door of first Room must always point to North Direction"), FirstRoomDoor.Direction, EWorldDirection::NORTH);
	TestEqual(TEXT("First Door of first Room has its room as Source"), FirstRoomDoor.SourceRoomIndex, 0);
	TestEqual(TEXT("First Door of first Room must point to second Room"), FirstRoomDoor.NextRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must always point to South Direction"), SecondRoomDoor.Direction, EWorldDirection::SOUTH);
	TestEqual(TEXT("First Door of second Room has its room as Source"), SecondRoomDoor.SourceRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must point to South Direction"), SecondRoomDoor.NextRoomIndex, 0);		

	return true;
}

bool FGeneratorMoreRoomsFirstTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(2);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	FDoorInfo FirstRoomDoor = Info.RoomsInfo[0].DoorsInfo[0];
	FDoorInfo SecondRoomDoor = Info.RoomsInfo[1].DoorsInfo[0];

	TestEqual(TEXT("First Door of first Room must always point to North Direction"), FirstRoomDoor.Direction, EWorldDirection::NORTH);
	TestEqual(TEXT("First Door of first Room has its room as Source"), FirstRoomDoor.SourceRoomIndex, 0);
	TestEqual(TEXT("First Door of first Room must point to second Room"), FirstRoomDoor.NextRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must always point to South Direction"), SecondRoomDoor.Direction, EWorldDirection::SOUTH);
	TestEqual(TEXT("First Door of second Room has its room as Source"), SecondRoomDoor.SourceRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must point to South Direction"), SecondRoomDoor.NextRoomIndex, 0);

	return true;
}


bool FGeneratorResetDungeonData::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	Info.Reset();

	TestEqual(TEXT("After Resetting this file, Grid size must be 0"), Info.GridSize, { 0,0,0 });
	TestTrue(TEXT("After Resetting this file, Grid Scheme data must be empty"), Info.GridScheme.Num() == 0);
	TestTrue(TEXT("After Resetting this file, Rooms Info data must be empty"), Info.RoomsInfo.Num() == 0);

	return true;
}


bool FGeneratorSaveDungeonEmptyNoValid::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(0);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	UDungeonData* TmpDungeonData = FDungeonUtils::Get()->SaveDungeonData(Info);

	TestNull(TEXT("Save Command should return Null Object with no valid DungeonInfo"), TmpDungeonData);

	return true;
}

bool FGeneratorSaveDungeonValid::RunTest(const FString& Parameters)
{
	FDungeonGenerator Generator;

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	UDungeonData* TmpDungeonData = FDungeonUtils::Get()->SaveDungeonData(Info);

	TestNotNull(TEXT("Save Command should work with valid DungeonInfo"), TmpDungeonData);

	return true;
}


bool FGeneratorSettingFileCreationAndDelete::RunTest(const FString& Parameters)
{
	FDungeonUtils* DungeonUtils = FDungeonUtils::Get();


	// File Creation Test
	UGenerationSettings* Settings = nullptr;
	FFileReport CreationReport = DungeonUtils->CreateSettingsFile(TEXT("/DungeonGenerator/Test/Settings"), Settings);
	UPackage* Package = Settings->GetPackage();

	TestNotNull(TEXT("Settings File should not be Null after Plugin Startup"), Settings);
	TestTrue(TEXT("File should exist after Creation"), FPackageName::DoesPackageExist(TEXT("/DungeonGenerator/Test/Settings/Settings.pkj")));
	TestEqual(TEXT("Settings File Result after Creation should be CREATED"), CreationReport.ResultStatus, EFileResultStatus::CREATED);
	TestEqual(TEXT("Settings File should start with Initial Preset Files Count = 0"), Settings->InitialPresetFilesCount, 0);


	// File Delete Test
	FAssetData DataBeforeDelete = FAutomationEditorCommonUtils::GetAssetDataFromPackagePath(TEXT(".") + Package->GetPathName()); //FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(*SettingsPath);
	FFileReport DeleteReport = DungeonUtils->DeleteAsset(Settings);
	FAssetData DataAfterDelete = FAutomationEditorCommonUtils::GetAssetDataFromPackagePath(TEXT(".") + Settings->GetPackage()->GetPathName()); //FAssetRegistryModule::GetRegistry().GetAssetByObjectPath(*SettingsPath);

	TestEqual(TEXT("Delete an Asset should result VALID"), DeleteReport.CommandStatus, ECommandStatusType::VALID);
	TestEqual(TEXT("Settings File Result after Delete should be DELETED"), DeleteReport.ResultStatus, EFileResultStatus::DELETED);
	TestFalse(TEXT("File should not exist anymore after Delete"), FPackageName::DoesPackageExist(Package->GetName()));
	TestNull(TEXT("Original Object after Delete should be Null"), DataAfterDelete.GetAsset());


	return true;
}

bool FGeneratorSettingFileLoad::RunTest(const FString& Parameters)
{
	FDungeonUtils* DungeonUtils = FDungeonUtils::Get();

	UGenerationSettings* Settings = nullptr;
	DungeonUtils->CreateSettingsFile(TEXT("/DungeonGenerator/Test/Settings"), Settings);

	// File Reload Test
	UGenerationSettings* ReSettings = nullptr;
	FFileReport LoadReport = DungeonUtils->LoadSettings(TEXT("/DungeonGenerator/Test/Settings/Settings"), Settings);

	TestNotNull(TEXT("Settings File should not be Null after reload"), ReSettings);
	TestEqual(TEXT("Settings File Result after Load should be LOADED"), LoadReport.ResultStatus, EFileResultStatus::LOADED);
	TestEqual(TEXT("Reloaded File should have Initial Preset Files Count saved equal to 2"), ReSettings->InitialPresetFilesCount, 2);
	TestEqual(TEXT("Reloaded File should have Dungeon Data File Path saved equal to /DungeonGenerator/Test/Dungeon"), ReSettings->DungeonDataFolderPath, TEXT("/DungeonGenerator/Test/Dungeon"));

	DungeonUtils->DeleteAsset(Settings);

	return true;
}

bool FGeneratorSettingFileSave::RunTest(const FString& Parameters)
{
	FDungeonUtils* DungeonUtils = FDungeonUtils::Get();

	UGenerationSettings* Settings = nullptr;
	DungeonUtils->CreateSettingsFile(TEXT("/DungeonGenerator/Test/Settings"), Settings);

	// File Save Test
	Settings->InitialPresetFilesCount = 2;
	Settings->DungeonDataFolderPath = TEXT("/DungeonGenerator/Test/Dungeon");

	FFileReport SaveReport = DungeonUtils->SaveAsset(Settings);

	TestEqual(TEXT("Settings File Result after Save should be SAVED"), SaveReport.ResultStatus, EFileResultStatus::SAVED);
	TestEqual(TEXT("Save an Asset should result VALID"), SaveReport.CommandStatus, ECommandStatusType::VALID);

	DungeonUtils->DeleteAsset(Settings);

	return true;
}