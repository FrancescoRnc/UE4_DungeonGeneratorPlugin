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

class DUNGEONGENERATOR_API FStandardGeneratorMethod : public IGeneratorMethod
{
	public:
	virtual FGrid Generate(const FGrid EmptyGridSample, FGridMakerInfo& MakerInfo) override;
};
