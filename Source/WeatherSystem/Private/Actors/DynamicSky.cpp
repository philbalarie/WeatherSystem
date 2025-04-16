#include "Actors/DynamicSky.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"


ADynamicSky::ADynamicSky()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	SunDirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunDirectionalLight"));
	SunDirectionalLight->SetupAttachment(RootComponent);
	SunDirectionalLight->SetForwardShadingPriority(1);

	MoonDirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonDirectionalLight"));
	MoonDirectionalLight->SetupAttachment(RootComponent);
	MoonDirectionalLight->SetIntensity(1);
	MoonDirectionalLight->SetUseTemperature(true);

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(RootComponent);

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->SetRealTimeCapture(true);

	ExponentialHeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	ExponentialHeightFog->SetupAttachment(RootComponent);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(RootComponent);
	// Exposure set in blueprint to prevent auto exposure

	// Should use SM_SkySphere with a custom material to remove mesh visual
	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	SkySphere->SetupAttachment(RootComponent);

	// Initialize variables
	TimeOfDay = 9.f;
	DawnTime = 6;
	DuskTime = 18;
	MoonLightIntensity = 2;
	MoonLightColor = FLinearColor::White;
	MoonLightSourceAngle = 0;
	MoonLightTemperature = 8800;
	NightTimeRayleighScattering = FLinearColor(0.15, 0.16, 0.57);
	NightTimeMultiScatteringFactor = 0.5;
	GroundAlbedo = FColor(0.08, 0.15, 0.61);
	bIsStarVisibleAtNight = true;
	bIsMoonVisibleAtNight = true;
}

void ADynamicSky::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	HandleDynamicMaterial();
	HandleSunMoonRotation();
	HandleVisibility();
	HandleNightSettings();
}

void ADynamicSky::HandleSunMoonRotation()
{
	// Handle sun rotation between dawn and dusk
	// Convert Time of day in degrees for directional light
	const double SunRotationBasedOnTime = FMath::GetMappedRangeValueUnclamped(
		FVector2d(DawnTime, DuskTime), FVector2d(0, -180), TimeOfDay);
	SunDirectionalLight->SetWorldRotation(FRotator(SunRotationBasedOnTime, 0, 0));

	// Handle moon rotation after dusk
	const double DuskToMidnightRotationBasedOnTime = FMath::GetMappedRangeValueUnclamped(
		FVector2d(DuskTime + 0.3, 24), FVector2d(-180, -90), TimeOfDay);
	MoonDirectionalLight->SetWorldRotation(FRotator(DuskToMidnightRotationBasedOnTime, 0, 0));

	// Handle moon rotation before dawn
	const double MidnightToDawnBasedOnTime = FMath::GetMappedRangeValueUnclamped(
		FVector2d(0, DawnTime - 0.3), FVector2d(-90, 0), TimeOfDay);
	MoonDirectionalLight->SetWorldRotation(FRotator(MidnightToDawnBasedOnTime, 0, 0));
}

bool ADynamicSky::IsDayTime() const
{
	return TimeOfDay > DawnTime - 0.3 && TimeOfDay < DuskTime + 0.3;
}

void ADynamicSky::HandleVisibility() const
{
	SunDirectionalLight->SetVisibility(IsDayTime());
	MoonDirectionalLight->SetVisibility(!IsDayTime());
}

void ADynamicSky::HandleNightSettings() const
{
	if (!IsDayTime())
	{
		// Control quantity of light
		MoonDirectionalLight->SetIntensity(MoonLightIntensity);
		MoonDirectionalLight->SetLightColor(MoonLightColor);
		MoonDirectionalLight->SetLightSourceAngle(MoonLightSourceAngle);
		MoonDirectionalLight->SetTemperature(MoonLightTemperature);

		SkyAtmosphere->SetRayleighScattering(NightTimeRayleighScattering);
		SkyAtmosphere->SetMultiScatteringFactor(NightTimeMultiScatteringFactor);
		SkyAtmosphere->SetGroundAlbedo(GroundAlbedo);

		SkySphereMaterialInstance->SetScalarParameterValue(TEXT("IsMoonVisible"), bIsMoonVisibleAtNight ? 1 : 0);
		SkySphereMaterialInstance->SetScalarParameterValue(TEXT("IsMoonVisible"), bIsMoonVisibleAtNight ? 1 : 0);
	}

	else
	{
		// need to reset value, since SkyAtmosphere is also used in daytime
		SkyAtmosphere->SetRayleighScattering(FLinearColor(0.175287, 0.409607, 1.0));
		SkyAtmosphere->SetMultiScatteringFactor(1);

		SkySphereMaterialInstance->SetScalarParameterValue(TEXT("IsStarVisible"), 0);
		SkySphereMaterialInstance->SetScalarParameterValue(TEXT("IsMoonVisible"), 0);
	}
}

void ADynamicSky::HandleDynamicMaterial()
{
	if (DynamicSkyMaterialTemplate)
	{
		SkySphereMaterialInstance = UMaterialInstanceDynamic::Create(DynamicSkyMaterialTemplate, this);
		SkySphere->SetMaterial(0, SkySphereMaterialInstance);
	}
}
