#include "RDThirdPersonCharacter.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"

#include "RDGA_Dash.h"

ARDThirdPersonCharacter::ARDThirdPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	DashAbilityClass = URDGA_Dash::StaticClass();
}

UAbilitySystemComponent* ARDThirdPersonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARDThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("RDThirdPersonCharacter BeginPlay"));

	if (!AbilitySystemComponent) return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (HasAuthority() && DashAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DashAbilityClass, 1, 0));
		UE_LOG(LogTemp, Warning, TEXT("Dash ability granted"));
	}
}

void ARDThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARDThirdPersonCharacter::TriggerDashFromBP()
{
	UE_LOG(LogTemp, Warning, TEXT("TriggerDashFromBP called"));

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent is null"));
		return;
	}

	if (!DashAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DashAbilityClass is null"));
		return;
	}

	const bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(DashAbilityClass);
	UE_LOG(LogTemp, Warning, TEXT("TryActivateAbilityByClass = %s"), bActivated ? TEXT("true") : TEXT("false"));
}