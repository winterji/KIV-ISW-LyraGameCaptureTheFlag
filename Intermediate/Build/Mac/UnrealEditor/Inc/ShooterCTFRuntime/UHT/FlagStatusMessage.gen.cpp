// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Messages/FlagStatusMessage.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeFlagStatusMessage() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_APawn_NoRegister();
SHOOTERCTFRUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FLyraFlagStatusMessage();
UPackage* Z_Construct_UPackage__Script_ShooterCTFRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FLyraFlagStatusMessage ********************************************
struct Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FLyraFlagStatusMessage); }
	static inline consteval int16 GetStructAlignment() { return alignof(FLyraFlagStatusMessage); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Message indicating a Flag status change\n" },
#endif
		{ "ModuleRelativePath", "Public/Messages/FlagStatusMessage.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Message indicating a Flag status change" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Instigator_MetaData[] = {
		{ "Category", "Gameplay" },
		{ "ModuleRelativePath", "Public/Messages/FlagStatusMessage.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FlagTeamId_MetaData[] = {
		{ "Category", "Gameplay" },
		{ "ModuleRelativePath", "Public/Messages/FlagStatusMessage.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Pad_MetaData[] = {
		{ "Category", "Gameplay" },
		{ "ModuleRelativePath", "Public/Messages/FlagStatusMessage.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FLyraFlagStatusMessage constinit property declarations ************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Instigator;
	static const UECodeGen_Private::FIntPropertyParams NewProp_FlagTeamId;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Pad;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FLyraFlagStatusMessage constinit property declarations **************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FLyraFlagStatusMessage>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage;
class UScriptStruct* FLyraFlagStatusMessage::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FLyraFlagStatusMessage, (UObject*)Z_Construct_UPackage__Script_ShooterCTFRuntime(), TEXT("LyraFlagStatusMessage"));
	}
	return Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.OuterSingleton;
	}

// ********** Begin ScriptStruct FLyraFlagStatusMessage Property Definitions ***********************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_Instigator = { "Instigator", nullptr, (EPropertyFlags)0x0114000000000004, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FLyraFlagStatusMessage, Instigator), Z_Construct_UClass_APawn_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Instigator_MetaData), NewProp_Instigator_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_FlagTeamId = { "FlagTeamId", nullptr, (EPropertyFlags)0x0010000000000004, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FLyraFlagStatusMessage, FlagTeamId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FlagTeamId_MetaData), NewProp_FlagTeamId_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_Pad = { "Pad", nullptr, (EPropertyFlags)0x0114000000000004, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FLyraFlagStatusMessage, Pad), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Pad_MetaData), NewProp_Pad_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_Instigator,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_FlagTeamId,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewProp_Pad,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FLyraFlagStatusMessage Property Definitions *************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_ShooterCTFRuntime,
	nullptr,
	&NewStructOps,
	"LyraFlagStatusMessage",
	Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::PropPointers),
	sizeof(FLyraFlagStatusMessage),
	alignof(FLyraFlagStatusMessage),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FLyraFlagStatusMessage()
{
	if (!Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.InnerSingleton, Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage.InnerSingleton);
}
// ********** End ScriptStruct FLyraFlagStatusMessage **********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_Messages_FlagStatusMessage_h__Script_ShooterCTFRuntime_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FLyraFlagStatusMessage::StaticStruct, Z_Construct_UScriptStruct_FLyraFlagStatusMessage_Statics::NewStructOps, TEXT("LyraFlagStatusMessage"),&Z_Registration_Info_UScriptStruct_FLyraFlagStatusMessage, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FLyraFlagStatusMessage), 2316881332U) },
	};
}; // Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_Messages_FlagStatusMessage_h__Script_ShooterCTFRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_Messages_FlagStatusMessage_h__Script_ShooterCTFRuntime_3184272167{
	TEXT("/Script/ShooterCTFRuntime"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_Messages_FlagStatusMessage_h__Script_ShooterCTFRuntime_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_Messages_FlagStatusMessage_h__Script_ShooterCTFRuntime_Statics::ScriptStructInfo),
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
