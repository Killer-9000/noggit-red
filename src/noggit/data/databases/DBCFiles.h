#pragma once

#include <noggit/DBCFile.h>

#include <ClientData.hpp>
#include <glm/glm.hpp>

// This could get slightly rediculous, so might wanna seperate out.
// Filenames also got turned into ID's after the great conversion of legion,
//   so might wanna return a filekey.

namespace AreaDB
{
  class IAreaDB : public DBCFile
  {
  public:
    IAreaDB() : DBCFile() { }

    virtual uint32_t GetAreaID() = 0;
    virtual uint32_t GetContinentID() = 0;
    virtual uint32_t GetParentID() = 0;
    virtual uint32_t GetFlags() = 0;
    virtual std::string_view GetName() = 0;
  };

	class AreaDB335 : public IAreaDB
	{
	public:
		AreaDB335() : IAreaDB() { }

    virtual uint32_t GetAreaID() override;
    virtual uint32_t GetContinentID() override;
    virtual uint32_t GetParentID() override;
    virtual uint32_t GetFlags() override;
    virtual std::string_view GetName() override;
	};

  class AreaDB623 : public IAreaDB
  {
  public:
    AreaDB623() : IAreaDB() { }

    virtual uint32_t GetAreaID() override;
    virtual uint32_t GetContinentID() override;
    virtual uint32_t GetParentID() override;
    virtual uint32_t GetFlags() override;
    virtual std::string_view GetName() override;
  };
}

namespace MapDB
{
  class IMapDB : public DBCFile
  {
  public:
    IMapDB() : DBCFile() { }

    virtual uint32_t GetMapID() = 0;
    virtual std::string_view GetInternalName() = 0;
    virtual uint32_t GetAreaType() = 0;
    virtual bool GetIsBattleground() = 0;
    virtual std::string_view GetName() = 0;
  };

  class MapDB335 : public IMapDB
  {
  public:
    MapDB335() : IMapDB() { }

    virtual uint32_t GetMapID() override;
    virtual std::string_view GetInternalName() override;
    virtual uint32_t GetAreaType() override;
    virtual bool GetIsBattleground() override;
    virtual std::string_view GetName() override;
  };

  class MapDB623 : public IMapDB
  {
  public:
    MapDB623() : IMapDB() { }

    virtual uint32_t GetMapID() override;
    virtual std::string_view GetInternalName() override;
    virtual uint32_t GetAreaType() override;
    virtual bool GetIsBattleground() override;
    virtual std::string_view GetName() override;
  };
}

namespace LoadingScreensDB
{
  class ILoadingScreensDB : public DBCFile
  {
  public:
    ILoadingScreensDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual std::string_view GetName() = 0;
    virtual std::string_view GetPath() = 0;
  };

  class LoadingScreensDB335 : public ILoadingScreensDB
  {
  public:
    LoadingScreensDB335() : ILoadingScreensDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetName() override;
    virtual std::string_view GetPath() override;
  };

  class LoadingScreensDB623 : public ILoadingScreensDB
  {
  public:
    LoadingScreensDB623() : ILoadingScreensDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetName() override;
    virtual std::string_view GetPath() override;
  };
}

namespace LightDB
{
  class ILightDB : public DBCFile
  {
  public:
    ILightDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual uint32_t GetMapID() = 0;
    virtual glm::vec3 GetPosition() = 0;
    virtual float GetPositionX() = 0;
    virtual float GetPositionY() = 0;
    virtual float GetPositionZ() = 0;
    virtual float GetRadiusInner() = 0;
    virtual float GetRadiusOuter() = 0;
    virtual uint32_t* GetDataIDs() = 0;
  };

  class LightDB335 : public ILightDB
  {
  public:
    LightDB335() : ILightDB() { }

    virtual uint32_t GetID() override;
    virtual uint32_t GetMapID() override;
    virtual glm::vec3 GetPosition() override;
    virtual float GetPositionX() override;
    virtual float GetPositionY() override;
    virtual float GetPositionZ() override;
    virtual float GetRadiusInner() override;
    virtual float GetRadiusOuter() override;
    virtual uint32_t* GetDataIDs() override;
  };

  class LightDB623 : public ILightDB
  {
  public:
    LightDB623() : ILightDB() { }

