#include "SoldierAI.h"
//#include "../../AI/AIGeneralController.h"
#include "../../AI/AISquadController.h"
#include "BehaviorTree/BlackboardComponent.h"

ASoldierAI::ASoldierAI(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemSoldier>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAttributeSetSoldier>(TEXT("Attribute Set"));
}

void ASoldierAI::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierAI, bUpdateTeamOnSpawn);
}

// TODO: See with AI team how to proceed
void ASoldierAI::LockControls()
{
	if (AAIGeneralController* AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		AIController->IsActivated = false;
		AIController->StopMovement();
	}
}

void ASoldierAI::UnLockControls()
{
	if (AAIGeneralController* AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		AIController->IsActivated = true;
	}
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

	if (bUpdateTeamOnSpawn && GetTeam())
		GetTeam()->AddSoldierList(this);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeTagChangeCallbacks();
	InitializeAttributeChangeCallbacks();
	InitializeAttributes();
	InitializeAbilities();
	AddStartupEffects();
}

FVector ASoldierAI::GetLookingAtPosition(const float _MaxRange) const
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
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
}

void ASoldierAI::CancelAbilityFire()
{
	CancelAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.FireWeapon")));
}

bool ASoldierAI::ActivateAbilityRun()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Run")));
}

void ASoldierAI::CancelAbilityRun()
{
	CancelAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Run")));
}

bool ASoldierAI::ActivateAbilityLaunchGrenade()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Grenade.Assault")));
}

bool ASoldierAI::ActivateAbilityLaunchHeal()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AreaEffectFromSelf.Instant.Heal")));
}

bool ASoldierAI::ActivateAbilityLaunchShield()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.SpawnPhysicalShield")));
}

bool ASoldierAI::ActivateAbilityRegenShield()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.AreaEffectFromSelf.Temporary.MaxShield")));
}

bool ASoldierAI::ActivateAbilityLaunchMine()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Grenade.Heavy")));
}

bool ASoldierAI::ActivateAbilityOverHeat()
{
	return ActivateAbility(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.OverheatingWeapon")));
}

FVector ASoldierAI::GetRespawnPoint(AControlArea* _ControlArea)
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

void ASoldierAI::Respawn() {
	Super::Respawn();
	auto AIController = Cast<AAIGeneralController>(GetController());
	if (AIController)
		AIController->Respawn();
}

void ASoldierAI::OnReceiveDamage(const FVector& _ImpactPoint, const FVector& _SourcePoint)
{
	//Init GetHit Behaviour
	if (AAIGeneralController* AIController = Cast<AAIGeneralController>(GetController()); AIController && AIController->GetSeenEnemySoldier().Num() == 0) {
		AIController->StopCurrentBehavior = true;
		AIController->get_blackboard()->SetValueAsBool("IsHit", true);
		AIController->get_blackboard()->SetValueAsVector("IsHitEnemyLocation", _SourcePoint);
	}
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

void ASoldierAI::OnBlurredVisionFromJammer(const bool _IsBlurred)
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController) {
		AIController->StopCurrentBehavior = true;
		if (_IsBlurred) {
			StunCount++;
		}
		else { 
			StunCount--;
		}

		if (StunCount != 0) {
			LockControls();
			AIController->get_blackboard()->SetValueAsBool("IsStun", true);
			if (!StunFX) {
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				StunFX = GetWorld()->SpawnActor<AActor>(StunFXClass, GetLocation(), { 0,0,0 }, SpawnInfo);
				StunFX->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
		else {
			UnLockControls();
			AIController->get_blackboard()->SetValueAsBool("IsStun", false);
			if (StunFX) {
				StunFX->Destroy();
				StunFX = nullptr;
			}
		}
		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")))) {
			if (StunFX)StunFX->Destroy();
			AIController->get_blackboard()->SetValueAsBool("IsStun", false);
		}
	}
}

SoldierClass ASoldierAI::GetClass()
{
	if (auto AIController = Cast<AAIGeneralController>(GetController()); AIController)
	{
		return AIController->GetClass();
	}
	return SoldierClass::NONE;
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

void FSoldierAIData::OnStateChanged(AIBasicState newState)
{
	MissionState = newState;
	OnStateNotify.Broadcast(MissionState);
}

void FSoldierAIData::OnClassChanged(SoldierClass newState)
{
	ClassSoldier = newState;
	OnClassNotify.Broadcast(ClassSoldier);
}
