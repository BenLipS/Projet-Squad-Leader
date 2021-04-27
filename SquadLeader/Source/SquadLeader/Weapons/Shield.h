#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "SquadLeader/Soldiers/SoldierTeam.h"
#include "SquadLeader/Soldiers/Interface/Teamable.h"
#include "Shield.generated.h"

UCLASS()
class SQUADLEADER_API AShield : public AActor, public ITeamable
{
	GENERATED_BODY()

public:
	AShield();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;

public:
	UStaticMeshComponent* GetMesh() const;

protected:
	// How much damage the shield can absorb
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Stats")
	float Health;

	void DestroyShield();

public:
	void SetHealth(const float _Health);
	void ApplyDamages(const float _Damage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyDamages(const float _Damage);
	void ServerApplyDamages_Implementation(const float _Damage);
	bool ServerApplyDamages_Validate(const float _Damage);

//////////////// Teamable
protected:
	UPROPERTY(replicated)
	ASoldierTeam* Team;

public:
	virtual ASoldierTeam* GetTeam() override;
	virtual bool SetTeam(ASoldierTeam* _Team) override;
};