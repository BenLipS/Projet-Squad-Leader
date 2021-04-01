// Copyright 2020 Dan Kestranek.


#include "SL_BlueprintFunctionLibrary.h"

FString USL_BlueprintFunctionLibrary::GetPlayerEditorWindowRole(UWorld* World)
{
	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	return Prefix;
}

UGameplayAbilitySoldier* USL_BlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass);
		if (AbilitySpec)
		{
			return Cast<UGameplayAbilitySoldier>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

bool USL_BlueprintFunctionLibrary::IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UGameplayAbilitySoldier>(AbilitySpec->GetPrimaryInstance())->IsActive();
		}
	}

	return false;
}

bool USL_BlueprintFunctionLibrary::IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle)
{
	return Handle.IsValid();
}

bool USL_BlueprintFunctionLibrary::DoesEffectContainerSpecHaveEffects(const FSL_GameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool USL_BlueprintFunctionLibrary::DoesEffectContainerSpecHaveTargets(const FSL_GameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

void USL_BlueprintFunctionLibrary::ClearEffectContainerSpecTargets(FSL_GameplayEffectContainerSpec& ContainerSpec)
{
	ContainerSpec.ClearTargets();
}

void USL_BlueprintFunctionLibrary::AddTargetsToEffectContainerSpec(FSL_GameplayEffectContainerSpec& ContainerSpec, const TArray<FGameplayAbilityTargetDataHandle>& TargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	ContainerSpec.AddTargets(TargetData, HitResults, TargetActors);
}

TArray<FActiveGameplayEffectHandle> USL_BlueprintFunctionLibrary::ApplyExternalEffectContainerSpec(const FSL_GameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

FGameplayAbilityTargetDataHandle USL_BlueprintFunctionLibrary::EffectContextGetTargetData(FGameplayEffectContextHandle EffectContextHandle)
{
	FSL_GameplayEffectContext* EffectContext = static_cast<FSL_GameplayEffectContext*>(EffectContextHandle.Get());
	
	if (EffectContext)
	{
		return EffectContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}

void USL_BlueprintFunctionLibrary::EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData)
{
	FSL_GameplayEffectContext* EffectContext = static_cast<FSL_GameplayEffectContext*>(EffectContextHandle.Get());

	if (EffectContext)
	{
		EffectContext->AddTargetData(TargetData);
	}
}

void USL_BlueprintFunctionLibrary::ClearTargetData(FGameplayAbilityTargetDataHandle& TargetData)
{
	TargetData.Clear();
}
