#include "RDGA_Dash.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

URDGA_Dash::URDGA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Dash")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Dash")));
}

void URDGA_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Dash ActivateAbility"));

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	ACharacter* Char = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

	if (!ASC || !Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Если уже на кулдауне — просто завершаем
	const FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Dash"));
	if (ASC->HasMatchingGameplayTag(CooldownTag))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 1) Рывок (импульс вперёд)
	const FVector Forward = Char->GetActorForwardVector();
	UE_LOG(LogTemp, Warning, TEXT("DashStrength=%f, VelBefore=%s, Role=%d"),
		DashStrength,
		*Char->GetVelocity().ToString(),
		(int32)Char->GetLocalRole());
	Char->LaunchCharacter(Forward * DashStrength, /*XYOverride*/ true, /*ZOverride*/ false);
	UE_LOG(LogTemp, Warning, TEXT("VelAfter=%s"), *Char->GetVelocity().ToString());


	// 2) Ставим "кулдаун" тег на 15 секунд через loose tag + таймер
	ASC->AddLooseGameplayTag(CooldownTag);

	FTimerHandle TimerHandle;
	Char->GetWorldTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(ASC, [ASC, CooldownTag]()
			{
				if (ASC)
				{
					ASC->RemoveLooseGameplayTag(CooldownTag);
				}
			}),
		CooldownSeconds,
		false
	);

	// 3) Завершаем ability сразу (ей не нужно жить 15 сек)
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}