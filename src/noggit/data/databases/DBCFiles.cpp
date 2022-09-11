//#include "DBCFiles.h"
//
//std::unique_ptr<AreaDB::IAreaDB> gAreaDB;
//std::unique_ptr<MapDB::IMapDB> gMapDB;
//std::unique_ptr<LoadingScreensDB::ILoadingScreensDB> gLoadingScreensDB;
//std::unique_ptr<LightDB::ILightDB> gLightDB;
//std::unique_ptr<LightParamsDB::ILightParamsDB> gLightParamsDB;
//std::unique_ptr<LightSkyboxDB::ILightSkyboxDB> gLightSkyboxDB;
//std::unique_ptr<LightDataDB::ILightDataDB> gLightDataDB;
//std::unique_ptr<GroundEffectDoodadDB::IGroundEffectDoodadDB> gGroundEffectDoodadDB;
//std::unique_ptr<GroundEffectTextureDB::IGroundEffectTextureDB> gGroundEffectTextureDB;
//std::unique_ptr<LiquidTypeDB::ILiquidTypeDB> gLiquidTypeDB;
//
//void OpenDBs(std::shared_ptr<BlizzardArchive::ClientData> clientData)
//{
//	switch (clientData->version())
//	{
//  case BlizzardArchive::ClientVersion::VANILLA:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::TBC:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::WOTLK:
//    gAreaDB = std::make_unique<AreaDB::AreaDB335>("DBFilesClient\\AreaTable.dbc");
//    gMapDB = std::make_unique<MapDB::MapDB335>("DBFilesClient\\Map.dbc");
//    gLoadingScreensDB = std::make_unique<LoadingScreensDB::LoadingScreensDB335>("DBFilesClient\\LoadingScreens.dbc");
//    gLightDB = std::make_unique<LightDB::LightDB335>("DBFilesClient\\Light.dbc");
//    gLightParamsDB = std::make_unique<LightParamsDB::LightParamsDB335>("DBFilesClient\\LightParams.dbc");
//    gLightSkyboxDB = std::make_unique<LightSkyboxDB::LightSkyboxDB335>("DBFilesClient\\LightSkybox.dbc");
//    gLightDataDB = std::make_unique<LightDataDB::LightDataDB335>("DBFilesClient\\LightIntBand.dbc", "DBFilesClient\\LightFloatBand.dbc");
//    gGroundEffectDoodadDB = std::make_unique<GroundEffectDoodadDB::GroundEffectDoodadDB335>("DBFilesClient\\GroundEffectDoodad.dbc");
//    gGroundEffectTextureDB = std::make_unique<GroundEffectTextureDB::GroundEffectTextureDB335>("DBFilesClient\\GroundEffectTexture.dbc");
//    gLiquidTypeDB = std::make_unique<LiquidTypeDB::LiquidTypeDB335>("DBFilesClient\\LiquidType.dbc");
//    break;
//  case BlizzardArchive::ClientVersion::CATA:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::MOP:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::WOD:
//    gAreaDB = std::make_unique<AreaDB::AreaDB623>("DBFilesClient\\AreaTable.db2");
//    gMapDB = std::make_unique<MapDB::MapDB623>("DBFilesClient\\Map.db2");
//    gLoadingScreensDB = std::make_unique<LoadingScreensDB::LoadingScreensDB623>("DBFilesClient\\LoadingScreens.db2");
//    gLightDB = std::make_unique<LightDB::LightDB623>("DBFilesClient\\Light.db2");
//    gLightParamsDB = std::make_unique<LightParamsDB::LightParamsDB623>("DBFilesClient\\LightParams.db2");
//    gLightSkyboxDB = std::make_unique<LightSkyboxDB::LightSkyboxDB623>("DBFilesClient\\LightSkybox.db2");
//    gLightDataDB = std::make_unique<LightDataDB::LightDataDB623>("DBFilesClient\\LightData.db2");
//    gGroundEffectDoodadDB = std::make_unique<GroundEffectDoodadDB::GroundEffectDoodadDB623>("DBFilesClient\\GroundEffectDoodad.db2");
//    gGroundEffectTextureDB = std::make_unique<GroundEffectTextureDB::GroundEffectTextureDB623>("DBFilesClient\\GroundEffectTexture.db2");
//    gLiquidTypeDB = std::make_unique<LiquidTypeDB::LiquidTypeDB623>("DBFilesClient\\LiquidType.db2");
//    break;
//  case BlizzardArchive::ClientVersion::LEGION:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::BFA:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::SHADOWLANDS:
//    throw std::exception("Not implemented this version yet.");
//    break;
//  case BlizzardArchive::ClientVersion::DRAGONFLIGHT:
//    throw std::exception("Not implemented this version yet.");
//    break;
//	default:
//    throw std::exception("Unknown version when opening DBCs.");
//	}
//}
