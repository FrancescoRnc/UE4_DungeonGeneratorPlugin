// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratorMethod.h"
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
