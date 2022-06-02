// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GeneratorMethod.h"

/**
 * Here's a Legend for Direction values:
 * Combine via Bitwise operations to get Room Patterns.
 */
enum class ECellDirection : uint8
{
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3,
	MAX   = 4
};


const static TArray<FIntVector> Directions
{
	{ 00, 01, 0 },
	{ -1, 00, 0 },
	{ 00, -1, 0 },
	{ 01, 00, 0 }
};

// Struct that stores data about a physical Cell in a Grid.
struct DUNGEONGENERATOR_API FGridCell
{
	FIntVector Coordinate = {-1, -1, 0};
	int32 Index = -1;	
	int32 PathOrder = 0; // Starts with 1
	int32 CellPattern = -1; // 0 stands for an empty Cell
	
};

// Struct that represents an empty 1D Grid to be filled with usefull data for the generation of a Dungeon
struct DUNGEONGENERATOR_API FGrid
{
	FGrid();
	FGrid(FIntVector NewSize) :
		Size{ NewSize }, Length{ NewSize.X * NewSize.Y }
	{
		Size.Z = 0;
		Cells.Init({}, Length);
		for (int32 Index = 0; Index < Length; Index++)
		{
			Cells[Index].Index = Index;
			Cells[Index].Coordinate.X = Index % Size.X;
			Cells[Index].Coordinate.Y = Index / Size.X;
		}
	}
	~FGrid();

	// This field stores all the Cells composing the Dungeon 
	TArray<FGridCell> Cells{};
	// This field stores the path through the Dungeon from first to last
	TArray<FIntVector> PathTrack{};
	TArray<TPair<int32, int32>> LinkedPairs{};
	FIntVector StartPoint	{ 0,0,0 };
	FIntVector EndPoint		{ 0,0,0 };
	FIntVector Size			{ 0,0,0 };
	FIntVector MinPoint		{ 999,999,0 };
	FIntVector MaxPoint		{ 0, 0, 0 };
	int32 Length = 0;
	int32 PathLength = 0;

	FORCEINLINE FGridCell& operator[] (const int32 Index)
	{
		return Cells[Index];
	}

	FORCEINLINE FGridCell& GetCell(const int32 X, const int32 Y)
	{
		const int32 Index = X + (Y * Size.X);
		return Cells[Index];
	}

	TArray<int32> GetScheme();
	TArray<int32> GetSchemePath();

	void CheckMinMaxCoordinate(const FIntVector Coordinate);
};

// Struct that stores data about for a GridMaker Object.
struct DUNGEONGENERATOR_API FGridMakerInfo
{
	public:

	FGridMakerInfo() = default;
	FGridMakerInfo(const int32 RoomsCount) :
		GridRadius{ RoomsCount + 1 }, RoomsCount{ RoomsCount }
	{
		if (RoomsCount > 0)
		{
			const int32 Side = (GridRadius * 2) + 1;
			Size.Y = Size.X = Side;
			Size.Z = Side * Side;
		}		

		// Here's an example with straight paths in every direction:
		// 2 Rooms: Side = ((2+1) * 2) + 1 = 7
		// X is the very first room, not included in Rooms. here's why (Rooms + 1)
		// 0 0 0 0 0 0 0 
		// 0 0 0 2 0 0 0
		// 0 0 0 1 0 0 0
		// 0 2 1 X 1 2 0
		// 0 0 0 1 0 0 0
		// 0 0 0 2 0 0 0
		// 0 0 0 0 0 0 0
	}

	int32 GridRadius{ 0 };
	int32 RoomsCount{ 0 };
	FIntVector Size{ 0,0,0 }; // Z Value stores the Length of the Grid Array.
};


/**
* Class that creates the base Scheme for our new dungeon, and given a specific Method gives us the final result of our Dungeon.
*/
class DUNGEONGENERATOR_API FDungeonGridMaker
{
public:
	FDungeonGridMaker();
	FDungeonGridMaker(const int32 RoomsCount, IGeneratorMethod* InMethod) :
		Rooms{RoomsCount}, MakerInfo{FGridMakerInfo(RoomsCount)}, Method{InMethod}
	{

	}
	~FDungeonGridMaker();

	FORCEINLINE FGrid GetGrid()
	{
		return OutGrid;
	}

	FGrid GetFromMethod();
	FGrid Make();

	void DebugGrid(const FGrid FullGrid);
	
	
private:
	FGrid CropGrid(const FGrid& SourceGrid);
	void LinkGridCells(FGrid& Grid);	
	
	int32 Rooms = 0;
	FGridMakerInfo MakerInfo{};
	IGeneratorMethod* Method = nullptr; // This determines the result of our Dungeon.

	FGrid InGrid = {};
	FGrid OutGrid = {};	
	
};
