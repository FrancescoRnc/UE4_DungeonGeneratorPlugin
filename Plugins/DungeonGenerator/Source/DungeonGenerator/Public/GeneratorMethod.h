// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

/**
* This is the Default Method given by this Plugin.
* This generates a Dungeon with Rooms = (RoomsCount + 1), where the extra Room is the Starting Room.
*/
class DUNGEONGENERATOR_API FDefaultGeneratorMethod : public IGeneratorMethod
{
	public:
	virtual FGrid Generate(const FGrid EmptyGridSample, FGridMakerInfo& MakerInfo) override;
};
