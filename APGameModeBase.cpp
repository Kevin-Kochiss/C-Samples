// Fill out your copyright notice in the Description page of Project Settings.


#include "APGameModeBase.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"


AAPGameModeBase::AAPGameModeBase() {

}

void AAPGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

// Called when the game starts or when spawned
void AAPGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UStereoLayerFunctionLibrary::HideSplashScreen();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController->IsValidLowLevel()) return;
	Player = PlayerController->GetPawn<AVRCharacterBase>();
	PlayerController->PlayerCameraManager->StartCameraFade(1, 0, CameraFadeTime, FLinearColor::Black, false);

	//Multicast Change levels
	//Plays a transition/loading screen until each call back is complete

}

void AAPGameModeBase::OpenLevel(FString InLevelName, FString InOptions)
{
	if (bChangingLevel) {
		return;
	}
	else {
		bChangingLevel = true;
	}
	LevelName = FName(InLevelName);
	OptionsString = InOptions;
	if (ChangeLevel.IsBound()) {
		ChangeLevel.Broadcast();
		
	}
	else {
		FadeOutChangeLevel();
	}
}

void AAPGameModeBase::FadeOutChangeLevel()
{
	//Fade out slight delay
	if (PlayerController->IsValidLowLevel()) { PlayerController->PlayerCameraManager->StartCameraFade(0, 1, CameraFadeTime, FLinearColor::Black, false, true); }
	GetWorldTimerManager().SetTimer(FadeDelay , this, &AAPGameModeBase::OpenLevelDelegate, CameraFadeTime, false);
	
}

void AAPGameModeBase::OpenLevelDelegate()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();
	UE_LOG(LogTemp, Warning, TEXT("FNAME %s"), *LevelName.ToString());
	UGameplayStatics::OpenLevel(this, LevelName, true);
}

void AAPGameModeBase::OpenLevel(FName InLevelName, FString InOptions)
{
	if (bChangingLevel) {
		return;
	}
	else {
		bChangingLevel = true;
	}
	LevelName = InLevelName;
	OptionsString = InOptions;
	if (ChangeLevel.IsBound()) {
		ChangeLevel.Broadcast();

	}
	else {
		FadeOutChangeLevel();
	}
}

void AAPGameModeBase::OpenLevelDelegateComplete(FDelegateHandle InHandle)
{
	ChangeLevel.Remove(InHandle);
	if (!ChangeLevel.IsBound()) {
		//Start Fade OUT
		if (PlayerController->IsValidLowLevel()) { PlayerController->PlayerCameraManager->StartCameraFade(0, 1, CameraFadeTime, FLinearColor::Black, false, true); }
		UStereoLayerFunctionLibrary::ShowSplashScreen();
		UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, OptionsString);
	}
}
