#include "RDThirdPersonCharacter.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"

#include "RDGA_Dash.h"

ARDThirdPersonCharacter::ARDThirdPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// GAS
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Default ability class (можно переопределить в BP)
	DashAbilityClass = URDGA_Dash::StaticClass();
}

UAbilitySystemComponent* ARDThirdPersonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARDThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 1) Enhanced Input: добавить Mapping Context локальному игроку
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				if (DefaultMappingContext)
				{
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}

	// 2) GAS init
	if (!AbilitySystemComponent) return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// 3) Выдать абилку (на сервере)
	if (HasAuthority() && DashAbilityClass)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DashAbilityClass, 1, 0));
	}
}

void ARDThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input binding
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (DashInputAction)
		{
			EnhancedInput->BindAction(
				DashInputAction,
				ETriggerEvent::Started,
				this,
				&ARDThirdPersonCharacter::DashTriggered
			);
		}
	}
}

void ARDThirdPersonCharacter::DashTriggered(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("DashTriggered fired"));
	// Для Boolean action обычно true при нажатии; можно игнорировать
	if (!AbilitySystemComponent || !DashAbilityClass) return;

	AbilitySystemComponent->TryActivateAbilityByClass(DashAbilityClass);
}