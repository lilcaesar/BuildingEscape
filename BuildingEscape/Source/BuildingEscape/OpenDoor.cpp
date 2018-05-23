// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s missing Pressure PLate"), *GetOwner()->GetName());
	}
}

void UOpenDoor::OpenDoor()
{
	//Set the door rotator
	//Owner->SetActorRotation(FRotator(0.f, OpenaAngle, 0.f));
	OnOpenRequest.Broadcast();	//Send request to the blueprint event
}

void UOpenDoor::CloseDoor()
{
	//Set the door rotator
	Owner->SetActorRotation(FRotator(0.f, 0.f, 0.f));
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Poll the trigger volume volume
	if (GetTotalMassOfActorsOnPlate() > 50.f) { // TODO make into parameter
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	//Check if is time to close the door
	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorCloseDelay) {
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate() {
	float TotalMass = 0.f;

	//Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	//Iterate through all their masses
	for (const auto* Actor : OverlappingActors){
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s on pressure plate"), *Actor->GetName());
	}

	return TotalMass;
}
