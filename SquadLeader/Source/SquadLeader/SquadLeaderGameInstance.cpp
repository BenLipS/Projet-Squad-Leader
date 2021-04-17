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


void USquadLeaderGameInstance::LaunchGame()
{
    GetFirstGamePlayer()->ConsoleCommand("open Factory_V1?listen", true);
}

void USquadLeaderGameInstance::JoinGame(FString IPAdress)
{
    GetFirstGamePlayer()->ConsoleCommand("open " + IPAdress, true);
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

void USquadLeaderGameInstance::HttpCallCreateUser(FString BaseAdress)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedCreateUser);
    //This is the url on which to process the request
    Request->SetURL(BaseAdress);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallConnectUser(FString BaseAdress)
{
    FString content = FString::Printf(TEXT("guestToken=%s"),
        *FGenericPlatformHttp::UrlEncode(UserData.GuestToken));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedConnectUser);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    Request->SetURL(BaseAdress + UserData.Id + "/sessions/create");
    Request->SetContentAsString(content);
    Request->SetVerb("POST");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallSendSyncData(FString BaseAdress)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedSendSync);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseAdress + UserData.Id + "/?name=" + UserData.Name + "&ipAdress=" + UserData.IpAdress + "&isInGame=" + 0);  // TODO : find the IPAdress
    Request->SetVerb("PATCH");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallReceiveSyncData(FString BaseAdress)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedReceiveSync);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseAdress + UserData.Id);
    Request->SetVerb("GET");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::OnResponseReceivedPing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // able to ping the data server, continu the sync with the server
        // check if the player need an account
        if (UserData.Id == "") {
            // create an account and connect the player
            HttpCallCreateUser(BaseServerDataAdress);
        }
        else {
            // connect the user
            HttpCallConnectUser(BaseServerDataAdress);
        }
    }
    else {  // no connection to the data server
        // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseReceivedCreateUser(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {  // able to ping the data server, sync the local data and save it, then continu with a connection
        TSharedPtr<FJsonObject> JsonObject;
        //Create a reader pointer to read the json data
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TJsonReader<TCHAR>>::Create(Response->GetContentAsString());
        //Deserialize the json data given Reader and the actual object to deserialize
        if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
            //Get the value of the json object by field name
            UserData.Id = JsonObject->GetStringField("id");
            UserData.GuestToken = JsonObject->GetStringField("guestToken");
            UserData.Save(UserDataFilename);
        }
        HttpCallConnectUser(BaseServerDataAdress);
    }
    else {  // no connection to the data server
        // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseReceivedConnectUser(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // save the identification token sent by the server, and sync the remaining data with the server
        AuthToken = Response->GetContentAsString();

        // change the name, the IPAdress and the IsInGame status on server, and download scores
        HttpCallSendSyncData(BaseServerDataAdress);
        HttpCallReceiveSyncData(BaseServerDataAdress);
    }
    else {  // connection problems
        // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseReceivedSendSync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (!bWasSuccessful) {  // no connection to the data server
        // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseReceivedReceiveSync(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // save imported data
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TJsonReader<TCHAR>>::Create(Response->GetContentAsString());
        if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
            //Get the value of the json object by field name
            UserData.NbKillIA = JsonObject->GetIntegerField("nbKillIA");
            UserData.NbKillPlayer = JsonObject->GetIntegerField("nbKillPlayer");
            UserData.NbDeathIA = JsonObject->GetIntegerField("nbDeathIA");
            UserData.NbDeathPlayer = JsonObject->GetIntegerField("nbDeathPlayer");
            UserData.NbVictory = JsonObject->GetIntegerField("nbVictory");
            UserData.NbLoss = JsonObject->GetIntegerField("nbLoss");
            UserData.Score = JsonObject->GetIntegerField("score");
            UserData.PlayTime = JsonObject->GetIntegerField("playTime");
            UserData.Friend;  // TODO: add friend system

            UserData.Save(UserDataFilename);
        }
    }
    else {  // no connection to the data server
        // arrange offline system
    }
}