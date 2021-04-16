#include "SquadLeaderGameInstance.h"

USquadLeaderGameInstance::USquadLeaderGameInstance() {
    //When the object is constructed, Get the HTTP module
    Http = &FHttpModule::Get();
}

void USquadLeaderGameInstance::OnStart()
{
    Super::OnStart();
    UserData.LoadOrCreate(UserDataFilename);
    HttpCallPing(BaseServerDataAdress);

}


void USquadLeaderGameInstance::HttpCallPing(FString BaseAdress)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedPing);
    //This is the url on which to process the request
    Request->SetURL(BaseAdress + "ping/");
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::OnResponseReceivedPing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // able to ping the data server, continu the sync with the server
        // check if the player need an account
        if (UserData.Id == "") {
            // create an account and connect the player
            int test = 12;
            test++;
        }
        // connect the user
    }
    // no connection to the data server
    else {
        // arrange offline system
    }
}