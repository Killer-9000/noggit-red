#pragma once
#include <filesystem>
#include <SQLiteCpp/SQLiteCpp.h>
#include <noggit/database/NoggitDatabaseCreator.h>

namespace Noggit::Database::Repositories
{
	struct MapEntry
	{
		int Id;
		std::string Directory;
		LocaleString Name;
		int InstanceType;
		int ExpansionId;
		int Flags;
		int PVP;
		int AreaTableId;
		LocaleString HordeMapDescription;
		LocaleString AllianceMapDescription;
		int LoadingScreenId;
		double MiniMapScale;
		int CorpseMapId;
		double Corpse_X;
		double Corpse_Y;
		int TimeDayOverride;
		int RaidOffset;
		int MaxPlayers;
	};

	class IRepository
	{

	};

	class IMapRepository : public IRepository
	{
	public:
		virtual int CreateLocaleString(LocaleString string) = 0;
		virtual void UpdateLocaleString(LocaleString string) = 0;
		virtual LocaleString GetLocaleString(int id) = 0;
		virtual std::vector<MapEntry> GetMapList() = 0;
		virtual std::string GetMapDirectory(int mapId) = 0;
		virtual bool DoesMapAlreadyExistWithName(std::string name) = 0;
		virtual void SaveMap(MapEntry entry) = 0;
		virtual MapEntry CreateMap(MapEntry entry) = 0;
		virtual MapEntry GetMapById(int id) = 0;
		virtual void DeleteMap(int id) = 0;
	};

	class WotlkMapRepository : public IMapRepository
	{
		std::filesystem::path _databasePath;
	public:
		WotlkMapRepository(const std::filesystem::path& database_path): _databasePath(database_path)
		{
		
		}

		int CreateLocaleString(LocaleString string) override
		{
			SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
			SQLite::Statement  updateLocaleString(db, "INSERT INTO _localeStrings(enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU, jaJP, ptPT, itIT, Unk1, Unk2, Unk3, Unk4, flags)"
												      "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )");

			updateLocaleString.bind(1, string[Locale::enUS]);
			updateLocaleString.bind(2, string[Locale::koKR]);
			updateLocaleString.bind(3, string[Locale::frFR]);
			updateLocaleString.bind(4, string[Locale::deDE]);
			updateLocaleString.bind(5, string[Locale::zhCN]);
			updateLocaleString.bind(6, string[Locale::zhTW]);
			updateLocaleString.bind(7, string[Locale::esES]);
			updateLocaleString.bind(8, string[Locale::esMX]);
			updateLocaleString.bind(9, string[Locale::ruRU]);
			updateLocaleString.bind(10, string[Locale::jaJP]);
			updateLocaleString.bind(11, string[Locale::ptPT]);
			updateLocaleString.bind(12, string[Locale::itIT]);
			updateLocaleString.bind(13, string[Locale::Unk1]);
			updateLocaleString.bind(14, string[Locale::Unk2]);
			updateLocaleString.bind(15, string[Locale::Unk3]);
			updateLocaleString.bind(16, string[Locale::Unk4]);
			updateLocaleString.bind(17, string.flags);
			updateLocaleString.exec();

			SQLite::Statement  getLastId(db, "SELECT last_insert_rowid()");
			if (getLastId.executeStep())
			{
				string.id = getLastId.getColumn(0).getInt();
				return string.id;
			}

			auto errorMessage = std::string(db.getErrorMsg());
		}

