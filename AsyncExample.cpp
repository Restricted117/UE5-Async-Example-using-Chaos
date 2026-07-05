// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncExample.h"


// Sets default values
AAsyncExample::AAsyncExample()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.TickGroup = TG_PrePhysics;
	asyncCallBack = nullptr;
	CachedWorld = nullptr;
}


// Called when the game starts or when spawned
void AAsyncExample::BeginPlay()
{
	Super::BeginPlay();

	CachedWorld = GetWorld();
	if (!CachedWorld.IsValid()) { return; }

	if (FPhysScene* PhysScene = CachedWorld->GetPhysicsScene())
	{
		if (!asyncCallBack)
		{
			asyncCallBack = PhysScene->GetSolver()->CreateAndRegisterSimCallbackObject_External<AsyncCallBack>();
			PhysScene->OnPhysScenePreTick.AddUObject(this, &AAsyncExample::GameTickCounter);
		}
	}
}


// Called every frame
void AAsyncExample::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	while (Chaos::TSimCallbackOutputHandle<AsyncCallBackOutput> out = asyncCallBack->PopOutputData_External())
	{
		const AsyncCallBackOutput* output = out.Get();

		FString message = FString::Printf(TEXT("game tick %i   phys tick: %i"), output->gameTick, output->asyncTick);
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Emerald, message);
	}
}


//deregister callback functions
void AAsyncExample::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!CachedWorld.IsValid()) return;

	if (FPhysScene* PhysScene = CachedWorld->GetPhysicsScene())
	{
		if (asyncCallBack)
		{
			PhysScene->GetSolver()->UnregisterAndFreeSimCallbackObject_External(asyncCallBack);
			asyncCallBack = nullptr;
		}

		PhysScene->OnPhysScenePreTick.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}


//function for preparing the input buffer before sending to async thread
void AAsyncExample::GameTickCounter(FPhysScene* PhysScene, float DeltaSeconds)
{
	AsyncCallBackInput* input = asyncCallBack->GetProducerInputData_External();
	++gameTick;

	input->gameTick = gameTick;
}


//function that prepares the output buffer
void AsyncCallBack::OnPreSimulate_Internal()
{
	const AsyncCallBackInput* input = GetConsumerInput_Internal();
	AsyncCallBackOutput& output = GetProducerOutputData_Internal();
	++asyncTick;

	output.asyncTick = asyncTick;
	output.gameTick = input->gameTick;
}

