#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UChatInterface : public UInterface
{
	GENERATED_BODY()
};

class SQUADLEADER_API IChatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnChatMessageReceived(const FString& message) = 0;

	UFUNCTION()
	virtual void OnChatInputPressed() = 0;
};
