#pragma once
// forward declaration
class ASoldier;
class ASoldierSpawn;

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "../Interface/PreInitable.h"
#include "SoldierTeam.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoldierAddedToList, ASoldier*, NewSoldier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoldierRemovedFromList, ASoldier*, NewSoldier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoldierTeamIntChanged, int, NewTicket);

UCLASS(Blueprintable)
class SQUADLEADER_API ASoldierTeam : public AInfo, public IPreInitable
{
	GENERATED_BODY()

public:
	FOnSoldierTeamIntChanged OnTicketChanged;

	void BroadcastTickets();

public:
	ASoldierTeam();

	// Pre init launch by the gameMode before the BeginPlay() function
	virtual void PreInitialisation() override;
	virtual int GetPriority() const override;

protected:
	virtual void BeginPlay() override;

public:
	// for replication purpose
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "GlobalTeamData")
		FString TeamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GlobalTeamData")
		int Id = 0;

	UPROPERTY(EditAnywhere, Category = "GlobalTeamData")
		int OverrideNbAIBasicAssault = -1;
	UPROPERTY(EditAnywhere, Category = "GlobalTeamData")
		int OverrideNbAIBasicHeavy = -1;

	UPROPERTY(EditAnywhere, Category = "GlobalTeamData")
		int OverrideTickets = -1;

protected:  // Soldier List
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "SoldierList")
		TArray<ASoldier*> SoldierList;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int NbAIBasicAssault = 0;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int NbAIBasicHeavy = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		TArray<ASoldier*> GetSoldierList() const;
	UFUNCTION(BlueprintCallable)
		int const GetNbAIBasicAssault() { return NbAIBasicAssault; }
	UFUNCTION(BlueprintCallable)
		int const GetNbAIBasicHeavy() { return NbAIBasicHeavy; }

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> ClassBasicAI;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> ClassBasicAIAssault;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> ClassBasicAIHeavy;

public:
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> GetClassBasicAI();

	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> GetClassBasicAIAssault();

	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		TSubclassOf<class ASoldierAI> GetClassBasicAIHeavy();

	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void AddSoldierList(ASoldier* NewSoldier);
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void RemoveSoldierList(ASoldier* NewSoldier);
	UFUNCTION(BlueprintCallable, Category = "SoldierList")
		void CleanSoldierList();

	UPROPERTY()
		FSoldierAddedToList OnSoldierAddedToList;

	UPROPERTY()
		FSoldierRemovedFromList OnSoldierRemovedFromList;

protected: // Spawn points
	UPROPERTY(VisibleAnywhere, Replicated, Category = "SpawnPoints")
		TArray<ASoldierSpawn*> mainSpawnPoints;
public:
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void AddSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void RemoveSpawn(ASoldierSpawn* newSpawn);
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		void CleanSpawnPoints();
	UFUNCTION(BlueprintCallable, Category = "SpawnPoints")
		TArray<ASoldierSpawn*> GetUsableSpawnPoints();

protected:  // Tickets
	UPROPERTY(Replicated)
	int Tickets = 100;  // default value, will be changed base on the gameMode base param

	UFUNCTION(Server, Reliable)
	void InformAllPlayerController();

public:
	UFUNCTION(BlueprintCallable, Category = "Tickets")
		void RemoveOneTicket();
	UFUNCTION(BlueprintCallable, Category = "Tickets")
		int GetTicket() const { return Tickets; }
};
