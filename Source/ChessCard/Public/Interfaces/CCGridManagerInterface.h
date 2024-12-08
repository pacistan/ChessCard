#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CCGridManagerInterface.generated.h"

class ACCGridManager;
// This class does not need to be modified.
UINTERFACE()
class UCCGridManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHESSCARD_API ICCGridManagerInterface
{
	GENERATED_BODY()

protected:
	ACCGridManager* GetGridManager(UWorld* World);
};
