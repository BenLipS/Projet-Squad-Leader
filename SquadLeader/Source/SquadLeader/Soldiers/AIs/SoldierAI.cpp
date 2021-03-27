#include "SoldierAI.h"
#include "../../AI/AIGeneralController.h"
#include "../../AI/AISquadController.h"

ASoldierAI::ASoldierAI(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

// TODO: See with AI team how to proceed
void ASoldierAI::LockControls()
{
	//if (LastUnpossessedController = GetController(); LastUnpossessedController)
		//LastUnpossessedController->UnPossess();
	
	/*if (auto AC = Cast<AAIController>(GetController()); AC)
	{
		if (AC->BrainComponent)
			AC->BrainComponent->StopLogic("");
	}*/
}

void ASoldierAI::UnLockControls()
{
	/*if (LastUnpossessedController)
	{
		LastUnpossessedController->Possess(this);
		LastUnpossessedController = nullptr;
	}*/
	/*if (auto AC = Cast<AAIController>(GetController()); AC)
	{
		if (AC->BrainComponent)
			AC->BrainComponent->RestartLogic();
	}	*/
}

void ASoldierAI::BroadCastDatas()
{
	OnHealthChanged.Broadcast(AttributeSet->GetHealth(), GetController<AAISquadController>());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth(), GetController<AAISquadController>());
	OnShieldChanged.Broadcast(AttributeSet->GetShield(), GetController<AAISquadController>());
	OnMaxShieldChanged.Broadcast(AttributeSet->GetMaxShield(), GetController<AAISquadController>());
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
}

FVector ASoldierAI::GetLookingAtPosition()
{
	return LookingAtPosition;
}

ASoldierTeam* ASoldierAI::GetTeam()
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		return AIController->GetTeam();
	}
	return nullptr; // else return default
}

bool ASoldierAI::SetTeam(ASoldierTeam* _Team)
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

bool ASoldierAI::ActivateAbilityLaunchGrenade()
{
	return ActivateAbility(ASoldier::SkillGrenadeTag);
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
	// TODO: Review callbacks with soldiers
	Super::HealthChanged(Data);
	OnHealthChanged.Broadcast(Data.NewValue, GetController<AAISquadController>());
}

void ASoldierAI::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue, GetController<AAISquadController>());
}

void ASoldierAI::ShieldChanged(const FOnAttributeChangeData& Data)
{
	OnShieldChanged.Broadcast(Data.NewValue, GetController<AAISquadController>());
}

void ASoldierAI::MaxShieldChanged(const FOnAttributeChangeData& Data)
{
	OnMaxShieldChanged.Broadcast(Data.NewValue, GetController<AAISquadController>());
}

void FSoldierAIData::OnHealthChanged(float newHealth)
{
	Health = newHealth;
	OnHealthNotify.Broadcast(Health);
}

void FSoldierAIData::OnMaxHealthChanged(float newMaxHealth)
{
	MaxHealth = newMaxHealth;
	OnMaxHealthNotify.Broadcast(MaxHealth);
}

void FSoldierAIData::OnShieldChanged(float newShield)
{
	Shield = newShield;
	OnShieldNotify.Broadcast(Shield);
}

void FSoldierAIData::OnMaxShieldChanged(float newMaxShield)
{
	MaxShield = newMaxShield;
	OnMaxShieldNotify.Broadcast(MaxShield);
}
