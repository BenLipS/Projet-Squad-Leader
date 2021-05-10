#include "SquadLeaderGameInstance.h"
#include "MainMenu/SLMainMenuGameModeBase.h"
#include "UI/Interface/StatInfoInterface.h"
#include "UI/Interface/StatInfoInterface.h"
#include "UI/Menu/MenuItem/MenuList/MenuListInfo.h"
#include "SquadLeader/UI/HUD/SL_HUD.h"
#include <iostream>

#include "winsock.h"

USquadLeaderGameInstance::USquadLeaderGameInstance() {
    //When the object is constructed, Get the HTTP module
    Http = &FHttpModule::Get();

    /// -> deprecated method to find Local IPAdress
    // Init WinSock
    WSADATA wsa_Data;
    int wsa_ReturnCode = WSAStartup(0x101, &wsa_Data);

    // Get the local hostname
    char szHostName[255];
    gethostname(szHostName, 255);
    struct hostent* host_entry;
    host_entry = gethostbyname(szHostName);
    LocalIPAdress = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
    WSACleanup();
}


void USquadLeaderGameInstance::Shutdown()
{
    // when closing
    if (OnlineStatus) {
        HttpCallChangeConnectedStatus(0);  // notify server of the deconnexion
    }
    UserData.Save(UserDataFilename);  // save data
}

void USquadLeaderGameInstance::OnStart()
{
    Super::OnStart();
    UserData.LoadOrCreate(UserDataFilename);
    if (!LocalIPAdress.IsEmpty()) {
        UserData.IpAdress = LocalIPAdress;
    }
    HttpCallPing();
}


void USquadLeaderGameInstance::LaunchGame()
{
    if (OnlineStatus) {
        HttpCallCreateNewGame();
        HttpCallChangeConnectedStatus(2); // notify that the client is joining a new game
    }
    // GetFirstGamePlayer()->ConsoleCommand("open HUB_Level?listen", true);
    GetFirstGamePlayer()->ConsoleCommand("open Factory_V1?listen", true);
}

void USquadLeaderGameInstance::SetGameParamToDefault()
{
    if (auto GM = GetWorld()->GetAuthGameMode<ASLMainMenuGameModeBase>(); GM)
        GameParam = GM->DefaultGameParam;
}

void USquadLeaderGameInstance::SetGameParamToRandom()
{
    if (auto GM = GetWorld()->GetAuthGameMode<ASLMainMenuGameModeBase>(); GM) {
        GameParam.GetDefaultObject()->RandomiseParam(GM->MinGameParam.GetDefaultObject(), GM->MaxGameParam.GetDefaultObject());
    }
}

void USquadLeaderGameInstance::JoinGame(FString IPAdress)
{
    GetFirstGamePlayer()->ConsoleCommand("open " + IPAdress, true);
}

bool USquadLeaderGameInstance::UpdateNetworkStatus(const int MatchResult, float GameDuration, int XP, AKillStats* KillData)
{
    if (OnlineStatus) {
        // first do some process and save it in UserData
        if (MatchResult == 1) {
            UserData.NbVictory++;
        }
        else if (MatchResult == -1) {
            UserData.NbLoss++;
        }
        else return false;  // error in the entry data

        // add kill data
        UserData.NbKillIA += KillData->NbKillAI;
        UserData.NbKillPlayer += KillData->NbKillPlayer;
        UserData.NbDeathIA += KillData->NbDeathByAI;
        UserData.NbDeathPlayer += KillData->NbDeathByPlayer;

        // update GameDuration
        UserData.PlayTime += GameDuration;

        // update score
        UserData.Score = (UserData.Score * (UserData.NbVictory + UserData.NbLoss - 1) + (XP / 100)) / (UserData.NbVictory + UserData.NbLoss);

        HttpCallUpdatePlayerAfterGame();  // send the changed data after the match
        UserData.Save(UserDataFilename);  // take some time to save the data locally
        return true;
    }
    return false;
}

