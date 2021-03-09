#include "SoldierAI.h"
#include "../../AI/AIGeneralController.h"

ASoldierAI::ASoldierAI(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

void ASoldierAI::BroadCastDatas()
{
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnShieldChanged.Broadcast(AttributeSet->GetShield());
	OnMaxShieldChanged.Broadcast(AttributeSet->GetMaxShield());
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


TSubclassOf<ASoldierTeam> ASoldierAI::GetTeam()
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		return AIController->GetTeam();
	}
	return nullptr; // else return default
}

bool ASoldierAI::SetTeam(TSubclassOf<ASoldierTeam> _Team)
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		return AIController->SetTeam(_Team);
	}
	return false; // else return default
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
		AIController->Die();}

void ASoldierAI::Respawn() {
	Super::Respawn();
	auto AIController = Cast<AAIGeneralController>(GetController());
	if (AIController)
		AIController->Respawn();
}

void ASoldierAI::InitializeAttributeChangeCallbacks()
{
	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASoldierAI::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ASoldierAI::MaxHealthChanged);
		ShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetShieldAttribute()).AddUObject(this, &ASoldierAI::ShieldChanged);
		MaxShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxShieldAttribute()).AddUObject(this, &ASoldierAI::MaxShieldChanged);
	}
}

void ASoldierAI::HealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void ASoldierAI::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void ASoldierAI::ShieldChanged(const FOnAttributeChangeData& Data)
{
	OnShieldChanged.Broadcast(Data.NewValue);
}

void ASoldierAI::MaxShieldChanged(const FOnAttributeChangeData& Data)
{
	OnMaxShieldChanged.Broadcast(Data.NewValue);
}