#include "AttributeSetSoldier.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "../../Soldiers/Soldier.h"

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

	//if (Attribute == GetMaxHealthAttribute())
	//	AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
}

void UAttributeSetSoldier::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (!Data.Target.AbilityActorInfo.IsValid() || !Data.Target.AbilityActorInfo->AvatarActor.IsValid())
		return;

	ASoldier* TargetSoldier = Cast<ASoldier>(Data.Target.AbilityActorInfo->AvatarActor.Get());

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Make a local damage copy
		const float LocalDamage = GetDamage();
		SetDamage(0.f);

		if (LocalDamage > 0.0f && TargetSoldier->IsAlive())
		{
			if (LocalDamage <= GetShield())
				SetShield(FMath::Clamp(GetShield() - LocalDamage, 0.0f, GetMaxShield()));
			else
			{
				SetShield(0.f);
				SetHealth(FMath::Clamp(GetHealth() - (LocalDamage - GetShield()), 0.0f, GetMaxHealth()));

				GEngine->AddOnScreenDebugMessage(78, 1.f, FColor::Red, FString::Printf(TEXT("%s still has %s HPs"), *TargetSoldier->GetName(), *FString::SanitizeFloat(GetHealth())));

				if (!TargetSoldier->IsAlive()) // The soldier has been killed
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s is dead"), *TargetSoldier->GetName()));
					//TargetSoldier->die();
					//SourceSoldier->getBounty();
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
		SetHealth(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
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