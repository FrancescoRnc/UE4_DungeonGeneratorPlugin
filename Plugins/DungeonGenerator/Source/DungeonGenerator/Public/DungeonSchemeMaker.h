// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

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


/*struct FSchemeCell
{
    FIntVector Coordinate = {0, 0, 0};
    // 0: OK; 1: Visited; 2: Start; 4: Goal; 8: Unused;
    int32 CellStatus = 0;
    int32 CellType = 0;	
    int32 Index = -1;
};

class DUNGEONGENERATOR_API FDungeonSchemeMaker
{	
public:
    FDungeonSchemeMaker();
    FDungeonSchemeMaker(const int32 Width, const int32 Height) :
        SchemeSize{FIntVector(Width, Height, 0)}
    {
  		SchemeLength = SchemeSize.X * SchemeSize.Y;
  		PrototypeScheme.Init({}, SchemeLength);
  		for (int32 Index = 0; Index < SchemeLength; Index++)
  		{
  		 	PrototypeScheme[Index].Index = Index;	
  		 	PrototypeScheme[Index].Coordinate.X = Index % SchemeSize.X;
  		 	PrototypeScheme[Index].Coordinate.Y = Index / SchemeSize.X;			
  		}
    }	
	~FDungeonSchemeMaker();

	void MakeNewScheme();
	const TArray<int32> GetScheme() const;

private:
	bool DeepFirstSearch(FSchemeCell& InCell);
	
	FIntVector SchemeSize;
	int32 SchemeLength;
	TArray<int32> OutScheme;
	TArray<FSchemeCell> CurrentScheme;
	TArray<FSchemeCell> PrototypeScheme;
	TArray<FSchemeCell> SchemeStack;
};*/


const static TArray<FIntVector> Directions
{
	{ 00, 01, 0 },
	{ -1, 00, 0 },
	{ 00, -1, 0 },
	{ 01, 00, 0 }
};


struct FGridCell
{
	FIntVector Coordinate = {0, 0, 0};
	int32 Index = -1;	
	int32 PathOrder = 0; // Starts with 1
	int32 CellPattern = 0; // 0 stands for an empty Cell
	
};

struct FGrid
{
	FGrid();
	FGrid(FIntVector NewSize) :
		Size{ NewSize }, Length{ NewSize.X * NewSize.Y }
	{
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
	FIntVector StartPoint, EndPoint;
	FIntVector Size = { 0, 0, 0 };
	int32 Length = 0;
	int32 PathLength = 0;

	FORCEINLINE FGridCell& operator[] (const int32 Index)
	{
		return Cells[Index];
	}

	FGridCell& GetCell(const int32 X, const int32 Y)
	{
		const int32 Index = X + (Y * Size.X);
		return Cells[Index];
	}

	TArray<int32> GetScheme();
	TArray<int32> GetSchemePath();
};

/**
* Class that creates the base Scheme for our new dungeon.
* Each Grid is squared and with Odd length sides.
*/
class DUNGEONGENERATOR_API FDungeonGridMaker
{
public:
	FDungeonGridMaker();
	FDungeonGridMaker(const int32 Rooms) :
		GridRadius{Rooms + 1}, RoomsCount{Rooms}
	{
		if (Rooms <= 0)
		{
			return;
		}

		const int32 Side = (GridRadius * 2) + 1;
		Size.Y = Size.X = Side;
		Size.Z = Side * Side;
		
		InGrid = MakeNewGrid(Size);
		OutGrid = CropGrid(InGrid);
		LinkGridCells(OutGrid);
	}
	~FDungeonGridMaker();

	FGrid GetGrid();
	void DebugGrid(const FGrid FullGrid);
	
	const static int32 GetOppositeIndex(const int32 Source);
	const static int32 GetPatternFromIndex(const int32 Index);

	
	TArray<TPair<int32, int32>> GetDirectionPairs(FGrid& Grid);
	
private:
	void CheckMinMaxCoordinate(const FIntVector Coordinate);
	//FGrid MakeNewGrid2(const FIntVector GridSize);
	FGrid MakeNewGrid(const FIntVector GridSize);
	FGrid CropGrid(const FGrid& SourceGrid);
	void LinkGridCells(FGrid& Grid);	
	
	
	int32 GridRadius{0};
	int32 RoomsCount{0};	
	FIntVector Size; // Z Value stores the Length of the Grid Array.
	
	FGrid InGrid = {};
	FGrid OutGrid = {};
	//FIntVector StartPoint, EndPoint;
	FIntVector MinPoint{99,99,0}, MaxPoint{0,0,0};	
	
};
