// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Grabber UE_LOG"));

	//Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get player viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	//UE_LOG(LogTemp, Warning, TEXT("LOCATION: %s, POSITION: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString())

	//Draw red trace
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector()*Reach;
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255,0,0),
		false,
		0.f,
		0.f,
		2.f
	);

	//Query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	// Ray-cast (Line-trace), we use byObjectType since we need to trace objects with PhysicsBody on Collision->Collision Preset->Object Type only
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),			//Contruct the query parameters based on collision channel
		TraceParameters
	);

	AActor* ActorIt = Hit.GetActor();
	if (ActorIt) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorIt->GetName()))
	}

	//See what hit
}

