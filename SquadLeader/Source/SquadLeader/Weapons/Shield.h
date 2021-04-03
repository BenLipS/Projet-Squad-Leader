#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shield.generated.h"

UCLASS()
class SQUADLEADER_API AShield : public AActor
{
	GENERATED_BODY()
	
public:	
	AShield();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	// How much damage the shield can absorb
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Stats")
	float Health;

	void DestroyShield();

public:
	void SetHealth(const float _Health);
	void OnReceiveDamage(const float _Damage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOnReceiveDamage(const float _Damage);
	void ServerOnReceiveDamage_Implementation(const float _Damage);
	bool ServerOnReceiveDamage_Validate(const float _Damage);
};
