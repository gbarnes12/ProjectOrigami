// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/AIModule/Private/AIModulePrivate.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Actors/Entities/Cocoon.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"
#include "EnvQueryGenerator_FreeCocoons.h"

UEnvQueryGenerator_FreeCocoons::UEnvQueryGenerator_FreeCocoons(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
	ItemType = UEnvQueryItemType_Actor::StaticClass();
	SearchRadius.DefaultValue = 500.0f;
}

void UEnvQueryGenerator_FreeCocoons::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	SearchRadius.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	float RadiusValue = SearchRadius.GetValue();

	UWorld* World = GEngine->GetWorldFromContextObject(QueryInstance.Owner.Get());
	if (World == NULL)
	{
		return;
	}

	const float RadiusSq = FMath::Square(RadiusValue);

	TArray<FVector> ContextLocations;
	QueryInstance.PrepareContext(SearchCenter, ContextLocations);

	for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, ACocoon::StaticClass()); ItActor; ++ItActor)
	{
		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex)
		{
			if (FVector::DistSquared(ContextLocations[ContextIndex], ItActor->GetActorLocation()) < RadiusSq)
			{
				ACocoon* cocoon = Cast<ACocoon>(*ItActor);
				if(cocoon && cocoon->GetIsFree()) 
				{
					QueryInstance.AddItemData<UEnvQueryItemType_Actor>(*ItActor);
				}
				break;
			}
		}
	}
}

FText UEnvQueryGenerator_FreeCocoons::GetDescriptionTitle() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DescriptionTitle"), Super::GetDescriptionTitle());
	Args.Add(TEXT("ActorsClass"), FText::FromString(GetNameSafe(ACocoon::StaticClass())));
	Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(SearchCenter));

	return FText::Format(LOCTEXT("DescriptionGenerateActorsAroundContext", "{DescriptionTitle}: generate set of actors of {ActorsClass} around {DescribeContext}"), Args);
};

FText UEnvQueryGenerator_FreeCocoons::GetDescriptionDetails() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));

	FText Desc = FText::Format(LOCTEXT("ActorsOfClassDescription", "radius: {Radius}"), Args);

	return Desc;
}

#undef LOCTEXT_NAMESPACE

