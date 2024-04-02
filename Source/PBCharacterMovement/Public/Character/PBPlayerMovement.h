// Copyright 2017-2019 Project Borealis

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Runtime/Launch/Resources/Version.h"

#include "PBPlayerMovement.generated.h"

#define LADDER_MOUNT_TIMEOUT 0.2f

// Crouch Timings (in seconds)
#define MOVEMENT_DEFAULT_CROUCHTIME 0.4f
#define MOVEMENT_DEFAULT_CROUCHJUMPTIME 0.0f
#define MOVEMENT_DEFAULT_UNCROUCHTIME 0.2f
#define MOVEMENT_DEFAULT_UNCROUCHJUMPTIME 0.8f

class USoundCue;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWaterJumpMode : uint8
{
	None = 0 UMETA(Hidden),
	Surface = 1 << 0,
	NearWall = 1 << 1,
};
ENUM_CLASS_FLAGS(EWaterJumpMode);

UCLASS()
class PBCHARACTERMOVEMENT_API UPBPlayerMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	/** If the player is using a ladder */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bOnLadder;

	/** Milliseconds between step sounds */
	float MoveSoundTime;

	/** If we are stepping left, else, right */
	bool StepSide;

	/** The multiplier for acceleration when on ground. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float GroundAccelerationMultiplier;

	/** The multiplier for acceleration when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float AirAccelerationMultiplier;
	
	/* The vector differential magnitude cap when in air. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling")
	float AirSpeedCap;

	/** Time to crouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchTime;

	/** Time to uncrouch on ground in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UncrouchTime;

	/** Time to crouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float CrouchJumpTime;

	/** Time to uncrouch in air in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
	float UncrouchJumpTime;

	/** the minimum step height from moving fast */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
	float MinStepHeight;

	/** Time (in millis) the player has to rejump without applying friction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta=(DisplayName="Rejump Window", ForceUnits="ms"))
	float BrakingWindow;

	/* Progress checked against the Braking Window, incremented in millis. */
	float BrakingWindowTimeElapsed;

	/** If the player has been on the ground past the Braking Window, start braking. */
	bool bBrakingWindowElapsed;

	/** Wait a frame before crouch speed. */
	bool bCrouchFrameTolerated = false;

	/** If in the crouching transition */
	bool bIsInCrouchTransition = false;

	/** The PB player character */
	class APBPlayerCharacter* PBCharacter;

	/** The target ground speed when running. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float RunSpeed;

	/** The target ground speed when sprinting.  */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SprintSpeed;

	/** The target ground speed when walking slowly. */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float WalkSpeed;

	/** Speed on a ladder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder")
	float LadderSpeed;

	/** The minimum speed to scale up from for slope movement  */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMin;

	/** The maximum speed to scale up to for slope movement */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SpeedMultMax;

	/** The maximum angle we can roll for camera adjust */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollAngle = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float RollSpeed = 0.0f;

	/** Speed of rolling the camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float BounceMultiplier = 0.0f;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float AxisSpeedLimit = 6667.5f;

	/** Threshold relating to speed ratio and friction which causes us to catch air */
	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideLimit = 0.5f;

	/** Fraction of uncrouch half-height to check for before doing starting an uncrouch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	float GroundUncrouchCheckFactor = 0.75f;

	bool bShouldPlayMoveSounds = true;

	/** The multiplier for acceleration when swimming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (DisplayAfter = "Buoyancy"))
	float FluidAccelerationMultiplier;

	/** The percentage of the body that needs to be submerged to be considered swimming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming")
	float ImmersionThreshold = 0.70f;

	/** Z velocity applied when pawn tries to jump in water */
	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ForceUnits = "cm/s", DisplayAfter = "OutofWaterZ"))
	float WaterJumpVelocity = 300;

	/** Can the character perform a water jump ? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Jumping / Falling", meta = (Bitmask, BitmaskEnum = "/Script/PBCharacterMovement.EWaterJumpMode", DisplayName = "Allowed Water Jumps"))
	uint8 WaterJumpMode = 0xFF;

	/** The speed target at which we want to be considered sliding if we crouch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding")
	float SlidingStartSpeed = 500.f;
	
	/** The speed target at which we want to be considered to stop sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding")
	float SlidingStopSpeed = 361.9f;

	/** The amount of speed we gain by powersliding. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding")
	float SlidingSpeedBoost = 200.f;

	/** The multiplier the floor friction when we're sliding. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding")
	float SlidingFrictionMultiplier = .25f;

	/** Deceleration when sliding and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding", meta = (ClampMin = "0", UIMin = "0"))
	float BrakingDecelerationSliding = 500.f;

	/** The time before the end of a powerslide that we must wait to start another. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding", meta = (Units = "Milliseconds"))
	float SlidingBoostCooldown = 1000.f;

	/** The min angle of a floor that will prevent us from stopping to slide */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding", meta = (Units = "Degrees", ClampMin = "0", ClampMax = "90", UIMin = "0", UIMax = "90"))
	float AutoSlidingFloorAngle = 15.f;

	/** Only allow powerslides if we're going forward. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sliding")
	bool bOnlyForwardPowerslides = true;

public:
	/** Print pos and vel (Source: cl_showpos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)")
	uint32 bShowPos : 1;

	UPBPlayerMovement();

	virtual void InitializeComponent() override;
	void OnRegister() override;

	// Overrides for Source-like movement
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	void PhysFalling(float deltaTime, int32 Iterations);
	bool ShouldLimitAirControl(float DeltaTime, const FVector& FallAcceleration) const override;
	FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;

	void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	void UpdateSurfaceFriction(bool bIsSliding = false);
	void UpdateCrouching(float DeltaTime, bool bOnlyUnCrouch = false);

	// Overrides for crouch transitions
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual void DoCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);
	virtual void DoUnCrouchResize(float TargetTime, float DeltaTime, bool bClientSimulation = false);
	
	bool MoveUpdatedComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = nullptr, ETeleportType Teleport = ETeleportType::None) override;

	// Jump overrides
	bool CanAttemptJump() const override;
	bool DoJump(bool bClientSimulation) override;

	void TwoWallAdjust(FVector& OutDelta, const FHitResult& Hit, const FVector& OldHitNormal) const override;
	float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact = false) override;
	FVector ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	FVector HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	bool ShouldCatchAir(const FFindFloorResult& OldFloor, const FFindFloorResult& NewFloor) override;
	bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const override;
	bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	bool ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta, const FHitResult& Hit) const override;

	void TraceCharacterFloor(FHitResult& OutHit);

	// Acceleration
	FORCEINLINE FVector GetAcceleration() const
	{
		return Acceleration;
	}

	/** Is this player on a ladder? */
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool IsOnLadder() const
	{
		return bOnLadder;
	}

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);

	/** Do camera roll effect based on velocity */
	float GetCameraRoll() const;

	void SetNoClip(bool bNoClip);

	/** Toggle no clip */
	void ToggleNoClip();

	bool IsBrakingWindowTolerated() const
	{
		return bBrakingWindowElapsed;
	}

	virtual float GetMaxSpeed() const override;

	bool IsWaterJumpAllowed(const EWaterJumpMode& Mode) const
	{
		return (WaterJumpMode & ((uint8)Mode));
	}

	bool IsCrouchingOrGoingTo() const
	{
		return IsCrouching() || (bIsInCrouchTransition && bWantsToCrouch);
	}

