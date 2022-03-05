// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "DungeonRoom.h"
#include "GameFramework/Character.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	InteractionCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Collider"));

	DoorMesh->SetupAttachment(RootComponent);
	InteractionCollider->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
//void ADoor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}


void ADoor::Teleport(ACharacter* Character)
{
	const EWorldDirection OppositeDirection = EWorldDirection::SOUTH;
	const FVector NewLocation = LinkedRoom->GetCardinalLocation(OppositeDirection);
	Character->SetActorLocation(NewLocation);
}
