//Folder to contain all of the project related files
#pragma once

#include <noggit/application/Configuration/NoggitApplicationConfiguration.hpp>
#include <noggit/project/ApplicationProjectReader.h>
#include <noggit/project/ApplicationProjectWriter.h>

#include <blizzard-archive-library/include/ClientData.hpp>
#include <blizzard-archive-library/include/Exception.hpp>
#include <blizzard-database-library/include/BlizzardDatabase.h>

#include <QMessageBox>
#include <QIcon>

#include <glm/vec3.hpp>

#include <cassert>
#include <filesystem>
#include <memory>
#include <vector>

namespace Noggit::Project
{
#pragma warning( push )
#pragma warning( disable : 4715)
  struct ClientVersionFactory
  {
    static BlizzardArchive::ClientVersion mapToEnumVersion(std::string const& projectVersion)
    {
      if (projectVersion == std::string("Vanilla"))
        return BlizzardArchive::ClientVersion::VANILLA;
      else if (projectVersion == std::string("The Burning Crusade"))
        return BlizzardArchive::ClientVersion::TBC;
      else if (projectVersion == std::string("Wrath Of The Lich King"))
        return BlizzardArchive::ClientVersion::WOTLK;
      else if (projectVersion == std::string("Cataclysm"))
        return BlizzardArchive::ClientVersion::CATA;
      else if (projectVersion == std::string("Mists Of Pandaria"))
        return BlizzardArchive::ClientVersion::MOP;
      else if (projectVersion == std::string("Warlords Of Draenor"))
        return BlizzardArchive::ClientVersion::WOD;
      else if (projectVersion == std::string("Legion"))
        return BlizzardArchive::ClientVersion::LEGION;
      else if (projectVersion == std::string("Battle For Azeroth"))
        return BlizzardArchive::ClientVersion::BFA;
      else if (projectVersion == std::string("Shadowlands"))
        return BlizzardArchive::ClientVersion::SHADOWLANDS;
      else if (projectVersion == std::string("Dragonflight"))
        return BlizzardArchive::ClientVersion::DRAGONFLIGHT;

      assert(false);
    }

    static std::string MapToStringVersion(BlizzardArchive::ClientVersion const& projectVersion)
    {
      if (projectVersion == BlizzardArchive::ClientVersion::VANILLA)
        return std::string("Vanilla");
      else if (projectVersion == BlizzardArchive::ClientVersion::TBC)
        return std::string("The Burning Crusade");
      else if (projectVersion == BlizzardArchive::ClientVersion::WOTLK)
        return std::string("Wrath Of The Lich King");
      else if (projectVersion == BlizzardArchive::ClientVersion::CATA)
        return std::string("Cataclysm");
      else if (projectVersion == BlizzardArchive::ClientVersion::MOP)
        return std::string("Mists Of Pandaria");
      else if (projectVersion == BlizzardArchive::ClientVersion::WOD)
        return std::string("Warlords Of Draenor");
      else if (projectVersion == BlizzardArchive::ClientVersion::LEGION)
        return std::string("Legion");
      else if (projectVersion == BlizzardArchive::ClientVersion::BFA)
        return std::string("Battle For Azeroth");
      else if (projectVersion == BlizzardArchive::ClientVersion::SHADOWLANDS)
        return std::string("Shadowlands");
      else if (projectVersion == BlizzardArchive::ClientVersion::DRAGONFLIGHT)
        return std::string("Dragonflight");

      assert(false);
    }

    static QIcon GetIcon(std::string const& projectVersion)
    {
      return GetIcon(mapToEnumVersion(projectVersion));
    }

    static QIcon GetIcon(BlizzardArchive::ClientVersion const& projectVersion)
    {
      switch (projectVersion)
      {
      case BlizzardArchive::ClientVersion::VANILLA:
        return QIcon(":/icon-classic");
      case BlizzardArchive::ClientVersion::TBC:
        return QIcon(":/icon-burning");
      case BlizzardArchive::ClientVersion::WOTLK:
        return QIcon(":/icon-wrath");
      case BlizzardArchive::ClientVersion::CATA:
        return QIcon(":/icon-cata");
      case BlizzardArchive::ClientVersion::MOP:
        return QIcon(":/icon-panda");
      case BlizzardArchive::ClientVersion::WOD:
        return QIcon(":/icon-warlords");
      case BlizzardArchive::ClientVersion::LEGION:
        return QIcon(":/icon-legion");
      case BlizzardArchive::ClientVersion::BFA:
        return QIcon(":/icon-battle");
      case BlizzardArchive::ClientVersion::SHADOWLANDS:
        return QIcon(":/icon-shadow");
        //case BlizzardArchive::ClientVersion::DRAGONFLIGHT:
        //  return QIcon(":/icon-dragon");
      default:
        return QIcon(":/icon-classic");
      }
    }
  };
#pragma warning( pop )

  struct NoggitProjectBookmarkMap
  {
    int map_id;
    std::string name;
    glm::vec3 position;
    float camera_yaw;
    float camera_pitch;
  };

  struct NoggitProjectPinnedMap
  {
    int MapId;
    std::string MapName;
  };

