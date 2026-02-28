#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RDGA_Dash.generated.h"

UCLASS()
class ROBOTDREAMS_API URDGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URDGA_Dash();

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashStrength = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float CooldownSeconds = 15.f;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};