protected:
	virtual void PhysicsVolumeChanged(class APhysicsVolume* NewVolume) override;
	virtual bool IsInWater() const override;
	virtual bool IsTouchingWater() const;
	virtual float GetCachedImmersionDepth() const;
	virtual float UpdateCachedImmersionDepth();
	virtual void EnterDeepWater();
	virtual void LeaveDeepWater();

	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;

	bool bIsPowerSliding = false;
	float PowerSlidingTimeElapsed = INFINITY;
	virtual void StartPowerSlide(bool IsBoostedSlide);
	virtual void EndPowerSlide();
	virtual bool MustStopPowerSlide() const;
	virtual bool CanPowerSlide() const;

private:
	/** Plays sound effect according to movement and surface */
	void PlayMoveSound(float DeltaTime);

	class UPBMoveStepSound* GetMoveStepSoundBySurface(EPhysicalSurface SurfaceType) const;


	virtual void PlayJumpSound(const FHitResult& Hit, bool bJumped);

	float DefaultStepHeight;
	float DefaultWalkableFloorZ;
	float SurfaceFriction;

	TOptional<float> CachedImmersionDepth;

	/** The time that the player can remount on the ladder */
	float OffLadderTicks = -1.0f;

	bool bHasDeferredMovementMode;
	EMovementMode DeferredMovementMode;
};
