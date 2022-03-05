// Fill out your copyright notice in the Description page of Project Settings.


#include "SRoomSelectorCombo.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SComboBox.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "SRoomSelectorCombo"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
/*void SRoomSelectorCombo::Construct(const FArguments& InArgs)
{
	Options = InArgs._Source.Get();
	
	ChildSlot
	[
		SNew(SComboBox<TSelectorComboType>)
		.OptionsSource(Options)
		.OnSelectionChanged(this, &SRoomSelectorCombo::OnSelectionChanged)
		.OnGenerateWidget(this, &SRoomSelectorCombo::MakeWidgetForOption)
		.InitiallySelectedItem(CurrentOption)
		[
			SNew(STextBlock)//.Text(LOCTEXT("None", "None"))
			.Text_Lambda([this]() -> FText
			{
				return CurrentOption ? FText::FromString(*CurrentOption->GetName()) : LOCTEXT("None", "None");
			})
		]
		//SNew(STextBlock).Text(LOCTEXT("None", "None"))
	];
	
}

//void SRoomSelectorCombo::AddOptionItem(TSelectorComboType Item)
//{	
//	Options->Add(Item);
//}

void SRoomSelectorCombo::OnSelectionChanged(TSelectorComboType NewValue, ESelectInfo::Type)
{
	CurrentOption = NewValue;
}

TSharedRef<SWidget> SRoomSelectorCombo::MakeWidgetForOption(TSelectorComboType InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption->GetName()));
}*/

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE