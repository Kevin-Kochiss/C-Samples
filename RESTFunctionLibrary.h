// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsylumPaths.h"
#include "JsonObjectConverter.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AsylumProject/Headers/URLDirectory.h"
#include "AsylumProject/Headers/RESTStructs.h"
#include "RESTFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ASYLUMPROJECT_API URESTFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	
	/*
	* Saves a Base64 image string to a png on disk
	* 
	* @param	WorldContextObject	--	UObject that is calling this function
	* @param	Base64String	--	The image string to be saved to disk
	* @param	FilePath	--	Destination path to save the file
	* @param	FileName	--	Deseried name of the file to be saved on disk (exclude .png)
	*/
	static void SaveBase64Image(UObject* WorldContextObject, FString Base64String, FString FilePath, FString FileName);
	
	/*
	* Generates the string representation of the supplied Json Object, useful for debugging
	* 
	* @param	--	InJson, TSharedRef to the JsonObject
	* @return FString Represnetation of the Json Object
	*/
	static FString JsonToString(TSharedRef<FJsonObject> InJson);

private:

	
	/*
	* Draws the supplied render texture to the supplied Canvas
	* 
	* @param	Canvas	--	Canvas to be drawn to
	* @param	RenderTexture	--	Render texture to draw
	* @param	ScreenPosition	--	2D screen position
	* @param	ScreenSize	--	2D screen size
	* @param	CoordinatePosition	--	2D coordinate position
	*/
	static void DrawTexture(UCanvas* Canvas, UTexture2D* RenderTexture, FVector2D ScreenPosition, FVector2D ScreenSize, FVector2D CoordinatePosition);
	
	/*
	* Generates the body for a standard http request
	* 
	* @param	Request	--	The request to be sent, in which the body will be applied
	* @param	Verb	--	The type of request to be made, GET, POST, ect.
	* @param	URL	--	The to which the request will be made
	*/
	static void GenerateRequestBody(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request, FString Verb, FString URL);

	

public:
/*
*						TEMPLATE FUNCTIONS
*/
	/**
	* Method: POST
	*
	* @param	API_URL	--		URL to the base API class + URL to the API function (+PK)
	* @param    InJson	--		Json Object to be sent in the request
	* @param	OnRequestComplete -- Function Signature: void OnResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	*/
	template<typename Object>
	static void PostRequest(FString API_URL, TSharedPtr<FJsonObject> InJson, Object* InObject, void (Object::* OnRequestComplete)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));
	//*****I would like to make another version that takes in a struct and does the conversion for you

	/**
	 * Method: GET
	 *
	 * @param	API_URL --				URL to the base API class
	 * @param	APPLICATION_URL -- 	URL to the API function
	 * @param	OnRequestComplete --	Function Signature: void OnResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	 */
	template<typename Object>
	static void GetRequest(FString API_URL, Object* InObject, void (Object::* OnRequestComplete)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));

	
	/**
	 * Converts a Http Response Object to a <T>struct
	 *
	 * @param	Response --	Http Response to be Serialized
	 * @param	OutStruct -- <T>Struct to be written to 
	 */
	template<class T>
	static void SerializeJsonToStruct(FHttpResponsePtr Response, T* OutStruct);

	/**
	 * Converts a <T>struct to and Json Object 
	 *
	 * @param	InStruct --		Refrence of the struct to be Serialized
	 * @retrun Json Object
	 */
	template<typename T>
	static TSharedRef<FJsonObject> SerializeStructToJson(T* InStruct);

};

/*
*			TEMPLATE DEFINITIONS
*/


template<typename Object>
inline void URESTFunctionLibrary::PostRequest(FString API_URL, TSharedPtr<FJsonObject> InJson, Object* InObject, void(Object::* OnRequestComplete)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FHttpModule& Http = FHttpModule::Get();

	// create a new HTTP request and bind the response callback
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(InObject, OnRequestComplete);

	GenerateRequestBody(Request, "POST", API_URL );

	//Converts Json object to a string to be sent via POST request	
	FString ContentString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(InJson.ToSharedRef(), Writer);

	Request->SetContentAsString(ContentString);

	//if auth token = "None" don't add to header
	//Request->SetHeader("Authorization", "Token <INSERT-TOKEN>") 

	Request->ProcessRequest();
}

template<typename Object>
inline void URESTFunctionLibrary::GetRequest(FString API_URL, Object* InObject, void(Object::* OnRequestComplete)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FHttpModule& Http = FHttpModule::Get();

	// create a new HTTP request and bind the response callback
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(InObject, OnRequestComplete);

	GenerateRequestBody(Request, "GET", API_URL);
	//Request->SetHeader("Authorization", "Token <INSERT-TOKEN>") 

	Request->ProcessRequest();
}

template<class T>
inline void URESTFunctionLibrary::SerializeJsonToStruct(FHttpResponsePtr Response, T* OutStruct)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

	FJsonSerializer::Deserialize(JsonReader, JsonObject);

	FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObject.ToSharedRef(), OutStruct);
}

template<typename T>
inline TSharedRef<FJsonObject> URESTFunctionLibrary::SerializeStructToJson(T* InStruct)
{

	TSharedRef<FJsonObject> OutJson = MakeShareable(new FJsonObject);

	FJsonObjectConverter::UStructToJsonObject(T::StaticStruct(), InStruct, OutJson, 0, 0);

	FString JsonString;
	auto JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);
	FJsonSerializer::Serialize(OutJson, JsonWriter);
	JsonWriter->Close();

	return OutJson;
}
