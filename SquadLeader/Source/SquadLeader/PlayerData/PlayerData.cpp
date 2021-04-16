// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerData.h"
#include "Templates/SharedPointer.h"
#include "Serialization/JsonReader.h"

PlayerData::PlayerData()
{
}

PlayerData::~PlayerData()
{
}



void PlayerData::LoadOrCreate(FString FileName) {
    FString FileContent = ReadFile(FileName);
    if (FileContent == "0") {  // Read succesful, but file empty: create default file, and save it
        FileContent = FString("{\"id\":\"") + ""
            + "\",\"guestToken\":\"" + ""
            + "\", \"name\":\"" + "Bob"
            + "\", \"ipAdress\":\"" + ""
            + "\", \"nbKillIA\":\"" + FString::FromInt(0)
            + "\", \"nbKillPlayer\":\"" + FString::FromInt(0)
            + "\", \"nbDeathIA\":\"" + FString::FromInt(0)
            + "\", \"nbDeathPlayer\":\"" + FString::FromInt(0)
            + "\", \"nbVictory\":\"" + FString::FromInt(0)
            + "\", \"nbLoss\":\"" + FString::FromInt(0)
            + "\", \"score\":\"" + FString::FromInt(0)
            + "\", \"playTime\":\"" + FString::FromInt(0)
            + "\"}";
        WriteFile(FileName, FileContent);
    }
    
    // save the content in the file (imported from json)
    ImportDataFromJson(FileContent);
}

void PlayerData::Save(FString FileName)
{
    WriteFile(FileName, ExportDataInJson());
}


FString PlayerData::ReadFile(FString FileName) {
    FString file = FPaths::ProjectConfigDir();
    file.Append(FileName);

    // We will use this FileManager to deal with the file.
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString FileContent;
    // Always first check if the file that you want to manipulate exist.
    if (FileManager.FileExists(*file)) {
        // We use the LoadFileToString to load the file into
        if (FFileHelper::LoadFileToString(FileContent, *file, FFileHelper::EHashOptions::None)) {
            //UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Text From File: %s"), *FileContent);
            if (FileContent == FString{})
                return "0";
            return FileContent;
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
            return {};
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *file);
        return {};
    }
}

void PlayerData::WriteFile(FString FileName, FString Content) {
    FString file = FPaths::ProjectConfigDir();
    file.Append(FileName);

    // We will use this FileManager to deal with the file.
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString StringToWrite(Content);
    // Always first check if the file that you want to manipulate exist.
    if (FileManager.FileExists(*file))
    {
        // We use the LoadFileToString to load the file into
        if (FFileHelper::SaveStringToFile(StringToWrite, *file))
        {
            UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%s\" to the text file"), *StringToWrite);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *file);
    }
}

void PlayerData::ImportDataFromJson(FString Content)
{
    //Create a pointer to hold the json serialized data
    TSharedPtr<FJsonObject> JsonObject;
    //Create a reader pointer to read the json data
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TJsonReader<TCHAR>>::Create(Content);

    //Deserialize the json data given Reader and the actual object to deserialize
    if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
        //Get the value of the json object by field name
        Id = JsonObject->GetStringField("id");
        GuestToken = JsonObject->GetStringField("guestToken");
        Name = JsonObject->GetStringField("name");
        IpAdress = JsonObject->GetStringField("ipAdress");
        NbKillIA = static_cast<int>(JsonObject->GetNumberField("nbKillIA"));
        NbKillPlayer = static_cast<int>(JsonObject->GetNumberField("nbKillPlayer"));
        NbDeathIA = static_cast<int>(JsonObject->GetNumberField("nbDeathIA"));
        NbDeathPlayer = static_cast<int>(JsonObject->GetNumberField("nbDeathPlayer"));
        NbVictory = static_cast<int>(JsonObject->GetNumberField("nbVictory"));
        NbLoss = static_cast<int>(JsonObject->GetNumberField("nbLoss"));
        Score = static_cast<int>(JsonObject->GetNumberField("score"));
        PlayTime = static_cast<int>(JsonObject->GetNumberField("playTime"));
    }
}

FString PlayerData::ExportDataInJson()
{
    FString StringToWrite(FString("{\"id\":\"") + Id 
        + "\",\"guestToken\":\"" + GuestToken 
        + "\", \"name\":\"" + Name 
        + "\", \"ipAdress\":\"" + IpAdress
        + "\", \"nbKillIA\":\"" + FString::FromInt(NbKillIA)
        + "\", \"nbKillPlayer\":\"" + FString::FromInt(NbKillPlayer)
        + "\", \"nbDeathIA\":\"" + FString::FromInt(NbDeathIA)
        + "\", \"nbDeathPlayer\":\"" + FString::FromInt(NbDeathPlayer)
        + "\", \"nbVictory\":\"" + FString::FromInt(NbVictory)
        + "\", \"nbLoss\":\"" + FString::FromInt(NbLoss)
        + "\", \"score\":\"" + FString::FromInt(Score)
        + "\", \"playTime\":\"" + FString::FromInt(PlayTime)
        + "\"}");
    return StringToWrite;
}
