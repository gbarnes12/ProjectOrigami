// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/AIModule/Private/AIModulePrivate.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Runtime/AIModule/Classes/EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Actors/OrbInteractables/OrbInteractable.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"
#include "EnvQueryGenerator_FreeAttachment.h"

UEnvQueryGenerator_FreeAttachment::UEnvQueryGenerator_FreeAttachment(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
	ItemType = UEnvQueryItemType_Actor::StaticClass();
	SearchRadius.DefaultValue = 500.0f;
}

void UEnvQueryGenerator_FreeAttachment::GenerateItems(FEnvQueryInstance& QueryInstance) const
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

	for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, AOrbInteractable::StaticClass()); ItActor; ++ItActor)
	{
		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex)
		{
			if (FVector::DistSquared(ContextLocations[ContextIndex], ItActor->GetActorLocation()) < RadiusSq)
			{
				AOrbInteractable* interactable = Cast<AOrbInteractable>(*ItActor);
				if (interactable && interactable->GetIsUsable())
				{
					QueryInstance.AddItemData<UEnvQueryItemType_Actor>(*ItActor);
				}
				break;
			}
		}
	}
}

FText UEnvQueryGenerator_FreeAttachment::GetDescriptionTitle() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DescriptionTitle"), Super::GetDescriptionTitle());
	Args.Add(TEXT("ActorsClass"), FText::FromString(GetNameSafe(AOrbInteractable::StaticClass())));
	Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(SearchCenter));

	return FText::Format(LOCTEXT("DescriptionGenerateActorsAroundContext", "{DescriptionTitle}: generate set of actors of {ActorsClass} around {DescribeContext}"), Args);
};

FText UEnvQueryGenerator_FreeAttachment::GetDescriptionDetails() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));

	FText Desc = FText::Format(LOCTEXT("FreeAttachmentDescription", "radius: {Radius}"), Args);

	return Desc;
}

#undef LOCTEXT_NAMESPACE
