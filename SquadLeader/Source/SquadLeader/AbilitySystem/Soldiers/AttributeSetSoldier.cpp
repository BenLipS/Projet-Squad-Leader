#include "AttributeSetSoldier.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "../../Soldiers/Soldier.h"
#include "GameplayEffects/States/GE_StateFighting.h"
#include "GameplayEffects/GE_UpdateStats.h"

UAttributeSetSoldier::UAttributeSetSoldier(const FObjectInitializer& _ObjectInitializer) : Super(_ObjectInitializer)
{
	CharacterLevel = 1.f;
}

void UAttributeSetSoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeSetSoldier, CharacterLevel);
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
				// TODO: SourceSoldier should get prestige/experience after killing. Then, in attributeSet we handle leveling up based on exp value
				if (SourceSoldier)
				{
					SourceSoldier->GetAttributeSet()->LevelUp();
				}
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
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
		ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, FMath::CeilToFloat(NewDelta));
	}
}

// TODO: See what to do with basic AIs and squad AIs. 
// Should they solo level up ? Do their levels depend of the leader of squad ?
// Do the Basic AIs have the same than the lower player/squad leader ?
void UAttributeSetSoldier::LevelUp()
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
		return;

	if (ASoldier* Soldier = Cast<ASoldier>(ASC->AbilityActorInfo->AvatarActor.Get()); Soldier)
	{
		// Increase level by 1
		ASC->ApplyModToAttributeUnsafe(GetCharacterLevelAttribute(), EGameplayModOp::Additive, 1);

		// Grant new attribute values
		FGameplayEffectSpecHandle Handle = ASC->MakeOutgoingSpec(Soldier->GetStatAttributeEffects(), GetCharacterLevel(), ASC->MakeEffectContext());
		ASC->ApplyGameplayEffectSpecToSelf(*Handle.Data.Get());

		// TODO: We should not give full hp when leveling up
	}
}

void UAttributeSetSoldier::OnRep_CharacterLevel(const FGameplayAttributeData& _OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSoldier, CharacterLevel, _OldCharacterLevel);
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