#include "AttributeSetSoldier.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "../../Soldiers/Soldier.h"
#include "GameplayEffects/States/GE_StateFighting.h"

void UAttributeSetSoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeSetSoldier, CharacterLevel);
	DOREPLIFETIME(UAttributeSetSoldier, Health);
	DOREPLIFETIME(UAttributeSetSoldier, MaxHealth);
	DOREPLIFETIME(UAttributeSetSoldier, HealthRegenRate);
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
			SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth()));

			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s loses %s hp (remain %s hp)"), *TargetSoldier->GetName(), *FString::SanitizeFloat(LocalDamage), *FString::SanitizeFloat(GetHealth())));

			if (!TargetSoldier->IsAlive()) // The soldier has been killed
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s is dead"), *TargetSoldier->GetName()));
				//SourceSoldier->getBounty();
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