		void UpdateLocaleString(LocaleString string) override
		{
			SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
			SQLite::Statement  updateLocaleString(db, "UPDATE _localeStrings SET enUS = ? , koKR = ? , frFR = ? , deDE = ? ,"
												      "zhCN = ? , zhTW = ? , esES = ? , esMX = ? , ruRU = ? , jaJP = ? , "
												      "ptPT = ? , itIT = ? , Unk1 = ? , Unk2 = ? , Unk3 = ? , Unk4 = ? , flags = ? "
													  "WHERE id = ?");

			updateLocaleString.bind(1, string[Locale::enUS]);
			updateLocaleString.bind(2, string[Locale::koKR]);
			updateLocaleString.bind(3, string[Locale::frFR]);
			updateLocaleString.bind(4, string[Locale::deDE]);
			updateLocaleString.bind(5, string[Locale::zhCN]);
			updateLocaleString.bind(6, string[Locale::zhTW]);
			updateLocaleString.bind(7, string[Locale::esES]);
			updateLocaleString.bind(8, string[Locale::esMX]);
			updateLocaleString.bind(9, string[Locale::ruRU]);
			updateLocaleString.bind(10, string[Locale::jaJP]);
			updateLocaleString.bind(11, string[Locale::ptPT]);
			updateLocaleString.bind(12, string[Locale::itIT]);
			updateLocaleString.bind(13, string[Locale::Unk1]);
			updateLocaleString.bind(14, string[Locale::Unk2]);
			updateLocaleString.bind(15, string[Locale::Unk3]);
			updateLocaleString.bind(16, string[Locale::Unk4]);
			updateLocaleString.bind(17, string.flags);
			updateLocaleString.bind(18, string.id);

			updateLocaleString.exec();

			auto errorMessage = std::string(db.getErrorMsg());
		}

		LocaleString GetLocaleString(int id) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  selectLocaleStringQuery(db, "SELECT enUS, koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU,"
																   "jaJP, ptPT, itIT, Unk1, Unk2, Unk3,Unk4, flags "
																   "FROM _localeStrings WHERE id = ?");

			selectLocaleStringQuery.bind(1, id);

			auto mapList = std::vector<MapEntry>();
			if(selectLocaleStringQuery.executeStep())
			{
				auto localeString = LocaleString();
				localeString.id = id;
				localeString.values = std::map<std::string, std::string>();

				for (int localeIndex = 0; localeIndex < 16; localeIndex++)
				{
					auto localeValue = selectLocaleStringQuery.getColumn(localeIndex).getString();
					localeString.values.emplace(LocaleNames[localeIndex], localeValue);
				}

				localeString.flags = selectLocaleStringQuery.getColumn(16).getUInt();

				return localeString;
			}
		}

		std::vector<MapEntry> GetMapList() override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT map.ID, map.MapName_lang, map.Directory ,map.ExpansionID, map.InstanceType FROM map WHERE map.InstanceType < 5");

