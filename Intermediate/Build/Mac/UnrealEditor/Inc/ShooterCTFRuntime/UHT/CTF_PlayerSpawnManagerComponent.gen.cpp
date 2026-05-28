// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "CTF_PlayerSpawnManagerComponent.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeCTF_PlayerSpawnManagerComponent() {}

// ********** Begin Cross Module References ********************************************************
LYRAGAME_API UClass* Z_Construct_UClass_ULyraPlayerSpawningManagerComponent();
SHOOTERCTFRUNTIME_API UClass* Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent();
SHOOTERCTFRUNTIME_API UClass* Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_ShooterCTFRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UCTF_PlayerSpawnManagerComponent *****************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent;
UClass* UCTF_PlayerSpawnManagerComponent::GetPrivateStaticClass()
{
	using TClass = UCTF_PlayerSpawnManagerComponent;
	if (!Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("CTF_PlayerSpawnManagerComponent"),
			Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.InnerSingleton,
			StaticRegisterNativesUCTF_PlayerSpawnManagerComponent,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_NoRegister()
{
	return UCTF_PlayerSpawnManagerComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "HideCategories", "Trigger PhysicsVolume" },
		{ "IncludePath", "CTF_PlayerSpawnManagerComponent.h" },
		{ "ModuleRelativePath", "Public/CTF_PlayerSpawnManagerComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UCTF_PlayerSpawnManagerComponent constinit property declarations *********
// ********** End Class UCTF_PlayerSpawnManagerComponent constinit property declarations ***********
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCTF_PlayerSpawnManagerComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics
UObject* (*const Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ULyraPlayerSpawningManagerComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_ShooterCTFRuntime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::ClassParams = {
	&UCTF_PlayerSpawnManagerComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::Class_MetaDataParams)
};
void UCTF_PlayerSpawnManagerComponent::StaticRegisterNativesUCTF_PlayerSpawnManagerComponent()
{
}
UClass* Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent()
{
	if (!Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.OuterSingleton, Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent.OuterSingleton;
}
UCTF_PlayerSpawnManagerComponent::UCTF_PlayerSpawnManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UCTF_PlayerSpawnManagerComponent);
UCTF_PlayerSpawnManagerComponent::~UCTF_PlayerSpawnManagerComponent() {}
// ********** End Class UCTF_PlayerSpawnManagerComponent *******************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_CTF_PlayerSpawnManagerComponent_h__Script_ShooterCTFRuntime_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCTF_PlayerSpawnManagerComponent, UCTF_PlayerSpawnManagerComponent::StaticClass, TEXT("UCTF_PlayerSpawnManagerComponent"), &Z_Registration_Info_UClass_UCTF_PlayerSpawnManagerComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCTF_PlayerSpawnManagerComponent), 3341601521U) },
	};
}; // Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_CTF_PlayerSpawnManagerComponent_h__Script_ShooterCTFRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_CTF_PlayerSpawnManagerComponent_h__Script_ShooterCTFRuntime_251440010{
	TEXT("/Script/ShooterCTFRuntime"),
	Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_CTF_PlayerSpawnManagerComponent_h__Script_ShooterCTFRuntime_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_winterji_UnrealEngine_LyraStarterGame_Plugins_GameFeatures_ShooterCTF_Source_ShooterCTFRuntime_Public_CTF_PlayerSpawnManagerComponent_h__Script_ShooterCTFRuntime_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
