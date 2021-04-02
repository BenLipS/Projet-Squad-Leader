#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Shield.generated.h"

UCLASS()
class SQUADLEADER_API AShield : public AActor
{
	GENERATED_BODY()
	
public:	
	AShield();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;
};
