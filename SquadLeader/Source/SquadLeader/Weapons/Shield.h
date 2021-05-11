#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "SquadLeader/Soldiers/SoldierTeam.h"
#include "SquadLeader/Soldiers/Interface/Teamable.h"
#include "Math/Color.h"
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

//////////////// Design
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

public:
	UStaticMeshComponent* GetMesh() const;

	void UpdateMaterialColor();

protected:
	// Parameter name to set color material
	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Design")
	FName ColorParameterName = FName{"BgColor"};

	// Color of the shield for allies
	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Design")
	FLinearColor ColorMeshAllie = FLinearColor{ FVector4{0.f, 0.19f, 1.f, 1.f} };

	// Color of the shield for ennemies
	UPROPERTY(EditDefaultsOnly, Category = "Mesh|Design")
	FLinearColor ColorMeshEnnemy = FLinearColor{ FVector4{ 0.90625f, 0.02832f, 0.02832f, 0.f} };

//////////////// Collision
protected:
	// CollisionProfileName of the mesh
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Mesh|Collision")
	FName CollisionProfileNameMesh = FName{ "BlockAllDynamic" };

public:
	void SetCollisionProfile(const FName& _Name);
	FName GetCollisionProfile() const;

//////////////// Health
protected:
	// How much damage the shield can absorb
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Stats")
	float Health = 100.f;

	virtual void Destroyed() override;

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

protected:
	void CreateInfluence(); 
	void EraseInfluence();
};