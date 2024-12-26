#pragma once

// Declare the log category 
CHESSCARD_API DECLARE_LOG_CATEGORY_EXTERN(LogCard, Log, All);
CHESSCARD_API DECLARE_LOG_CATEGORY_EXTERN(LogDebugCard, Log, All);
CHESSCARD_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplay, Log, All);
CHESSCARD_API DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);
CHESSCARD_API DECLARE_LOG_CATEGORY_EXTERN(LogData, Log, All);




// Log macros
#define DEBUG_ERROR(Format, ...) UE_LOG(LogDebugCard, Error, TEXT(Format), ##__VA_ARGS__)
#define DEBUG_WARNING(Format, ...) UE_LOG(LogDebugCard, Warning, TEXT(Format), ##__VA_ARGS__)
#define DEBUG_LOG(Format, ...) UE_LOG(LogDebugCard, Log, TEXT(Format), ##__VA_ARGS__)

#define DEBUG_ERROR_CATEGORY(Category, Format, ...) UE_LOG(Category, Error, TEXT(Format), ##__VA_ARGS__)
#define DEBUG_WARNING_CATEGORY(Category, Format, ...) UE_LOG(Category, Warning, TEXT(Format), ##__VA_ARGS__)
#define DEBUG_LOG_CATEGORY(Category, Format, ...) UE_LOG(Category, Log, TEXT(Format), ##__VA_ARGS__)

#define LOG_ERROR(Format, ...) UE_LOG(LogCard, Error, TEXT(Format), ##__VA_ARGS__)
#define LOG_WARNING(Format, ...) UE_LOG(LogCard, Warning, TEXT(Format), ##__VA_ARGS__)
#define LOG_LOG(Format, ...) UE_LOG(LogCard, Log, TEXT(Format), ##__VA_ARGS__)

// On-screen log macros
#define DEBUG_LOG_SCREEN(Key, DisplayTime, Color, Format, ...) if(GEngine){ \
GEngine->AddOnScreenDebugMessage(Key, DisplayTime, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__)); \
}

#define DEBUG_LOG_SCREEN_SIMPLE(Format, ...) if(GEngine){ \
GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__)); \
}