    virtual uint32_t GetID() override;
    virtual uint32_t GetMapID() override;
    virtual glm::vec3 GetPosition() override;
    virtual float GetPositionX() override;
    virtual float GetPositionY() override;
    virtual float GetPositionZ() override;
    virtual float GetRadiusInner() override;
    virtual float GetRadiusOuter() override;
    virtual uint32_t* GetDataIDs() override;
  };
}

namespace LightParamsDB
{
  class ILightParamsDB : public DBCFile
  {
  public:
    ILightParamsDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual bool GetHighlightSky() = 0;
    virtual uint32_t GetSkyboxID() = 0;
    virtual uint32_t GetCloudType() = 0;
    virtual float GetGlow() = 0;
    virtual float GetWaterShallowAlpha() = 0;
    virtual float GetWaterDeepAlpha() = 0;
    virtual float GetOceanShallowAlpha() = 0;
    virtual float GetOceanDeepAlpha() = 0;
    virtual uint32_t GetFlags() = 0;
  };

  class LightParamsDB335 : public ILightParamsDB
  {
  public:
    LightParamsDB335() : ILightParamsDB() { }

    virtual uint32_t GetID() override;
    virtual bool GetHighlightSky() override;
    virtual uint32_t GetSkyboxID() override;
    virtual uint32_t GetCloudType() override;
    virtual float GetGlow() override;
    virtual float GetWaterShallowAlpha() override;
    virtual float GetWaterDeepAlpha() override;
    virtual float GetOceanShallowAlpha() override;
    virtual float GetOceanDeepAlpha() override;
    virtual uint32_t GetFlags() override;
  };

  class LightParamsDB623 : public ILightParamsDB
  {
  public:
    LightParamsDB623() : ILightParamsDB() { }

    virtual uint32_t GetID() override;
    virtual bool GetHighlightSky() override;
    virtual uint32_t GetSkyboxID() override;
    virtual uint32_t GetCloudType() override;
    virtual float GetGlow() override;
    virtual float GetWaterShallowAlpha() override;
    virtual float GetWaterDeepAlpha() override;
    virtual float GetOceanShallowAlpha() override;
    virtual float GetOceanDeepAlpha() override;
    virtual uint32_t GetFlags() override;
  };
}

namespace LightSkyboxDB
{
  class ILightSkyboxDB : public DBCFile
  {
  public:
    ILightSkyboxDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual std::string_view GetFilename() = 0;
    virtual uint32_t GetFlags() = 0;
  };

  class LightSkyboxDB335 : public ILightSkyboxDB
  {
  public:
    LightSkyboxDB335() : ILightSkyboxDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetFilename() override;
    virtual uint32_t GetFlags() override;
  };

  class LightSkyboxDB623 : public ILightSkyboxDB
  {
  public:
    LightSkyboxDB623() : ILightSkyboxDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetFilename() override;
    virtual uint32_t GetFlags() override;
  };
}

namespace LightDataDB
{
  class ILightDataDB : public DBCFile
  {
  public:
    ILightDataDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
  };

  class LightDataDB335 : public ILightDataDB
  {
  public:
    LightDataDB335() : ILightDataDB() { }

    virtual uint32_t GetID() override;
  };

  class LightDataDB623 : public ILightDataDB
  {
  public:
    LightDataDB623() : ILightDataDB() { }

    virtual uint32_t GetID() override;
  };
}

namespace GroundEffectTextureDB
{
  class IGroundEffectTextureDB : public DBCFile
  {
  public:
    IGroundEffectTextureDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual uint32_t* GetDoodads() = 0;
    virtual uint32_t* GetWeights() = 0;
    virtual uint32_t GetAmount() = 0;
    virtual uint32_t GetTerrainType() = 0;
  };

  class GroundEffectTextureDB335 : public IGroundEffectTextureDB
  {
  public:
    GroundEffectTextureDB335() : IGroundEffectTextureDB() { }

    virtual uint32_t GetID() override;
    virtual uint32_t* GetDoodads() override;
    virtual uint32_t* GetWeights() override;
    virtual uint32_t GetAmount() override;
    virtual uint32_t GetTerrainType() override;
  };

