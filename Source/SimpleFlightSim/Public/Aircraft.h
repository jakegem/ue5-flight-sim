// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "JSBSimMovementComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"
 #include "GameFramework/SpringArmComponent.h"
 #include "Components/SkeletalMeshComponent.h"
// 
// ----- *.generated.h must be the last #include ----- //
#include "Aircraft.generated.h"

UCLASS()
class SIMPLEFLIGHTSIM_API AAircraft : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAircraft();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Root component
	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Skeletal mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;

	// Spring arm for controlling camera boom length and angle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArcGISCameraComponent* OrbitCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArcGISCameraComponent* CockpitCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MouseUpDown(float Value);
	void MouseLeftRight(float Value);
	void SwitchCamera();

private:
	float RadialSpeed;
	float MinRadius;
	float MaxRadius;
	float RotationSpeed;
	float PrevDeltaYaw;
	float PrevDeltaPitch;
	float MaxRotationDelta;
	float LocalPitch;
	float LocalYaw;
	bool OrbitCameraEnabled;
};
