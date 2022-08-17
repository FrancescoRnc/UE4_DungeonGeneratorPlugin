// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetRegistry/AssetRegistryModule.h"
#include "DungeonGenerator.h"
#include "DungeonGeneratorEditor.h"
#include "GenerationSettings.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "Tests/AutomationEditorCommon.h"


// Dungeon Data Generator classes Tests:
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridEmptyGrid, "Generator.Classes.FGrid.Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridSizeOne, "Generator.Classes.FGrid.SizeOne", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridSize1X2, "Generator.Classes.FGrid.Size1X2", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridGetCellWithIndex, "Generator.Classes.FGrid.GetCell", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridGetScheme, "Generator.Classes.FGrid.GetScheme", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridGetSchemePath, "Generator.Classes.FGrid.GetSchemePath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridMakerInfoEmpty, "Generator.Classes.FGridMakerInfo.Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFGridMakerInfoRoomsCountOne, "Generator.Classes.FGridMakerInfo.RoomsCountOne", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFDungeonGridMakerEmpty, "Generator.Classes.FDungeonGridMaker.Empty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFDungeonGridMakerGeneratorMethod, "Generator.Classes.FDungeonGridMaker.GeneratorMethod", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorFDungeonGridMakerTOneRoom, "Generator.Classes.FDungeonGridMaker.OneRoom", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorUDungeonDataExtractInfo, "Generator.Classes.UDungeonData.ExtractInfo", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

// 

// Dungeon Generation Commands:
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDungeonNoValidRoomsCount, "Generator.Commands.Dungeon.Generate.RoomsCountZeroNoValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDungeonOneRoomInput, "Generator.Commands.Dungeon.Generate.RoomsCountOneInputTwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDungeonResetDungeonData, "Generator.Commands.Dungeon.Reset.WholeFileIsEmpty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDungeonSaveDungeonEmptyNoValid, "Generator.Commands.Dungeon.Save.EmptyInfoNoValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDungeonSaveDungeonValid, "Generator.Commands.Dungeon.Save.Valid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDoorsTwoDoorsLinkedCorrectly, "Generator.Commands.Door.Generate.TwoDoorrs.CorrectLinking.TwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorCommandsDoorsMoreRoomsFirstTwoDoorsLinkedCorrectly, "Generator.Commands.Door.Generate.TwoDoorrs.CorrectLinking.MoreRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)


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

class FMemoryTestArchive : public FMemoryArchive
{
	
};


// FGrid Struct Tests:
bool FGeneratorFGridEmptyGrid::RunTest(const FString& Parameters)
{
	{
		FGrid NoParametersEmptyGrid;

		TestEqual(TEXT("Empty Grid without parameters has an empty array of Cells"), 
				  NoParametersEmptyGrid.Cells.Num(), 0);
	}	
	{
		FGrid EmptyGrid({ 0,0,0 });

		TestEqual(TEXT("Empty Grid with a Vector Zero as parameter has an empty array of Cells"),
				  EmptyGrid.Cells.Num(), 0);
	}	

	return true;
}

bool FGeneratorFGridSizeOne::RunTest(const FString& Parameters)
{
	const FIntVector Size = { 1,1,1 };
	FGrid Grid(Size);

	TestEqual(TEXT("Grid with a Vector One as parameter has an array of Cells with lenght == 1"),
			  Grid.Cells.Num(), 1);

	TestEqual(TEXT("Grid's Size field equals (1,2,0)"),
			  Grid.Size, { 1,1,0 });

	return true;
}

bool FGeneratorFGridSize1X2::RunTest(const FString& Parameters)
{
	const FIntVector Size = { 1,2,0 };
	FGrid Grid(Size);

	TestEqual(TEXT("Grid with a 1 X 2 size as parameter has an array of Cells with lenght == 2"),
			  Grid.Cells.Num(), 2);

	TestEqual(TEXT("Grid's Size field equals (1,1,1)"),
			  Grid.Size, { 1,2,0 });

	return true;
}