  class NoggitProject
  {
    std::shared_ptr<ApplicationProjectWriter> _projectWriter;
  public:
    std::string ProjectPath;
    std::string ProjectName;
    std::string ClientPath;
    BlizzardArchive::ClientVersion projectVersion;
    std::vector<NoggitProjectPinnedMap> PinnedMaps;
    std::vector<NoggitProjectBookmarkMap> Bookmarks;
    std::shared_ptr<BlizzardDatabaseLib::BlizzardDatabase> ClientDatabase;
    std::shared_ptr<BlizzardArchive::ClientData> ClientData;

    NoggitProject()
    {
      PinnedMaps = std::vector<NoggitProjectPinnedMap>();
      Bookmarks = std::vector<NoggitProjectBookmarkMap>();
      _projectWriter = std::make_shared<ApplicationProjectWriter>();
    }

    void createBookmark(const NoggitProjectBookmarkMap& bookmark)
    {
      Bookmarks.push_back(bookmark);

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }

    void deleteBookmark() { }

    void pinMap(int map_id, const std::string& map_name)
    {
      auto pinnedMap = NoggitProjectPinnedMap();
      pinnedMap.MapName = map_name;
      pinnedMap.MapId = map_id;

      auto pinnedMapFound = std::find_if(std::begin(PinnedMaps), std::end(PinnedMaps),
                                         [&](Project::NoggitProjectPinnedMap pinnedMap)
                                         {
                                           return pinnedMap.MapId == map_id;
                                         });

      if (pinnedMapFound != std::end(PinnedMaps))
        return;

      PinnedMaps.push_back(pinnedMap);

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }

    void unpinMap(int mapId)
    {
      PinnedMaps.erase(std::remove_if(PinnedMaps.begin(), PinnedMaps.end(),
                                      [=](NoggitProjectPinnedMap pinnedMap)
                                      {
                                        return pinnedMap.MapId == mapId;
                                      }),
                       PinnedMaps.end());

      _projectWriter->saveProject(this, std::filesystem::path(ProjectPath));
    }
  };

  class ApplicationProject
  {
    std::shared_ptr<NoggitProject> _active_project;
    std::shared_ptr<Application::NoggitApplicationConfiguration> _configuration;
  public:
    ApplicationProject(std::shared_ptr<Application::NoggitApplicationConfiguration> configuration)
    {
      _active_project = nullptr;
      _configuration = configuration;
    }

    void createProject(std::filesystem::path const& project_path, std::filesystem::path const& client_path,
                       std::string const& client_version, std::string const& project_name)
    {
      if (!std::filesystem::exists(project_path))
        std::filesystem::create_directory(project_path);

      auto project = NoggitProject();
      project.ProjectName = project_name;
      project.projectVersion = ClientVersionFactory::mapToEnumVersion(client_version);
      project.ClientPath = client_path.generic_string();
      project.ProjectPath = project_path.generic_string();

      auto project_writer = ApplicationProjectWriter();
      project_writer.saveProject(&project, project_path);
    }

    std::shared_ptr<NoggitProject> loadProject(std::filesystem::path const& project_path)
    {
      ApplicationProjectReader project_reader{};
      auto project = project_reader.readProject(project_path);

      if(!project.has_value())
        return {};

      std::string dbd_file_directory = _configuration->ApplicationDatabaseDefinitionsPath;

      BlizzardDatabaseLib::Structures::Build client_build("3.3.5.12340");
      auto client_archive_locale = BlizzardArchive::Locale::AUTO;
      if (project->projectVersion == BlizzardArchive::ClientVersion::SHADOWLANDS)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("9.2.7.45161");
        client_archive_locale = BlizzardArchive::Locale::enUS;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::LEGION)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("7.3.5.26365");
        client_archive_locale = BlizzardArchive::Locale::enUS;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::WOD)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("6.2.3.20779");
        client_archive_locale = BlizzardArchive::Locale::enUS;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::MOP)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("5.4.8.18414");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::CATA)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("4.3.4.15595");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::WOTLK)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("3.3.5.12340");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::TBC)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("2.4.3.8606");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }
      else if (project->projectVersion == BlizzardArchive::ClientVersion::VANILLA)
      {
        client_build = BlizzardDatabaseLib::Structures::Build("1.12.1.5875");
        client_archive_locale = BlizzardArchive::Locale::AUTO;
      }
      else
      {
        QMessageBox::warning(nullptr, "Error", "The client does not match a supported version.");
        return {};
      }

      project->ClientDatabase = std::make_shared<BlizzardDatabaseLib::BlizzardDatabase>(dbd_file_directory, client_build);

      try
      {
        project->ClientData = std::make_shared<BlizzardArchive::ClientData>(
            project->ClientPath, project->projectVersion, client_archive_locale, project_path.generic_string());
      }
      catch (BlizzardArchive::Exceptions::Locale::LocaleNotFoundError&)
      {
        QMessageBox::critical(nullptr, "Error", "The client does not appear to be valid.");
        return {};
      }

      return std::make_shared<NoggitProject>(project.value());
    }
  };
}