			auto mapList = std::vector<MapEntry>();
			while(query.executeStep())
			{
				auto mapEntry = MapEntry();
				mapEntry.Id = query.getColumn(0).getInt();
				mapEntry.Directory = query.getColumn(2).getString();
				mapEntry.ExpansionId = query.getColumn(3).getInt();
				mapEntry.InstanceType = query.getColumn(4).getInt();

				auto localeStringId = query.getColumn(1).getInt();
				mapEntry.Name = GetLocaleString(localeStringId);

				mapList.push_back(mapEntry);
			}
			return mapList;
		}

		std::string GetMapDirectory(int mapId) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Directory FROM map WHERE ID = ?; ");

			query.bind(1, mapId);

			if (query.executeStep())
			{
				return query.getColumn(0).getString();
			}	
		}

		bool DoesMapAlreadyExistWithName(std::string name) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT 1 FROM map WHERE Directory = ?; ");

			query.bind(1, name);

			if (query.executeStep())
			{
				return true;
			}

			return false;
		}

		void SaveMap(MapEntry entry) override
		{
			SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
			SQLite::Statement  updateQuery(db, "UPDATE Map SET Directory = ? , InstanceType = ? , Flags = ? , PVP = ? ,"
											   "AreaTableID = ? ,LoadingScreenID = ? , MinimapIconScale = ? , CorpseMapID = ? ,"
											   "Corpse_0 = ? , Corpse_1 = ? , TimeOfDayOverride = ? , ExpansionID = ? , RaidOffset = ? , MaxPlayers = ?"
											   "WHERE ID = ?");

			updateQuery.bind(1, entry.Directory);
			updateQuery.bind(2, entry.InstanceType);
			updateQuery.bind(3, entry.Flags);
			updateQuery.bind(4, entry.PVP);
			updateQuery.bind(5, entry.AreaTableId);
			updateQuery.bind(6, entry.LoadingScreenId);
			updateQuery.bind(7, entry.MiniMapScale);
			updateQuery.bind(8, entry.CorpseMapId);
			updateQuery.bind(9, entry.Corpse_X);
			updateQuery.bind(10, entry.Corpse_Y);
			updateQuery.bind(11, entry.TimeDayOverride);
			updateQuery.bind(12, entry.ExpansionId);
			updateQuery.bind(13, entry.RaidOffset);
			updateQuery.bind(14, entry.MaxPlayers);
			updateQuery.bind(15, entry.Id);

			UpdateLocaleString(entry.Name);
			UpdateLocaleString(entry.HordeMapDescription);
			UpdateLocaleString(entry.AllianceMapDescription);
		
			updateQuery.exec();
		}

		MapEntry CreateMap(MapEntry entry) override
		{
			auto nextRecordId = 0;
		
			SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
			SQLite::Statement  nextId(db, "SELECT MAX(ID) FROM Map");
			if (nextId.executeStep())
			{
				nextRecordId = nextId.getColumn(0).getInt();
			}

			nextId.reset();

			entry.Id = nextRecordId + 1;

			SQLite::Statement  updateQuery(db, "INSERT INTO Map (ID,Directory, InstanceType, Flags, PVP, MapName_lang, AreaTableID,"
											   "MapDescription0_lang, MapDescription1_lang, LoadingScreenID, MinimapIconScale, CorpseMapID,"
											   "Corpse_0, Corpse_1, TimeOfDayOverride, ExpansionID, RaidOffset, MaxPlayers)"
											   "VALUES(? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? ); ");

			auto nameId = CreateLocaleString(entry.Name);
			auto hordeDescId = CreateLocaleString(entry.HordeMapDescription);
			auto allianceDescId = CreateLocaleString(entry.AllianceMapDescription);

			updateQuery.bind(1, entry.Id);
			updateQuery.bind(2, entry.Directory);
			updateQuery.bind(3, entry.InstanceType);
			updateQuery.bind(4, entry.Flags);
			updateQuery.bind(5, entry.PVP);
			updateQuery.bind(6, nameId);
			updateQuery.bind(7, entry.AreaTableId);
			updateQuery.bind(8, hordeDescId);
			updateQuery.bind(9, allianceDescId);
			updateQuery.bind(10, entry.LoadingScreenId);
			updateQuery.bind(11, entry.MiniMapScale);
			updateQuery.bind(12, entry.CorpseMapId);
			updateQuery.bind(13, entry.Corpse_X);
			updateQuery.bind(14, entry.Corpse_Y);
			updateQuery.bind(15, entry.TimeDayOverride);
			updateQuery.bind(16, entry.ExpansionId);
			updateQuery.bind(17, entry.RaidOffset);
			updateQuery.bind(18, entry.MaxPlayers);
			updateQuery.exec();

			return entry;
		}

		MapEntry GetMapById(int id) override
		{
			SQLite::Database   db(_databasePath.generic_string());
			SQLite::Statement  query(db, "SELECT Directory, InstanceType, Flags, PVP, MapName_lang,AreaTableID, MapDescription0_lang,"
										 "MapDescription1_lang, LoadingScreenID,MinimapIconScale, CorpseMapID, Corpse_0, Corpse_1,"
										 "TimeOfDayOverride,ExpansionID, RaidOffset, MaxPlayers FROM Map WHERE ID = ? ");

			query.bind(1, id);

			if (query.executeStep())
			{
				auto mapEntry = MapEntry();
				mapEntry.Id = id;
				mapEntry.Directory = query.getColumn(0).getString();
				mapEntry.InstanceType = query.getColumn(1).getInt();
				mapEntry.Flags = query.getColumn(2).getInt();
				mapEntry.PVP = query.getColumn(3).getInt();
				auto mapLangLocaleStringId = query.getColumn(4).getInt();
				mapEntry.Name = GetLocaleString(mapLangLocaleStringId);
				mapEntry.AreaTableId = query.getColumn(5).getInt();
				auto mapHordeDescLocaleStringId = query.getColumn(6).getInt();
				mapEntry.HordeMapDescription = GetLocaleString(mapHordeDescLocaleStringId);
				auto mapAllianceDescLocaleStringId = query.getColumn(7).getInt();
				mapEntry.AllianceMapDescription = GetLocaleString(mapAllianceDescLocaleStringId);
				mapEntry.LoadingScreenId = query.getColumn(8).getInt();
				mapEntry.MiniMapScale = query.getColumn(9).getDouble();
				mapEntry.CorpseMapId = query.getColumn(10).getInt();
				mapEntry.Corpse_X = query.getColumn(11).getDouble();
				mapEntry.Corpse_Y = query.getColumn(12).getDouble();
				mapEntry.TimeDayOverride = query.getColumn(13).getInt();
				mapEntry.ExpansionId = query.getColumn(14).getInt();
				mapEntry.RaidOffset = query.getColumn(15).getInt();
				mapEntry.MaxPlayers = query.getColumn(16).getInt();

				return mapEntry;
			}
		} 

		void DeleteMap(int id) override
		{
			SQLite::Database   db(_databasePath.generic_string(), SQLite::OPEN_READWRITE);
			SQLite::Statement  getLocaleStrings(db, "SELECT MapName_lang, MapDescription0_lang, MapDescription1_lang FROM Map WHERE ID = ? ");
			getLocaleStrings.bind(1, id);

			auto localtStringsToRemove = std::vector<int>(3);
			if (getLocaleStrings.executeStep())
			{
				localtStringsToRemove.push_back(getLocaleStrings.getColumn(0).getInt());
				localtStringsToRemove.push_back(getLocaleStrings.getColumn(1).getInt());
				localtStringsToRemove.push_back(getLocaleStrings.getColumn(2).getInt());
			}

			SQLite::Statement  deleteLocaleStrings(db, "DELETE FROM _localeStrings WHERE ID in (?,?,?) ");
			deleteLocaleStrings.bind(1, localtStringsToRemove[0]);
			deleteLocaleStrings.bind(2, localtStringsToRemove[1]);
			deleteLocaleStrings.bind(3, localtStringsToRemove[2]);
			deleteLocaleStrings.exec();

			SQLite::Statement  deleteMap(db, "DELETE FROM Map WHERE ID = ? ");
			deleteMap.bind(1, id);
			deleteMap.exec();
		}
	};

	class ShadowlandsMapRepository : public IMapRepository
	{
		std::filesystem::path _databasePath;
	public:
		ShadowlandsMapRepository(const std::filesystem::path& database_path) : _databasePath(database_path)
		{

		}

		int CreateLocaleString(LocaleString string) override {};
		void UpdateLocaleString(LocaleString string) override {};
		LocaleString GetLocaleString(int id) override {};
		std::vector<MapEntry> GetMapList() override {};
		std::string GetMapDirectory(int mapId) override {};
		bool DoesMapAlreadyExistWithName(std::string name) override {};
		void SaveMap(MapEntry entry) override {};
		MapEntry CreateMap(MapEntry entry) override {};
		MapEntry GetMapById(int id) override {};
		void DeleteMap(int id) override {};
	};
}