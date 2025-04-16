#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DynamicSky.generated.h"

class UPostProcessComponent;

UCLASS()
class WEATHERSYSTEM_API ADynamicSky : public AActor
{
	GENERATED_BODY()

public:
	ADynamicSky();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
#pragma region Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> DefaultRoot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDirectionalLightComponent> SunDirectionalLight;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDirectionalLightComponent> MoonDirectionalLight;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkyLightComponent> SkyLight;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UExponentialHeightFogComponent> ExponentialHeightFog;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPostProcessComponent> PostProcess;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> SkySphere;

#pragma endregion

	UPROPERTY(EditAnywhere, Category = "01-BasicSettings", meta=(UIMin ="0", UIMax ="24"))
	float TimeOfDay;

	// Decide when sun should come out
	UPROPERTY(EditAnywhere, Category = "01-BasicSettings", meta=(UIMin ="5", UIMax ="8"))
	float DawnTime;

	// Decide when sun should go down
	UPROPERTY(EditAnywhere, Category = "01-BasicSettings", meta=(UIMin ="16", UIMax ="20"))
	float DuskTime;

	UFUNCTION(Category = "01-BasicSettings")
	void HandleSunMoonRotation();

	bool IsDayTime() const;

	UFUNCTION(Category = "01-BasicSettings")
	void HandleVisibility() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "01-BasicSettings")
	TObjectPtr<UMaterialInterface> DynamicSkyMaterialTemplate;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SkySphereMaterialInstance;

	// Control quantity of light
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	float MoonLightIntensity;

	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	FLinearColor MoonLightColor;

	// Control the size of light disc
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	float MoonLightSourceAngle;

	// Bigger the value, colder the colors
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	float MoonLightTemperature;

	// Control the tint of sky atmosphere
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	FLinearColor NightTimeRayleighScattering;

	// Control intensity of light scattering in the level
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	float NightTimeMultiScatteringFactor;

	// Control the color of the horizon.
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	FColor GroundAlbedo;
	
	UFUNCTION(Category = "02-NightSettings")
	void HandleNightSettings() const;

	UFUNCTION()
	void HandleDynamicMaterial();
	
	UPROPERTY(EditAnywhere, Category = "02-NightSettings")
	bool bIsStarVisibleAtNight;
};