bool FGeneratorFGridGetCellWithIndex::RunTest(const FString& Parameters)
{
	const FIntVector Size = { 1,3,0 };
	FGrid Grid(Size);

	{	
		FGridCell FirstCell = Grid[0];
		FGridCell LastCell = Grid[2];
	
		//TestEqual<FGridCell>(TEXT("Grid Cell got from Index 0 is the first Cell of the array Cells inside Grid"),
		//		  FirstCell, Grid.Cells[0]);	
		TestEqual(TEXT("First Cell Index must be 0"),
				  FirstCell.Index, 0);
		TestEqual(TEXT("First Cell Coordinate must be (0,0,0)"),
				  FirstCell.Coordinate, { 0,0,0 });

		//TestEqual<FGridCell>(TEXT("Grid Cell got from Index 2 is the last Cell of the array Cells inside Grid"),
		//		  LastCell, Grid.Cells[2]);
		TestEqual(TEXT("Last Cell Index must be 2"),
				  LastCell.Index, 2);		
		TestEqual(TEXT("Last Cell Coordinate must be (0,2,0)"),
				  LastCell.Coordinate, { 0,2,0 });
	}
	
	{
		FGridCell GetSecondCell = Grid.GetCell(0,1); // Cartesian Coordinate
	
		//TestEqual<FGridCell>(TEXT("Grid Cell got from Coordinate (0,1) is the second Cell of the array Cells inside Grid"),
		//		  GetSecondCell, Grid.Cells[1]);
		TestEqual(TEXT("Second Cell Index must be 1"),
				  GetSecondCell.Index, 1);
		TestEqual(TEXT("Second Cell Coordinate must be (0,1,0)"),
				  GetSecondCell.Coordinate, { 0,1,0 });
	}

	return true;
}

bool FGeneratorFGridGetScheme::RunTest(const FString& Parameters)
{
	const FIntVector Size = { 1,4,0 };
	FGrid Grid(Size);
	Grid[0].CellPattern = 1;
	Grid[1].CellPattern = 2;
	Grid[2].CellPattern = 4;
	Grid[3].CellPattern = 8;

	TArray<int32> Scheme = Grid.GetScheme();

	TestEqual(TEXT("Grid Scheme should be (1,2,4,8)"),
			  Scheme, { 1,2,4,8 });

	return true;
}

bool FGeneratorFGridGetSchemePath::RunTest(const FString& Parameters)
{
	const FIntVector Size = { 1,3,0 };
	FGrid Grid(Size);
	Grid[0].PathOrder = 1;
	Grid[1].PathOrder = 2;
	Grid[2].PathOrder = 3;

	TArray<int32> SchemePath = Grid.GetSchemePath();

	TestEqual(TEXT("Grid Scheme Path should be (1,2,3)"),
			  SchemePath, { 1,2,3 });

	return true;
}


// FGridMakerInfo Struct Tests:
bool FGeneratorFGridMakerInfoEmpty::RunTest(const FString& Parameters)
{
	{
		FGridMakerInfo NoParametersEmptyMakerInfo;

		TestEqual(TEXT("Empty MakerInfo without parameters has an empty Size"),
				  NoParametersEmptyMakerInfo.Size, { 0,0,0 });
	}

	{
		FGridMakerInfo EmptyMakerInfo(0);

		TestEqual(TEXT("Empty MakerInfo with RoomsCount = 0 has an empty Size"),
				  EmptyMakerInfo.Size, { 0,0,0 });
	}	

	return true;
}

bool FGeneratorFGridMakerInfoRoomsCountOne::RunTest(const FString& Parameters)
{
	FGridMakerInfo EmptyMakerInfo(1);

	TestEqual(TEXT("MakerInfo with RoomsCount = 1 has GridRadius == 2"),
			  EmptyMakerInfo.GridRadius, 2);
	TestEqual(TEXT("MakerInfo with RoomsCount = 1 has RoomsCount == 1"),
			  EmptyMakerInfo.RoomsCount, 1);
	TestEqual(TEXT("MakerInfo with RoomsCount = 1 has Size == (5, 5, 25)"),
			  EmptyMakerInfo.Size, { 5, 5, 25 });
	// Size.Z is the Lenght for the Grid to make.


	return true;
}


// FDungeonGridMaker Class Tests:
bool FGeneratorFDungeonGridMakerEmpty::RunTest(const FString& Parameters)
{
	{
		FDungeonGridMaker NoParametersEmptyGridMaker;
		NoParametersEmptyGridMaker.Make();

		TestEqual(TEXT("Grid Maker without parameters has Grid Length == 0"),
				  NoParametersEmptyGridMaker.GetGrid().Length, 0);
	}
	
	{
		FDungeonGridMaker Count0EmptyGridMaker(0, nullptr);
		Count0EmptyGridMaker.Make();

		TestEqual(TEXT("Grid Maker with RoomsCount parameter 0 has Grid Length == 0"),
				  Count0EmptyGridMaker.GetGrid().Length, 0);
	}

	{
		FDungeonGridMaker NullMethodEmptyGridMaker(2, nullptr);
		NullMethodEmptyGridMaker.Make();

		TestEqual(TEXT("Grid Maker with InMethod as nullptr has Grid Length == 0"),
				  NullMethodEmptyGridMaker.GetGrid().Length, 0);
	}

	return true;
}

