// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AsylumProject/VRCharacter/VRCharacterBase.h"
#include "APGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ASYLUMPROJECT_API AAPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AAPGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Broadcats when level change is attempted */
	DECLARE_EVENT(AAPGameModeBase, FChangeLevelEvent)
	FChangeLevelEvent& OnChangeLevel() { return ChangeLevel; }

	/*
	* Attempts to open the level, first calling all level change dynamics
	* @param	LevelName	--	Name of the level to open
	* @param	Options	--	Options string
	*/
	void OpenLevel(FString InLevelName, FString InOptions=((FString)(L"")));

	/*
	* Attempts to open the level, first calling all level change dynamics
	* @param	LevelName	--	Name of the level to open
	* @param	Options	--	Options string
	*/
	void OpenLevel(FName InLevelName, FString InOptions = ((FString)(L"")));

	/*
	* Called when a LevelChangeDelegate is completed, unbinding it
	* If there are no bound delegates, it opens the level
	* @param	InHandle	--	The handle for the delegate calling this function
	*/
	void OpenLevelDelegateComplete(FDelegateHandle InHandle);
	
	/*
	* Template function to bind a LevelChangeDelegate
	* @param	InObject	--	Object that owns the function being bound
	* @param	OnCompleteDelegate	--	Function to be bound, Signature: void Func()
	*	**NOTE	The OnCompleteDelegate Mush call AAPGameMode::OpenLevelDelegateComplete 
	*/
	template<typename Object>
	static bool BindOpenLevelDelegate(Object* InObject, void(Object::* OnCompleteDelegate)(), FDelegateHandle& InHandle);

	/** 
	* Gets a refernce to the active player
	* 
	* @return	AVRCharcterBase ref
	*/
	AVRCharacterBase* GetPlayer() { return Player; }

	/**
	* Returns the player charcter cast as the specified type
	* 
	* @return	<class>* refernce
	*/
	template<class T>
	T* GetPlayer() { return Cast<T>(Player); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AVRCharacterBase* Player = nullptr;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;

private:
	/** Broadcats when level change is attempted */
	FChangeLevelEvent ChangeLevel;

	FTimerHandle FadeDelay;

	/**This function fades the camera out and exits the level once fadded out */
	void FadeOutChangeLevel();

	/**This delegate is used by FadeOutChangeLevel to change the level once fade out is complete*/
	void OpenLevelDelegate();

	UPROPERTY(EditDefaultsOnly)
	float CameraFadeTime = 2.f;

	FName LevelName;
	FString OptionsString;
	bool bChangingLevel = false;

};

template<typename Object>
inline bool AAPGameModeBase::BindOpenLevelDelegate(Object* InObject, void(Object::* OnCompleteDelegate)(), FDelegateHandle& InHandle)
{
	AAPGameModeBase* APGameMode = Cast<AAPGameModeBase>(UGameplayStatics::GetGameMode(InObject->GetWorld()));
	if (!APGameMode->IsValidLowLevel()) return false;
	InHandle = APGameMode->OnChangeLevel().AddUObject(InObject, OnCompleteDelegate);
	return true;
}
