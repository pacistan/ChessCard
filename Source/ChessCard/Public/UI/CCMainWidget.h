#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCMainWidget.generated.h"


UCLASS()
class CHESSCARD_API UCCMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSelectCardChange(bool IsCardSelected);
};
