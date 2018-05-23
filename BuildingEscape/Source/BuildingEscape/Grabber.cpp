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
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();

	SetupInputComponent();
}

//Look for attached physics handle
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());
	}
}

//Look for the InputComponent
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	//If the physics handle is attached move the object that we're holding
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab() {
	//LINE TRACE and try reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent(); //Gets the mesh
	auto ActorHit = HitResult.GetActor();

	//if we hit something then attach a physics handle
	if (ActorHit){
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation()
		);
	}
}

void UGrabber::Release() {
	//Release physics
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach(){
	//Query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	// Ray-cast (Line-trace), we use byObjectType since we need to trace objects with PhysicsBody on Collision->Collision Preset->Object Type only
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),			//Contruct the query parameters based on collision channel
		TraceParameters
	);
	return HitResult;
}

FVector UGrabber::GetReachLineStart() {
	// Get player viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd() {
	// Get player viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector()*Reach;
}
