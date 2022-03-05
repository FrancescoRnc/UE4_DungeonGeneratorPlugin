// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DungeonBuilderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDungeonBuilderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUNGEONGENERATOR_API IDungeonBuilderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	* This step builds the fundamentals of the Target Dungeon.
	* @param sizeX The width of the Dungeon;
	* @param sizeY The height of the Dungeon;
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuildDungeon();

	/**
	* This step builds all the Rooms to form, your Dungeon.
	* @param _assets The Asset that give form to the Rooms;
	* @param inGrid The Array of values for creating Rooms;
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuildRooms();

	/**
	* This step builds all the Doors that link your Rooms;
	* @param _asset The Asset that give form to the Doors;
	* @param _inGrid The Array of Values reference from the Grid;
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BuildDoors();
};