bool FGeneratorFDungeonGridMakerGeneratorMethod::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonGridMaker GridMaker(1, Method.Get());

	FGrid Grid = GridMaker.GetFromMethod();
	TArray<int32> Path = Grid.GetSchemePath();

	TestEqual(TEXT("Grid Maker with RoomsCount 1 gives a 5x5 grid"),
			  Grid.Size, { 5, 5, 0 });
	TestEqual(TEXT("The Central Grid Cell has Path Order == 1"),
			  Path[12], 1);
	TestEqual(TEXT("The Upper Grid Cell has Path Order == 2"),
			  Path[17], 2);

	return true;
}

bool FGeneratorFDungeonGridMakerTOneRoom::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonGridMaker GridMaker(1, Method.Get());

	FGrid Grid = GridMaker.Make();

	TArray<int32> Scheme = Grid.GetScheme();
	TArray<int32> Path = Grid.GetSchemePath();

	TestEqual(TEXT("New generated Grid has a Size of 1X2"),
			  Grid.Size, {1, 2, 0});
	TestEqual(TEXT("New generated Grid has a Pattern like: First Cell - 1; Last Cell - 4"),
			  Scheme, {1, 4});
	TestEqual(TEXT("New generated Grid has a Path like: 1 - 2"),
			  Path, {1, 2});

	return true;
}

bool FGeneratorUDungeonDataExtractInfo::RunTest(const FString& Parameters)
{

	return true;
}


// Commands:
bool FGeneratorCommandsDungeonNoValidRoomsCount::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

	Generator.BuildDungeon(0);
	FDungeonInfo Info = Generator.GetDungeonInfo();

	TestEqual(TEXT("Grid Generation is NO Valid"), Info.State, EDungeonInfoState::NOVALID);
	TestEqual(TEXT("Grid with Rooms Count value equal to 0 should be empty"), Info.RoomsInfo.Num(), 0);	

	return true;
}

bool FGeneratorCommandsDungeonOneRoomInput::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	TestEqual(TEXT("Grid Generation is Valid"), Info.State, EDungeonInfoState::VALID);
	TestEqual(TEXT("This Grid with Rooms Count value == 1 should return a Grid with Size 1x2"), Info.GridSize, { 1,2,0 });
	TestEqual(TEXT("Every Grid with a Rooms Count greater than 0 will have an Extra Room (A Starting Room)"), Info.RoomsInfo.Num(), 2);	

	return true;
}

bool FGeneratorCommandsDungeonResetDungeonData::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

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

bool FGeneratorCommandsDungeonSaveDungeonEmptyNoValid::RunTest(const FString& Parameters)
{
	/*TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

	Generator.BuildDungeon(0);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	UDungeonData* TmpDungeonData = FDungeonUtilities::Get()->SaveDungeonData(Info);

	TestNull(TEXT("Save Command should return Null Object with no valid DungeonInfo"), TmpDungeonData);
	*/
	return true;
}

bool FGeneratorCommandsDungeonSaveDungeonValid::RunTest(const FString& Parameters)
{
	/*TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	UDungeonData* TmpDungeonData = FDungeonUtilities::Get()->SaveDungeonData(Info);

	TestNotNull(TEXT("Save Command must work with valid DungeonInfo"), TmpDungeonData);
	*/
	return true;
}

bool FGeneratorCommandsDoorsTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

	Generator.BuildDungeon(1);
	Generator.BuildRooms();
	Generator.BuildDoors();
	FDungeonInfo Info = Generator.GetDungeonInfo();

	FDoorInfo FirstRoomDoor = Info.RoomsInfo[0].DoorsInfo[0];
	FDoorInfo SecondRoomDoor = Info.RoomsInfo[1].DoorsInfo[0];

	TestEqual(TEXT("First Door of first Room must point to North Direction"), FirstRoomDoor.Direction, EWorldDirection::NORTH);
	TestEqual(TEXT("First Door of first Room has its room as Source"), FirstRoomDoor.SourceRoomIndex, 0);
	TestEqual(TEXT("First Door of first Room must point to second Room"), FirstRoomDoor.NextRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must point to South Direction"), SecondRoomDoor.Direction, EWorldDirection::SOUTH);
	TestEqual(TEXT("First Door of second Room has its room as Source"), SecondRoomDoor.SourceRoomIndex, 1);
	TestEqual(TEXT("First Door of second Room must point to South Direction"), SecondRoomDoor.NextRoomIndex, 0);

	return true;
}

bool FGeneratorCommandsDoorsMoreRoomsFirstTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
	TSharedPtr<FMockGeneratorMethod> Method = MakeShared<FMockGeneratorMethod>();
	FDungeonDataGenerator Generator(Method.Get());

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