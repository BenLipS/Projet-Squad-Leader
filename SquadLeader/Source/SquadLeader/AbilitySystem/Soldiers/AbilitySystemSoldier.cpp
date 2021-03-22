#include "AbilitySystemSoldier.h"
#include "GameplayAbilitySoldier.h"

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

FGameplayAbilitySpecHandle UAbilitySystemSoldier::FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		TSubclassOf<UGameplayAbility> SpecAbilityClass = Spec.Ability->GetClass();
		if (SpecAbilityClass == AbilityClass)
		{
			if (!OptionalSourceObject || (OptionalSourceObject && Spec.SourceObject == OptionalSourceObject))
			{
				return Spec.Handle;
			}
		}
	}
	return FGameplayAbilitySpecHandle();
}