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


/**
 * IGeneratorMethod interface
 */
class DUNGEONGENERATOR_API IGeneratorMethod
{
public:

	virtual struct FGrid Generate(const FGrid EmptyGridSample, struct FGridMakerInfo& MakerInfo) = 0;

	const static int32 GetOppositeIndex(const int32 Source);
	const static int32 GetPatternFromIndex(const int32 Index);
};

class DUNGEONGENERATOR_API FStandardGeneratorMethod : public IGeneratorMethod
{
	public:

	//FStandardGeneratorMethod() = delete;
	//
	//FStandardGeneratorMethod(const FIntVector InGridSize) :
	//	GridSize{InGridSize} 
	//{
	//
	//}

	virtual FGrid Generate(const FGrid EmptyGridSample, FGridMakerInfo& MakerInfo) override;

	//FORCEINLINE void GetMinMaxCoordinates(const FGrid& Grid, FIntVector& OutMin, FIntVector& OutMax) const
	//{
	//	OutMin = ;
	//	OutMax = ;
	//}
};


const static TArray<FIntVector> Directions
{
	{ 00, 01, 0 },
	{ -1, 00, 0 },
	{ 00, -1, 0 },
	{ 01, 00, 0 }
};


struct DUNGEONGENERATOR_API FGridCell
{
	FIntVector Coordinate = {0, 0, 0};
	int32 Index = -1;	
	int32 PathOrder = 0; // Starts with 1
	int32 CellPattern = 0; // 0 stands for an empty Cell
	
};

struct DUNGEONGENERATOR_API FGrid
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

	FGridCell& GetCell(const int32 X, const int32 Y)
	{
		const int32 Index = X + (Y * Size.X);
		return Cells[Index];
	}

	TArray<int32> GetScheme();
	TArray<int32> GetSchemePath();

	void CheckMinMaxCoordinate(const FIntVector Coordinate);
};


struct DUNGEONGENERATOR_API FGridMakerInfo
{
	public:

	FGridMakerInfo() = default;
	FGridMakerInfo(const int32 Rooms) :
		GridRadius{ Rooms + 1 }, RoomsCount{ Rooms }
	{
		const int32 Side = (GridRadius * 2) + 1;
		Size.Y = Size.X = Side;
		Size.Z = Side * Side;
	}

	//void CheckMinMaxCoordinate(const FIntVector Coordinate);

	

	//private:

	int32 GridRadius{ 0 };
	int32 RoomsCount{ 0 };
	FIntVector Size{ 0,0,0 }; // Z Value stores the Length of the Grid Array.

	//FIntVector StartPoint, EndPoint;
	FIntVector MinPoint{ 999,999,0 }, MaxPoint{ 0,0,0 };
};


/**
* Class that creates the base Scheme for our new dungeon.
* Each Grid is squared and with Odd length sides.
*/
class DUNGEONGENERATOR_API FDungeonGridMaker
{
public:
	FDungeonGridMaker();
	FDungeonGridMaker(const int32 Rooms, IGeneratorMethod* InMethod) :
		MakerInfo{FGridMakerInfo(Rooms)}, Method{InMethod}
	{
		if (Rooms <= 0)
		{
			return;
		}

		//const int32 Side = (GridRadius * 2) + 1;
		//MakerInfo.Size.Y = MakerInfo.Size.X = Side;
		//MakerInfo.Size.Z = Side * Side;
		//Size.Y = Size.X = Side;
		//Size.Z = Side * Side;
		
		//InGrid = MakeNewGrid(Size);
		FGrid EmptyGrid(MakerInfo.Size);
		InGrid = Method->Generate(EmptyGrid, MakerInfo);

		OutGrid = CropGrid(InGrid);
		LinkGridCells(OutGrid);
	}
	~FDungeonGridMaker();

	FGrid GetGrid();
	void DebugGrid(const FGrid FullGrid);
	
	//const static int32 GetOppositeIndex(const int32 Source);
	//const static int32 GetPatternFromIndex(const int32 Index);
	
	TArray<TPair<int32, int32>> GetDirectionPairs(FGrid& Grid);
	
private:
	//void CheckMinMaxCoordinate(const FIntVector Coordinate);
	//FGrid MakeNewGrid(const FIntVector GridSize);
	FGrid CropGrid(const FGrid& SourceGrid);
	void LinkGridCells(FGrid& Grid);	
	
	IGeneratorMethod* Method;
	FGridMakerInfo MakerInfo{};

	//int32 GridRadius{0};
	//int32 RoomsCount{0};	
	//FIntVector Size; // Z Value stores the Length of the Grid Array.
	
	FGrid InGrid = {};
	FGrid OutGrid = {};
	//FIntVector StartPoint, EndPoint;
	FIntVector MinPoint{999,999,0}, MaxPoint{0,0,0};	
	
};