void USquadLeaderGameInstance::NoConnexionComportment() {
    OnlineStatus = false;

    FTimerHandle timerRetryServerConnexion;
    GetWorld()->GetTimerManager().SetTimer(timerRetryServerConnexion, this,
        &USquadLeaderGameInstance::HttpCallPing, 10.f);  // retry later
}


void USquadLeaderGameInstance::ProfileInfo()
{
    TMap<FString, FString> statsIn;
    statsIn.Add("Player Id", UserData.Id);
    statsIn.Add("Name", UserData.Name);
    statsIn.Add("IP Adress", UserData.IpAdress);
    statsIn.Add("Number of killed AI", FString::FromInt(UserData.NbKillIA));
    statsIn.Add("Number of killed players", FString::FromInt(UserData.NbKillPlayer));
    statsIn.Add("Number of deaths per AI", FString::FromInt(UserData.NbDeathIA));
    statsIn.Add("Number of deaths per players", FString::FromInt(UserData.NbDeathPlayer));
    statsIn.Add("Number of wins", FString::FromInt(UserData.NbVictory));
    statsIn.Add("Number of defeats", FString::FromInt(UserData.NbLoss));
    
    float WinRate;
    if (UserData.NbLoss == 0) {
        WinRate = UserData.NbVictory;
    }
    else {
        WinRate = static_cast<float>(UserData.NbVictory) / UserData.NbLoss;
    }
    statsIn.Add("Win rate", FString::SanitizeFloat(WinRate));
    statsIn.Add("Score", FString::FromInt(UserData.Score));
    statsIn.Add("Playtime", FString::FromInt(UserData.PlayTime));
    

    if (auto PC = GetPrimaryPlayerController(); PC)
    {
        if (auto HUD = PC->GetHUD<IStatInfoInterface>(); HUD)
        {
            HUD->OnStatsInfoCleanOrder();
            HUD->OnStatsInfoReceived(statsIn);
        }
    }

    
}


void USquadLeaderGameInstance::HttpCallPing()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedPing);
    //This is the url on which to process the request
    Request->SetURL(BaseServerDataAdress + "ping/");
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallCreateUser()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedCreateUser);
    //This is the url on which to process the request
    Request->SetURL(BaseServerDataAdress);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    Request->SetHeader("Content-Type", TEXT("application/json"));
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallConnectUser()
{
    FString content = FString::Printf(TEXT("guestToken=%s"),
        *FGenericPlatformHttp::UrlEncode(UserData.GuestToken));

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedConnectUser);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/sessions/create");
    Request->SetContentAsString(content);
    Request->SetVerb("POST");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallSendSyncData()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDoNothing);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/?name=" + UserData.Name + "&ipAdress=" + UserData.IpAdress + "&isInGame=" + FString::FromInt(1));  // isInGame = 1 for presence
    Request->SetVerb("PATCH");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallReceiveSyncData()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseReceivedReceiveSync);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id);
    Request->SetVerb("GET");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallCreateNewGame()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseCreateNewGame);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/Games/");
    Request->SetVerb("POST");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallSetUpNewGame()
{
    if (GameID != "") {
        const FString space = FString(" ");
        const FString changeCaracter = FString("_");

        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
        Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDoNothing);
        Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
        FString authHeader = FString("Bearer ") + AuthToken;
        Request->SetHeader("Authorization", authHeader);
        Request->SetURL(BaseServerDataAdress + UserData.Id + "/Games/" + GameID +
            "?name=" + GameParam.GetDefaultObject()->Name.Replace(*space, *changeCaracter) +
            "&levelTarget=" + FString::FromInt(GameParam.GetDefaultObject()->LevelTarget) +
            "&levelRange=" + FString::FromInt(GameParam.GetDefaultObject()->LevelRange) +
            "&nbAIBasic=" + FString::FromInt(GameParam.GetDefaultObject()->NbAIBasicAssault + GameParam.GetDefaultObject()->NbAIBasicHeavy) +
            "&levelAIBasic=" + FString::FromInt(GameParam.GetDefaultObject()->LevelAIBasic) +
            "&nbAISquad=" + FString::FromInt(GameParam.GetDefaultObject()->StartingNbAISquad) +
            "&levelAISquad=" + FString::FromInt(GameParam.GetDefaultObject()->LevelAISquad) +
            "&nbTicket=" + FString::FromInt(GameParam.GetDefaultObject()->NbTickets) +
            "&weather=" + FString::FromInt(GameParam.GetDefaultObject()->Weather) +
            "&respawnDuration=" + FString::FromInt(GameParam.GetDefaultObject()->RespawnDuration));  // + friends
        Request->SetVerb("PATCH");
        Request->ProcessRequest();
    }
}

