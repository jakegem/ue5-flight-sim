// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Math/Rotator.h"


// Sets default values
AAircraft::AAircraft() :
    RadialSpeed(5000.0),
    MinRadius(500.0),
    MaxRadius(30000.0),
    RotationSpeed(60.0),
    MaxRotationDelta(8.0),
    PrevDeltaYaw(0.0),
    PrevDeltaPitch(0.0),
    LocalPitch(0.0),
    LocalYaw(90.0),
    OrbitCameraEnabled(true)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Initialize the components and attach them to the RootComponent or relevant parent component
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SetRootComponent(Root);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(Root);

    // Assuming you have a Skeletal Mesh asset at the given path.
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> AircraftSkeletalMesh(TEXT("/Game/CommercialPlane/Meshes/SK_CommercialPlane.SK_CommercialPlane"));

    if (AircraftSkeletalMesh.Succeeded())
    {
        SkeletalMesh->SetSkeletalMesh(AircraftSkeletalMesh.Object);
    }

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(Root);
    SpringArm->TargetArmLength = 3000.0f;
    SpringArm->SetRelativeRotation(FRotator(-20.0f, 90.0f, 0.0f));
    SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 410.0f));
    // Enable camera lag for a smoother effect
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 3.0f;
    // Set the mobility of the spring arm and camera
    SpringArm->Mobility = EComponentMobility::Movable;
    
    OrbitCamera = CreateDefaultSubobject<UArcGISCameraComponent>(TEXT("OrbitCamera"));
    OrbitCamera->SetupAttachment(SpringArm);
    OrbitCamera->Mobility = EComponentMobility::Movable;

    CockpitCamera = CreateDefaultSubobject<UArcGISCameraComponent>(TEXT("CockpitCamera"));
    CockpitCamera->SetupAttachment(Root);
    CockpitCamera->FieldOfView = 90.0f;
    CockpitCamera->SetAspectRatio(16.0f / 9.0f);
    CockpitCamera->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    CockpitCamera->SetRelativeLocation(FVector(0.0f, 3160.0f, 650.0f));
}

// Called when the game starts or when spawned
void AAircraft::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAircraft::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MouseUpDown", this, &AAircraft::MouseUpDown);
    PlayerInputComponent->BindAxis("MouseLeftRight", this, &AAircraft::MouseLeftRight);
    PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AAircraft::SwitchCamera);
}

void AAircraft::MouseUpDown(float Value)
{
    if (Value != 0.0 && this->OrbitCameraEnabled) {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        double dt = UGameplayStatics::GetWorldDeltaSeconds(this);

        if (PlayerController)
        {
            if (PlayerController->IsInputKeyDown(EKeys::MiddleMouseButton)) {
                float val = - Value * this->RadialSpeed * dt + this->SpringArm->TargetArmLength;
                if (val > this->MaxRadius) {
                    val = MaxRadius;
                }
                else if (val < this->MinRadius) {
                    val = MinRadius;
                }
                this->SpringArm->TargetArmLength = val;
            }
            if (PlayerController->IsInputKeyDown(EKeys::RightMouseButton)) {
                float delta = Value * this->RotationSpeed * dt;
                if (delta > this->MaxRotationDelta) {
                    delta = this->MaxRotationDelta;
                }
                else if (delta < -this->MaxRotationDelta) {
                    delta = -this->MaxRotationDelta;
                }
                if (fabs(delta) > fabs(this->PrevDeltaPitch)) {
                    if (delta - this->PrevDeltaPitch > dt * this->MaxRotationDelta) {
                        delta = this->PrevDeltaPitch + dt * this->MaxRotationDelta;
                    }
                    else if (delta - this->PrevDeltaPitch < dt * this->MaxRotationDelta) {
                        delta = this->PrevDeltaPitch - dt * this->MaxRotationDelta;
                    }
                }
                this->LocalPitch -= delta;
                this->PrevDeltaPitch = delta;
                this->SpringArm->SetRelativeRotation(FRotator(this->LocalPitch, this->LocalYaw, 0.0));
            }
        }
    }
}

void AAircraft::MouseLeftRight(float Value)
{
    if (Value != 0.0 && this->OrbitCameraEnabled) {
        // Getting the Player Controller
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        double dt = UGameplayStatics::GetWorldDeltaSeconds(this);
        if (PlayerController)
        {
            if (PlayerController->IsInputKeyDown(EKeys::RightMouseButton)) {
                float delta = Value * this->RotationSpeed * dt;
                if (delta > this->MaxRotationDelta) {
                    delta = this->MaxRotationDelta;
                }
                else if (delta < - this->MaxRotationDelta) {
                    delta = -this->MaxRotationDelta;
                }
                if (fabs(delta) > fabs(this->PrevDeltaYaw)) {
                    if (delta - this->PrevDeltaYaw > dt * this->MaxRotationDelta) {
                        delta = this->PrevDeltaYaw + dt * this->MaxRotationDelta;
                    }
                    else if (delta - this->PrevDeltaYaw < dt * this->MaxRotationDelta) {
                        delta = this->PrevDeltaYaw - dt * this->MaxRotationDelta;
                    }
                }
                this->LocalYaw -= delta;
                this->PrevDeltaYaw = delta;
                this->SpringArm->SetRelativeRotation(FRotator(this->LocalPitch, this->LocalYaw, 0.0));
            }
        }
    }
}

void AAircraft::SwitchCamera()
{
    //if (GEngine)
    //    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Pressed!")));
    this->OrbitCameraEnabled = !this->OrbitCameraEnabled;
    this->CockpitCamera->SetActive(!this->OrbitCameraEnabled);
    this->OrbitCamera->SetActive(this->OrbitCameraEnabled);
}