  class GroundEffectTextureDB623 : public IGroundEffectTextureDB
  {
  public:
    GroundEffectTextureDB623() : IGroundEffectTextureDB() { }

    virtual uint32_t GetID() override;
    virtual uint32_t* GetDoodads() override;
    virtual uint32_t* GetWeights() override;
    virtual uint32_t GetAmount() override;
    virtual uint32_t GetTerrainType() override;
  };
}

namespace GroundEffectDoodadDB
{
  class IGroundEffectDoodadDB : public DBCFile
  {
  public:
    IGroundEffectDoodadDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual std::string_view GetFilename() = 0;
    virtual uint32_t GetFlags() = 0;
  };

  class GroundEffectDoodadDB335 : public IGroundEffectDoodadDB
  {
  public:
    GroundEffectDoodadDB335() : IGroundEffectDoodadDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetFilename() override;
    virtual uint32_t GetFlags() override;
  };

  class GroundEffectDoodadDB623 : public IGroundEffectDoodadDB
  {
  public:
    GroundEffectDoodadDB623() : IGroundEffectDoodadDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetFilename() override;
    virtual uint32_t GetFlags() override;
  };
}

namespace LiquidTypeDB
{
  class ILiquidTypeDB : public DBCFile
  {
  public:
    ILiquidTypeDB() : DBCFile() { }

    virtual uint32_t GetID() = 0;
    virtual std::string_view GetName() = 0;
    virtual uint32_t GetType() = 0;
    virtual uint32_t GetShaderType() = 0;
    virtual std::string_view* GetTextureFilenames() = 0;
    virtual uint32_t GetTextureTilesPerBlock() = 0;
    virtual uint32_t GetRotation() = 0;
    virtual uint32_t GetAnimationX() = 0;
    virtual uint32_t GetAnimationY() = 0;
  };

  class LiquidTypeDB335 : public ILiquidTypeDB
  {
  public:
    LiquidTypeDB335() : ILiquidTypeDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetName() override;
    virtual uint32_t GetType() override;
    virtual uint32_t GetShaderType() override;
    virtual std::string_view* GetTextureFilenames() override;
    virtual uint32_t GetTextureTilesPerBlock() override;
    virtual uint32_t GetRotation() override;
    virtual uint32_t GetAnimationX() override;
    virtual uint32_t GetAnimationY() override;
  };

  class LiquidTypeDB623 : public ILiquidTypeDB
  {
  public:
    LiquidTypeDB623() : ILiquidTypeDB() { }

    virtual uint32_t GetID() override;
    virtual std::string_view GetName() override;
    virtual uint32_t GetType() override;
    virtual uint32_t GetShaderType() override;
    virtual std::string_view* GetTextureFilenames() override;
    virtual uint32_t GetTextureTilesPerBlock() override;
    virtual uint32_t GetRotation() override;
    virtual uint32_t GetAnimationX() override;
    virtual uint32_t GetAnimationY() override;
  };
}

void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData);

extern std::unique_ptr<AreaDB::IAreaDB> gAreaDB;
extern std::unique_ptr<MapDB::IMapDB> gMapDB;
extern std::unique_ptr<LoadingScreensDB::ILoadingScreensDB> gLoadingScreensDB;
extern std::unique_ptr<LightDB::ILightDB> gLightDB;
extern std::unique_ptr<LightParamsDB::ILightParamsDB> gLightParamsDB;
extern std::unique_ptr<LightSkyboxDB::ILightSkyboxDB> gLightSkyboxDB;

// These get combined into LightData after MOP
// Thinking of wrapping the seperate DBC's when using a version before MOP, since using functions.
extern std::unique_ptr<LightDataDB::ILightDataDB> gLightDataDB;
//extern std::unique_ptr<LightIntBandDB::ILightIntBandDB> gLightIntBandDB;
//extern std::unique_ptr<LightFloatBandDB::ILightFloatBandDB> gLightFloatBandDB;

extern std::unique_ptr<GroundEffectDoodadDB::IGroundEffectDoodadDB> gGroundEffectDoodadDB;
extern std::unique_ptr<GroundEffectTextureDB::IGroundEffectTextureDB> gGroundEffectTextureDB;
extern std::unique_ptr<LiquidTypeDB::ILiquidTypeDB> gLiquidTypeDB;
