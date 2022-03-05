// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonData.h"
#include "RoomData.h"

void FDungeonInfo::Reset()
{
	Grid = {};
	GridSize = {0,0,0};
	GridScheme.Empty();
	RoomsInfo.Empty();
}


void UDungeonData::Reset()
{
	GridSize = {0,0,0};
	PathLength = 0;
	GridScheme.Empty();
		
	RoomsPresetID.Empty();
	RoomsCoordinate.Empty();
	RoomsGridIndex.Empty();
		
	DoorsSourceRoomIndex.Empty();
	DoorsNextRoomIndex.Empty();
	DoorsDirection.Empty();
}