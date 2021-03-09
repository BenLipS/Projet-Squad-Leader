#include "SoldierAI.h"
#include "../../AI/AIGeneralController.h"

ASoldierAI::ASoldierAI(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

void ASoldierAI::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeTagChangeCallbacks();
	InitializeAttributeChangeCallbacks();
	InitializeAttributes();
	InitializeAbilities();
	AddStartupEffects();
	initWeapons();
}

FVector ASoldierAI::lookingAtPosition()
{
	return LookingAtPosition;
}

void ASoldierAI::SetLookingAtPosition(const FVector &_LookingAtPosition)
{
	LookingAtPosition = _LookingAtPosition;
	SyncControlRotation = FVector{ LookingAtPosition - GetActorLocation() }.Rotation();

	if (HasAuthority())
		MulticastSyncControlRotation(SyncControlRotation);
	else
		ServerSyncControlRotation(SyncControlRotation);
};

bool ASoldierAI::ActivateAbilityFire()
{
	return ActivateAbility(ASoldier::SkillFireWeaponTag);
}

void ASoldierAI::CancelAbilityFire()
{
	CancelAbility(ASoldier::SkillFireWeaponTag);
}

bool ASoldierAI::ActivateAbilityRun()
{
	return ActivateAbility(ASoldier::SkillRunTag);
}

void ASoldierAI::CancelAbilityRun()
{
	CancelAbility(ASoldier::SkillRunTag);
}

FVector ASoldierAI::GetRespawnPoint()
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		return AIController->GetRespawnPoint();
	}
	return FVector(0.f, 0.f, 1500.f); // else return default
}

void ASoldierAI::Die() {
	Super::Die();
	auto AIController = Cast<AAIGeneralController>(GetController());
	if(AIController)
		AIController->Die();
}