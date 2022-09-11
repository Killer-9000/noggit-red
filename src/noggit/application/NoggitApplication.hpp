// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_APPLICATION_HPP
#define NOGGIT_APPLICATION_HPP

#include <noggit/application/Configuration/NoggitApplicationConfiguration.hpp>
#include <noggit/ui/windows/projectSelection/NoggitProjectSelectionWindow.hpp>

#include <ClientData.hpp>

#include <memory>

namespace Noggit::Ui::Windows
{
    class NoggitProjectSelectionWindow;
}

namespace Noggit::Application {

    class NoggitApplication
    {
    public:
        static NoggitApplication* instance()
        {
            static NoggitApplication inst{};
            return &inst;
        }

        BlizzardArchive::ClientData* clientData() { return _client_data.get(); }
        void setClientData(std::shared_ptr<BlizzardArchive::ClientData> data) { _client_data = data; }

        void initalize(int argc, char* argv[]);
        std::shared_ptr<Noggit::Application::NoggitApplicationConfiguration> getConfiguration();
        static void terminationHandler();
    private:
        NoggitApplication() = default;

        std::shared_ptr<Noggit::Application::NoggitApplicationConfiguration> _application_configuration;
        std::unique_ptr<Noggit::Ui::Windows::NoggitProjectSelectionWindow> _project_selection_page;
        std::shared_ptr<BlizzardArchive::ClientData> _client_data;

    };

}

#endif //NOGGIT_APPLICATION_HPP