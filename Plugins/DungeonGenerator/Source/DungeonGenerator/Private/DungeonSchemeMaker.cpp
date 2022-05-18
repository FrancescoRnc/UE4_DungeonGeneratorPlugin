// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonSchemeMaker.h"



const int32 IGeneratorMethod::GetOppositeIndex(const int32 Source)
{
	return (Source + 2) % 4;
}

const int32 IGeneratorMethod::GetPatternFromIndex(const int32 Index)
{
	return 1 << Index;
}

FGrid FStandardGeneratorMethod::Generate(const FGrid EmptyGridSample, FGridMakerInfo& MakerInfo)
{
	FGrid NewGrid = EmptyGridSample;
	TArray<FIntVector> PathTrack;
	TArray<int32> ExcludedDirections;

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
	ExcludedDirections = { 1, 3 };

	for (int32 Index = 0; Index < MakerInfo.RoomsCount; Index++)
	{
		const int32 OppositePatternIndex = GetOppositeIndex(NextPatternIndex);
		ExcludedDirections.Add(OppositePatternIndex);

		for (int32 PossibleDirection = 0;
			 PossibleDirection < 4;
			 PossibleDirection++)
		{
			if (!ExcludedDirections.Contains(PossibleDirection))
			{
				const FIntVector TmpCoords = NextCoordinate + Directions[PossibleDirection];
				if (PathTrack.Contains(TmpCoords))
				{
					ExcludedDirections.Add(PossibleDirection);
				}
			}
		}
		if (ExcludedDirections.Contains(0) &&
			ExcludedDirections.Contains(1) &&
			ExcludedDirections.Contains(2) &&
			ExcludedDirections.Contains(3))
		{
			break;
		}

		// Random direction excluding opposite direction and already existing Cell
		int32 NewPatternIndex = FMath::RandRange(0, 3);
		FIntVector NewCoordinate = NextCoordinate + Directions[NewPatternIndex];
		while (ExcludedDirections.Contains(NewPatternIndex))
		{
			NewPatternIndex = FMath::RandRange(0, 3);
			NewCoordinate = NextCoordinate + Directions[NewPatternIndex];
		}
		NextPatternIndex = NewPatternIndex;
		ExcludedDirections.Empty();

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


// Grid
FGrid::FGrid()
{
	
}

FGrid::~FGrid()
{
	Cells.Empty();
	PathTrack.Empty();
	Size = {0,0,0};
	Length = PathLength = 0;	
}


TArray<int32> FGrid::GetScheme()
{
	TArray<int32> Scheme;
	Scheme.Init(0, Length);
	for (int32 Index = 0; Index < Length; Index++)
	{
		Scheme[Index] = Cells[Index].CellPattern;
	}
	return Scheme;
}

TArray<int32> FGrid::GetSchemePath()
{
	TArray<int32> Scheme;
	Scheme.Init(0, Length);
	for (int32 Index = 0; Index < Length; Index++)
	{
		Scheme[Index] = Cells[Index].PathOrder;
	}
	return Scheme;
}

void FGrid::CheckMinMaxCoordinate(const FIntVector Coordinate)
{
	// MinPoint
	MinPoint.X = FMath::Min(MinPoint.X, Coordinate.X);
	MinPoint.Y = FMath::Min(MinPoint.Y, Coordinate.Y);

	// MaxPoint
	MaxPoint.X = FMath::Max(MaxPoint.X, Coordinate.X);
	MaxPoint.Y = FMath::Max(MaxPoint.Y, Coordinate.Y);
}


// Second Implementation
FDungeonGridMaker::FDungeonGridMaker()
{
	
}

FDungeonGridMaker::~FDungeonGridMaker()
{
	//PathTrack.Empty();
}

FGrid FDungeonGridMaker::GetGrid()
{
	return OutGrid;
}

/*void FDungeonGridMaker::CheckMinMaxCoordinate(const FIntVector Coordinate)
{
	// MinPoint
	MinPoint.X = FMath::Min(MinPoint.X, Coordinate.X);
	MinPoint.Y = FMath::Min(MinPoint.Y, Coordinate.Y);

	// MaxPoint
	MaxPoint.X = FMath::Max(MaxPoint.X, Coordinate.X);
	MaxPoint.Y = FMath::Max(MaxPoint.Y, Coordinate.Y);
}

const int32 FDungeonGridMaker::GetOppositeIndex(const int32 Source) 
{
	return (Source + 2) % 4;
}

const int32 FDungeonGridMaker::GetPatternFromIndex(const int32 Index)
{
	return 1 << Index;
}*/

TArray<TPair<int32, int32>> FDungeonGridMaker::GetDirectionPairs(FGrid& Grid)
{
	TArray<TPair<int32, int32>> Pairs;

	// CONTINUE THIS WAY
	
	InGrid.LinkedPairs = Pairs;
	return Pairs;
}

/*FGrid FDungeonGridMaker::MakeNewGrid(const FIntVector GridSize)
{
	FGrid NewGrid(GridSize);
	TArray<FIntVector> PathTrack;
	TArray<int32> ExcludedDirections;
	
	const FIntVector GridCenter = {GridRadius, GridRadius, 0};
	FIntVector NextCoordinate = GridCenter;
	int32 NextGridIndex = NextCoordinate.X + (NextCoordinate.Y * Size.X);
	int32 NextPatternIndex = 0;
	
	// First Cell setup
	NewGrid.StartPoint = GridCenter;
	PathTrack.Add(NewGrid.StartPoint);
	NewGrid[NextGridIndex].PathOrder = 1;
	CheckMinMaxCoordinate(GridCenter);

	// Now we are forcing the next Cell to go Up, by excluding Left, right and Down later
	ExcludedDirections = { 1, 3 };
	
	for (int32 Index = 0; Index < RoomsCount; Index++)
	{
		const int32 OppositePatternIndex = GetOppositeIndex(NextPatternIndex);
		ExcludedDirections.Add(OppositePatternIndex);

		for (int32 PossibleDirection = 0;
			 PossibleDirection < 4;
			 PossibleDirection++)
		{
			if (!ExcludedDirections.Contains(PossibleDirection))
			{
				const FIntVector TmpCoords = NextCoordinate + Directions[PossibleDirection];
				if (PathTrack.Contains(TmpCoords))
				{
					ExcludedDirections.Add(PossibleDirection);
				}
			}
		}
		if (ExcludedDirections.Contains(0) &&
			ExcludedDirections.Contains(1) &&
			ExcludedDirections.Contains(2) &&
			ExcludedDirections.Contains(3))
		{
			break;
		}
		
		// Random direction excluding opposite direction and already existing Cell
		int32 NewPatternIndex = FMath::RandRange(0, 3);
		FIntVector NewCoordinate = NextCoordinate + Directions[NewPatternIndex];
		while(ExcludedDirections.Contains(NewPatternIndex))
		{
			NewPatternIndex = FMath::RandRange(0, 3);
			NewCoordinate = NextCoordinate + Directions[NewPatternIndex];
		}
		NextPatternIndex = NewPatternIndex;
		ExcludedDirections.Empty();		
		
		NextCoordinate = NewCoordinate;
		CheckMinMaxCoordinate(NextCoordinate);
		NextGridIndex = NextCoordinate.X + (NextCoordinate.Y * Size.X);

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
}*/

void FDungeonGridMaker::LinkGridCells(FGrid& Grid)
{
	TArray<FGridCell> Cells;
	Cells.Init({}, Grid.PathLength);
	for (int32 Index = 0; Index < Grid.PathLength; Index++)
	{
		const FIntVector Coord = Grid.PathTrack[Index];
		Cells[Index] = Grid.GetCell(Coord.X, Coord.Y);
	}

	Cells[0].CellPattern = 1;
	Grid.GetCell(Cells[0].Coordinate.X, Cells[0].Coordinate.Y).CellPattern = 1;

	for (int32 Index = 1; Index < Grid.PathLength; Index++)
	{
		int32 NextDirectionPattern{0}, SourceDirection{0}, SourceIndex{0};        
		for (int32 Direction = 0; Direction < Directions.Num(); Direction++)
		{
			if (Index < Grid.PathLength - 1)
			{
				const FIntVector CheckForward = Grid.PathTrack[Index + 1] - Grid.PathTrack[Index];
                if (CheckForward.X == Directions[Direction].X &&
                	CheckForward.Y == Directions[Direction].Y)
                {
					NextDirectionPattern = 1 << Direction; //MakerInfo.GetPatternFromIndex(Direction);
                }
			}
			
			const FIntVector CheckBackward = Grid.PathTrack[Index - 1] - Grid.PathTrack[Index];
			if (CheckBackward.X == Directions[Direction].X &&
				CheckBackward.Y == Directions[Direction].Y)
			{
				SourceDirection = 1 << Direction; //GetPatternFromIndex(Direction);
				SourceIndex = Direction;
			}
		}
		Cells[Index].CellPattern = NextDirectionPattern | SourceDirection;
		const FIntVector& Coord = Cells[Index].Coordinate;
		const int32 GridIndex = Coord.X + (Coord.Y * Grid.Size.X);
		Grid[GridIndex].CellPattern = NextDirectionPattern | SourceDirection;

		// Linking
		//const int32 OppositeSourceDirection =
		//	GetPatternFromIndex(GetOppositeIndex(SourceIndex));
	}
	
}

FGrid FDungeonGridMaker::CropGrid(const FGrid& SourceGrid)
{	
	const FIntVector NewSize =
	{
		(SourceGrid.MaxPoint.X - SourceGrid.MinPoint.X) + 1,
		(SourceGrid.MaxPoint.Y - SourceGrid.MinPoint.Y) + 1,
		0
	};
	
	FGrid Crop(NewSize);
	Crop.PathTrack.Init({0,0,0}, SourceGrid.PathLength);
	Crop.PathLength = SourceGrid.PathLength;
	
	for(int32 Index = 0; Index < Crop.PathLength; Index++)
	{
		const FIntVector CropCoords = SourceGrid.PathTrack[Index] - SourceGrid.MinPoint;
		const int32 CropIndex = CropCoords.X + (CropCoords.Y * NewSize.X);
		const int32 GridIndex = SourceGrid.PathTrack[Index].X +
								(SourceGrid.PathTrack[Index].Y * MakerInfo.Size.X);
		Crop[CropIndex].PathOrder = SourceGrid.Cells[GridIndex].PathOrder;
		
		Crop.PathTrack[Index] = SourceGrid.PathTrack[Index] - SourceGrid.MinPoint;
	}

	Crop.StartPoint = Crop.PathTrack[0];
	Crop.EndPoint	= Crop.PathTrack[Crop.PathLength - 1];
	
	return Crop;
}

void FDungeonGridMaker::DebugGrid(const FGrid FullGrid)
{
	UE_LOG(LogTemp, Warning, TEXT("Path Length: %d"), FullGrid.PathTrack.Num());
	UE_LOG(LogTemp, Warning, TEXT("Path Order"));
	for (int32 Y = FullGrid.Size.Y - 1; Y >= 0; Y--)
	{
		FString Row("");
		for (int32 X = FullGrid.Size.X - 1; X >= 0; X--)
		{
			const int32 Index = X + (Y * FullGrid.Size.X);
			Row.Append(FullGrid.Cells[Index].PathOrder == 0 ?
				TEXT("[  ]") : FString::Printf(TEXT("[%02d]"), FullGrid.Cells[Index].PathOrder));
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Row);
	}
	UE_LOG(LogTemp, Warning, TEXT("+--------------------------------+"));
	UE_LOG(LogTemp, Warning, TEXT("Path Pattern"));
	for (int32 Y = FullGrid.Size.Y - 1; Y >= 0; Y--)
	{
		FString Row("");
		for (int32 X = FullGrid.Size.X - 1; X >= 0; X--)
		{
			const int32 Index = X + (Y * FullGrid.Size.X);
			Row.Append(FullGrid.Cells[Index].PathOrder == 0 ?
				TEXT("[  ]") : FString::Printf(TEXT("[%02d]"), FullGrid.Cells[Index].CellPattern));
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Row);
	}
}



//void FGridMakerInfo::CheckMinMaxCoordinate(const FIntVector Coordinate)
//{
//	// MinPoint
//	MinPoint.X = FMath::Min(MinPoint.X, Coordinate.X);
//	MinPoint.Y = FMath::Min(MinPoint.Y, Coordinate.Y);
//
//	// MaxPoint
//	MaxPoint.X = FMath::Max(MaxPoint.X, Coordinate.X);
//	MaxPoint.Y = FMath::Max(MaxPoint.Y, Coordinate.Y);
//}
