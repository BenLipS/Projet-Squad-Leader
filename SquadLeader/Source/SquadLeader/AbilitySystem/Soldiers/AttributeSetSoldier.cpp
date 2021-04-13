#include "AttributeSetSoldier.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "../../Soldiers/Soldier.h"
#include "GameplayEffects/States/GE_StateFighting.h"
#include "GameplayEffects/GE_UpdateStats.h"
#include "SquadLeader/SquadLeaderGameModeBase.h"

UAttributeSetSoldier::UAttributeSetSoldier(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer),
CharacterLevel{ 1.f },
MaxLevel{ 10.f },
EXP{ 0.f },
MoveSpeedMultiplier{ 1.f }
{
}

void UAttributeSetSoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeSetSoldier, CharacterLevel);
	DOREPLIFETIME(UAttributeSetSoldier, EXP);
	DOREPLIFETIME(UAttributeSetSoldier, EXPLevelUp);
	DOREPLIFETIME(UAttributeSetSoldier, Health);
	DOREPLIFETIME(UAttributeSetSoldier, MaxHealth);
	DOREPLIFETIME(UAttributeSetSoldier, HealthRegenRate);
	DOREPLIFETIME(UAttributeSetSoldier, Shield);
	DOREPLIFETIME(UAttributeSetSoldier, MaxShield);
	DOREPLIFETIME(UAttributeSetSoldier, MoveSpeedWalk);
	DOREPLIFETIME(UAttributeSetSoldier, MoveSpeedCrouch);
	DOREPLIFETIME(UAttributeSetSoldier, MoveSpeedMultiplier);
}

bool UAttributeSetSoldier::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return true;
}

void UAttributeSetSoldier::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// We don't want stat changes if the soldier is dead
	if (GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"))))
		return;

	if (Attribute == GetMaxHealthAttribute())
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	else if (Attribute == GetMaxShieldAttribute())
		AdjustAttributeForMaxChange(Shield, MaxShield, NewValue, GetShieldAttribute());
}

void UAttributeSetSoldier::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* TargetASC = &Data.Target;

	if (!TargetASC->AbilityActorInfo.IsValid() || !TargetASC->AbilityActorInfo->AvatarActor.IsValid())
		return;

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = Context.GetOriginalInstigatorAbilitySystemComponent();

	// You must test if the pointers below are valid in the appropriate attributs
	ASoldier* SourceSoldier = Cast<ASoldier>(SourceASC->AbilityActorInfo->AvatarActor.Get());
	ASoldier* TargetSoldier = Cast<ASoldier>(TargetASC->AbilityActorInfo->AvatarActor.Get());

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (!TargetSoldier)
			return;

		// Make a local damage copy
		const float LocalDamage = GetDamage();
		SetDamage(0.f);

		if (LocalDamage > 0.0f && TargetSoldier->IsAlive())
		{
			if (LocalDamage <= GetShield())
				SetShield(GetShield() - LocalDamage);
			else
			{
				SetHealth(FMath::Max(GetHealth() - (LocalDamage - GetShield()), 0.0f));
				SetShield(0.f);
			}
			if (!TargetSoldier->IsAlive()) // The soldier has been killed
			{
				if (ASquadLeaderGameModeBase* GameMode = Cast<ASquadLeaderGameModeBase>(GetWorld()->GetAuthGameMode()); GameMode)
					GameMode->NotifySoldierKilled(TargetSoldier, SourceSoldier);
			}
			else if (SourceASC)
			{
				// Give fighting tag
				FGameplayEffectSpecHandle FightingTagHandle = SourceASC->MakeOutgoingSpec(UGE_StateFighting::StaticClass(), 1.f, Context);
				SourceASC->ApplyGameplayEffectSpecToTarget(*FightingTagHandle.Data.Get(), TargetASC);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
		SetShield(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
}

void UAttributeSetSoldier::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, const float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && ASC)
	{
		// Increase affected attribute to same max attribute increase
		// Only decrease affected attribute if max attribute is lower
		const float CurrentAffectedValue = AffectedAttribute.GetBaseValue();
		ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Override, FMath::Clamp(CurrentAffectedValue + NewMaxValue - CurrentMaxValue, FMath::Min(CurrentAffectedValue, NewMaxValue), NewMaxValue));
	}
}

void UAttributeSetSoldier::LevelUp()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (ASoldier* Soldier = Cast<ASoldier>(ASC->AbilityActorInfo->AvatarActor.Get()); Soldier)
	{
		// Increase level by 1
		ASC->ApplyModToAttributeUnsafe(GetCharacterLevelAttribute(), EGameplayModOp::Additive, 1);

		// Grant new attribute values
		FGameplayEffectSpecHandle Handle = ASC->MakeOutgoingSpec(Soldier->GetStatAttributeEffects(), GetCharacterLevel(), ASC->MakeEffectContext());
		ASC->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());
	}
}

void UAttributeSetSoldier::GrantEXP(const float _EXP)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	
	if (ASoldier* Soldier = Cast<ASoldier>(ASC->AbilityActorInfo->AvatarActor.Get()); Soldier)
	{
		ASC->ApplyModToAttributeUnsafe(GetEXPAttribute(), EGameplayModOp::Additive, _EXP);

		while (GetEXP() >= GetEXPLevelUp() && !FMath::IsNearlyEqual(GetCharacterLevel(), MaxLevel, 0.1f))
			Soldier->LevelUp();
	}
}

void UAttributeSetSoldier::OnRep_CharacterLevel(const FGameplayAttributeData& _OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, CharacterLevel, _OldCharacterLevel);
}

void UAttributeSetSoldier::OnRep_EXP(const FGameplayAttributeData& _OldEXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, EXP, _OldEXP);
}

void UAttributeSetSoldier::OnRep_EXPLevelUp(const FGameplayAttributeData& _OldEXPLevelUp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, EXPLevelUp, _OldEXPLevelUp);
}

void UAttributeSetSoldier::OnRep_Health(const FGameplayAttributeData& _OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, Health, _OldHealth);
}

void UAttributeSetSoldier::OnRep_MaxHealth(const FGameplayAttributeData& _OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, MaxHealth, _OldMaxHealth);
}

void UAttributeSetSoldier::OnRep_HealthRegenRate(const FGameplayAttributeData& _OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, HealthRegenRate, _OldHealthRegenRate);
}

void UAttributeSetSoldier::OnRep_Shield(const FGameplayAttributeData& _OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, Shield, _OldMaxShield);
}

void UAttributeSetSoldier::OnRep_MaxShield(const FGameplayAttributeData& _OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, MaxShield, _OldMaxHealth);
}

void UAttributeSetSoldier::OnRep_MoveSpeedWalk(const FGameplayAttributeData& _OldMoveSpeedWalk)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, MoveSpeedWalk, _OldMoveSpeedWalk);
}

void UAttributeSetSoldier::OnRep_MoveSpeedCrouch(const FGameplayAttributeData& _OldMoveSpeedCrouch)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, MoveSpeedCrouch, _OldMoveSpeedCrouch);
}

void UAttributeSetSoldier::OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& _OldMoveSpeedMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, MoveSpeedMultiplier, _OldMoveSpeedMultiplier);
}