// Fill out your copyright notice in the Description page of Project Settings.


#include "RESTFunctionLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Misc/Base64.h"

void URESTFunctionLibrary::SaveBase64Image(UObject* WorldContextObject, FString Base64String, FString FilePath, FString FileName)
{
	FString TotalFileName = FPaths::Combine(*FilePath, *FileName);
	FText PathError;
	FPaths::ValidatePath(TotalFileName, &PathError);
	if (!PathError.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Inavlid Path"));
	}

	//Decode Base64 String into a byte Array that can be imported as a Texture
	TArray<uint8> byteArray;
	FBase64::Decode(Base64String, byteArray);
	UTexture2D* Texture = UKismetRenderingLibrary::ImportBufferAsTexture2D(WorldContextObject, byteArray);

	//Creates Render Target
	int32 Width = Texture->GetSizeX();
	int32 Height = Texture->GetSizeY();
	UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(WorldContextObject, Width, Height, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::White);

	//Begin to draw render target onto canavs
	UCanvas* Canvas;
	FVector2D ScreenSize;
	FDrawToRenderTargetContext Context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(WorldContextObject, RenderTarget, Canvas, ScreenSize, Context);
	DrawTexture(Canvas, Texture, FVector2D(0.f, 0.f), ScreenSize, FVector2D(0.f, 0.f));

	//Export Render Target as PNG
	UKismetRenderingLibrary::ExportRenderTarget(WorldContextObject, RenderTarget, FilePath, FileName + ".png");
	//Clean Up
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(WorldContextObject, Context);
	UKismetRenderingLibrary::ClearRenderTarget2D(WorldContextObject, RenderTarget);
}

void URESTFunctionLibrary::DrawTexture(UCanvas* Canvas, UTexture2D* RenderTexture, FVector2D ScreenPosition, FVector2D ScreenSize, FVector2D CoordinatePosition)
{
	if (ScreenSize.X > 0.0f && ScreenSize.Y > 0.0f && Canvas && RenderTexture)
	{
		FTexture* RenderTextureResource = RenderTexture->Resource;
		// ScreenSize, CoordinatePosition, CoordinatePosition + FVector2D::UnitVector,
		FCanvasTileItem TileItem(ScreenPosition, RenderTextureResource, FLinearColor::White);
		TileItem.Rotation = FRotator(0, 0, 0);
		TileItem.PivotPoint = FVector2D(0.5f, 0.5f);
		TileItem.BlendMode = FCanvas::BlendToSimpleElementBlend(EBlendMode::BLEND_Translucent);
		Canvas->DrawItem(TileItem);

	}
}

void URESTFunctionLibrary::GenerateRequestBody(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request, FString Verb, FString URL)
{
	Request->SetURL(URL);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
}

FString URESTFunctionLibrary::JsonToString(TSharedRef<FJsonObject> InJson)
{
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(InJson, Writer);
	return OutputString;
}
