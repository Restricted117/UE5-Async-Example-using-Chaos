// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PBDRigidsSolver.h"
#include "AsyncExample.generated.h"



//input buffer that is prepared by gamethread before sending to async thread
struct MYPROJECT2_API AsyncCallBackInput : public Chaos::FSimCallbackInput
{
	int gameTick;

	void Reset()
	{
		*this = AsyncCallBackInput();
	}
};


//output buffer that is prepared by async thread and sent to gamethread
struct MYPROJECT2_API AsyncCallBackOutput : public Chaos::FSimCallbackOutput
{
	int gameTick;
	int asyncTick;

	void Reset()
	{
		*this = AsyncCallBackOutput();
	}
};


//callback class for handling async calculations
class MYPROJECT2_API AsyncCallBack : public Chaos::TSimCallbackObject<AsyncCallBackInput, AsyncCallBackOutput>
{
	//called before physics integration
	virtual void OnPreSimulate_Internal() override;
	int asyncTick = 0;

	//if substepping enabled use this if you want it to be called every step instead of the first step
	//virtual void OnPreIntegrate_Internal() override;
};


UCLASS()
class MYPROJECT2_API AAsyncExample : public AActor
{
	GENERATED_BODY()


	AsyncCallBack* asyncCallBack;

	UPROPERTY()
	TWeakObjectPtr<UWorld> CachedWorld;

	//function that prepares the input before before sending to async thread
	void GameTickCounter(FPhysScene* PhysScene, float DeltaSeconds);

	int gameTick = 0;

public:	
	// Sets default values for this actor's properties
	AAsyncExample();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

