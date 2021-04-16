// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlAreaInfoWidget.h"

void UControlAreaInfoWidget::OnControlAreaChange(int newOwner, int newCapturer, float Percentage)
{
	if (ImageControlArea)
	{
		auto mat = ImageControlArea->GetDynamicMaterial();
		if (mat)
		{
			if (newOwner != Owner)
			{
				Owner = newOwner;
				mat->SetVectorParameterValue("OutterColor", GetColorFromOwner(Owner) * 0.1f);
			}

			if (newCapturer != Capturer)
			{
				Capturer = newCapturer;
				mat->SetVectorParameterValue("BottomInnerColor", GetColorFromOwner(Capturer));
			}
			mat->SetScalarParameterValue("Percentage_fill", Percentage);
			ImageControlArea->SetBrushFromMaterial(mat);
		}
	}
}

FLinearColor UControlAreaInfoWidget::GetColorFromOwner(int ownerIn)
{
	return ownerIn < 0 ? EnnemyColor : ownerIn == 0 ? NeutralColor : AllyColor;
}