void USquadLeaderGameInstance::HttpCallAllowFriendForGame()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDoNothing);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/Games/" + GameID + "/allowFriend/");
    Request->SetVerb("PUT");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallDeleteGame()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDeleteGame);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/Games/" + GameID);
    Request->SetVerb("DELETE");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallChangeConnectedStatus(int status)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDoNothing);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id + "/isInGame?isInGame=" + FString::FromInt(status));
    Request->SetVerb("PUT");
    Request->ProcessRequest();
}

void USquadLeaderGameInstance::HttpCallUpdatePlayerAfterGame()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &USquadLeaderGameInstance::OnResponseDoNothing);
    Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
    FString authHeader = FString("Bearer ") + AuthToken;
    Request->SetHeader("Authorization", authHeader);
    Request->SetURL(BaseServerDataAdress + UserData.Id +
        "/?nbKillIA=" + FString::FromInt(UserData.NbKillIA) +
        "&nbKillPlayer=" + FString::FromInt(UserData.NbKillPlayer) +
        "&nbDeathIA=" + FString::FromInt(UserData.NbDeathIA) +
        "&nbDeathPlayer=" + FString::FromInt(UserData.NbDeathPlayer) +
        "&nbVictory=" + FString::FromInt(UserData.NbVictory) +
        "&nbLoss=" + FString::FromInt(UserData.NbLoss) +
        "&score=" + FString::FromInt(UserData.Score) +
        "&playTime=" + FString::FromInt(UserData.PlayTime) + 
        "&isInGame=" + FString::FromInt(1));
    Request->SetVerb("PATCH");
    Request->ProcessRequest();
}


void USquadLeaderGameInstance::OnResponseReceivedPing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // able to ping the data server, continu the sync with the server
        OnlineStatus = true;
        // check if the player need an account
        if (UserData.Id == "") {
            // create an account and connect the player
            HttpCallCreateUser();
        }
        else {
            // connect the user
            HttpCallConnectUser();
        }
    }
    else {  // no connection to the data server
        NoConnexionComportment();  // arrange offline system
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
        HttpCallConnectUser();
    }
    else {  // no connection to the data server
        NoConnexionComportment();  // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseReceivedConnectUser(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (bWasSuccessful) {
        // save the identification token sent by the server, and sync the remaining data with the server
        AuthToken = Response->GetContentAsString();

        // change the name, the IPAdress and the IsInGame status on server, and download scores
        HttpCallSendSyncData();
        HttpCallReceiveSyncData();
    }
    else {  // connection problems
        NoConnexionComportment();  // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseDoNothing(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (!bWasSuccessful) {  // no connection to the data server
        NoConnexionComportment();  // arrange offline system
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
        NoConnexionComportment();  // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseCreateNewGame(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful) {
        // save imported data
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TJsonReader<TCHAR>>::Create(Response->GetContentAsString());
        if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
            //Get the value of the json object by field name
            GameID = JsonObject->GetStringField("id");
        }
        HttpCallSetUpNewGame();
        if (GameParam.GetDefaultObject()->FriendOnly) {
            HttpCallAllowFriendForGame();
        }
    }
    else {  // no connection to the data server
        NoConnexionComportment();  // arrange offline system
    }
}

void USquadLeaderGameInstance::OnResponseDeleteGame(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful) {
        GameID = "";
    }
    else {  // no connection to the data server or problems
        // TODO: retry later
        NoConnexionComportment();
    }
}
