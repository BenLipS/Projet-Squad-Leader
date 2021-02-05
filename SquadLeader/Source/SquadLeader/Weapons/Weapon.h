#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class SQUADLEADER_API AWeapon : public AActor
{
	GENERATED_BODY()

protected:
	AWeapon();

//////////////// Fire
protected:
	bool isNextFireReady;

	FTimerHandle timerReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	float timeToReloadNextShoot;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	bool isAutomatic;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = "Stats")
	int penetration;

public:
	virtual void tryFiring();
	virtual void fire();

	virtual void BeginPlay() override;
	virtual void onReadyToShoot();
};