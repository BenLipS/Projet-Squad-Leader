#include "AbilitySystemSoldier.h"
#include "GameplayAbilitySoldier.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Algo/MaxElement.h"
#include "Algo/Find.h"

bool UAbilitySystemSoldier::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle _InAbilityHandle, bool _EndAbilityImmediately)
{
	bool AbilityActivated = false;
	if (_InAbilityHandle.IsValid())
	{
		FScopedServerAbilityRPCBatcher AbilityRPCBatcher(this, _InAbilityHandle);
		AbilityActivated = TryActivateAbility(_InAbilityHandle, true);

		if (_EndAbilityImmediately)
		{
			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(_InAbilityHandle);
			if (AbilitySpec)
			{
				UGameplayAbilitySoldier* GA = Cast<UGameplayAbilitySoldier>(AbilitySpec->GetPrimaryInstance());
				GA->ExternalEndAbility();
			}
		}
		return AbilityActivated;
	}
	return AbilityActivated;
}

FGameplayAbilitySpecHandle UAbilitySystemSoldier::FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> _AbilityClass, UObject* _OptionalSourceObject)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		TSubclassOf<UGameplayAbility> SpecAbilityClass = Spec.Ability->GetClass();
		if (SpecAbilityClass == _AbilityClass)
		{
			if (!_OptionalSourceObject || (_OptionalSourceObject && Spec.SourceObject == _OptionalSourceObject))
			{
				return Spec.Handle;
			}
		}
	}
	return FGameplayAbilitySpecHandle();
}

UGameplayAbilitySoldier* UAbilitySystemSoldier::GetPrimaryAbilityInstanceFromHandle(FGameplayAbilitySpecHandle _Handle)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(_Handle);
	return AbilitySpec ? Cast<UGameplayAbilitySoldier>(AbilitySpec->GetPrimaryInstance()) : nullptr;
}

void UAbilitySystemSoldier::ExecuteGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), _GameplayCueTag, EGameplayCueEvent::Type::Executed, _GameplayCueParameters);
}

void UAbilitySystemSoldier::AddGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), _GameplayCueTag, EGameplayCueEvent::Type::OnActive, _GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), _GameplayCueTag, EGameplayCueEvent::Type::WhileActive, _GameplayCueParameters);
}

void UAbilitySystemSoldier::RemoveGameplayCueLocal(const FGameplayTag _GameplayCueTag, const FGameplayCueParameters& _GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), _GameplayCueTag, EGameplayCueEvent::Type::Removed, _GameplayCueParameters);
}

bool UAbilitySystemSoldier::IsInCooldown(const FGameplayTag& _Tag)
{
	float RemainTime = 0.00f;
	const bool bTagFoundAsCooldown = GetCooldownRemainingForTag(_Tag, RemainTime);
	return bTagFoundAsCooldown && (RemainTime > 0.00f);
}

bool UAbilitySystemSoldier::GetCooldownRemainingForTags(const FGameplayTagContainer& _CooldownTags, float& _TimeRemaining)
{
	if (_CooldownTags.Num() > 0)
	{
		_TimeRemaining = 0.f;

		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(_CooldownTags);
		const TArray<float> TimeRemainingList = GetActiveEffectsTimeRemaining(Query);

		if (TimeRemainingList.Num() > 0)
		{
			_TimeRemaining = *Algo::MaxElement(TimeRemainingList, [](const float time1, const float time2) { return time1 < time2; });
			return true;
		}
	}
	return false;
}

bool UAbilitySystemSoldier::GetCooldownRemainingForTag(const FGameplayTag& _CooldownTags, float& _TimeRemaining)
{
	const FGameplayTagContainer CooldownTags{ _CooldownTags };
	return GetCooldownRemainingForTags(CooldownTags, _TimeRemaining);
}

bool UAbilitySystemSoldier::GetCooldownRemainingAndDurationForTags(const FGameplayTagContainer& _CooldownTags, float& _TimeRemaining, float& _CooldownDuration)
{
	if (_CooldownTags.Num() > 0)
	{
		_TimeRemaining = 0.f;
		_CooldownDuration = 0.f;

		const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(_CooldownTags);
		const TArray<TPair<float, float>> DurationAndTimeRemainingList = GetActiveEffectsTimeRemainingAndDuration(Query);

		if (DurationAndTimeRemainingList.Num() > 0)
		{
			const TPair<float, float> DurationAndTimeRemaining = *Algo::MaxElement(DurationAndTimeRemainingList, [](const TPair<float, float>& pair1, const TPair<float, float>& pair2) { return pair1.Key < pair2.Key; });

			_TimeRemaining = DurationAndTimeRemaining.Key;
			_CooldownDuration = DurationAndTimeRemaining.Value;
			return true;
		}
	}
	return false;
}

bool UAbilitySystemSoldier::GetCooldownRemainingAndDurationForTag(const FGameplayTag& _CooldownTags, float& _TimeRemaining, float& _CooldownDuration)
{
	const FGameplayTagContainer CooldownTags{ _CooldownTags };
	return GetCooldownRemainingAndDurationForTags(CooldownTags, _TimeRemaining, _CooldownDuration);
}

float UAbilitySystemSoldier::GetCooldownRemainingFromAbilityID(const ESoldierAbilityInputID _AbilityID)
{
	// Find the ability matching _AbilityID
	auto It_Ability = Algo::FindByPredicate(GetActivatableAbilities(), [_AbilityID](FGameplayAbilitySpec AbilitySpec)
	{
		return AbilitySpec.InputID == static_cast<int32>(_AbilityID);
	});

	if (!It_Ability)
		return 0.f;

	// Get the cooldown
	UGameplayAbilitySoldier* Ability = Cast<UGameplayAbilitySoldier>(It_Ability->Ability);
	if (Ability)
	{
		float RemainTime = 0.00f;
		GetCooldownRemainingForTags(*Ability->GetCooldownTags(), RemainTime);
		return RemainTime;
	}
	return 0.f;
}