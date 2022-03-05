// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "Tests/AutomationEditorCommon.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorNoValidRoomsCount, "Generator.Generate.RoomsCount.ZeroNoValid", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorOneRoomInput, "Generator.Generate.RoomsCount.OneInputTwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorTwoDoorsLinkedCorrectly, "Generator.Generate.Door.TwoDoorrs.CorrectLinking.TwoRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneratorMoreRoomsFirstTwoDoorsLinkedCorrectly, "Generator.Generate.Door.TwoDoorrs.CorrectLinking.MoreRooms", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)



bool FGeneratorNoValidRoomsCount::RunTest(const FString& Parameters)
{
	//FDungeonGeneratorModule& GeneratorModule = FModuleManager::LoadModuleChecked<FDungeonGeneratorModule>("DungeonGenerator");
		
	{
		FDungeonGenerator Generator;

		Generator.BuildDungeon(0);
		//Generator.BuildRooms();
		//Generator.BuildDoors();
		FDungeonInfo Info = Generator.GetDungeonInfo();

		TestEqual(TEXT("Grid with Rooms Count equal to 0 should be empty"), Info.GridSize.X * Info.GridSize.Y, 0);
	}

	return true;
}

bool FGeneratorOneRoomInput::RunTest(const FString& Parameters)
{
	{
		FDungeonGenerator Generator;

		Generator.BuildDungeon(1);
		Generator.BuildRooms();
		Generator.BuildDoors();
		FDungeonInfo Info = Generator.GetDungeonInfo();

		TestEqual(TEXT("Every Grid with a Rooms Count greater than 0 will have an Extra Room (A Starting Room)"), Info.RoomsInfo.Num(), 2);
	}

	return true;
}

bool FGeneratorTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
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
	}

	return true;
}

bool FGeneratorMoreRoomsFirstTwoDoorsLinkedCorrectly::RunTest(const FString& Parameters)
{
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
	}

	return true;
}