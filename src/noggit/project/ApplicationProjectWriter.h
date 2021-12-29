#ifndef NOGGIT_APPLICATION_PROJECT_WRITER_HPP
#define NOGGIT_APPLICATION_PROJECT_WRITER_HPP

#include <filesystem>

namespace Noggit::Project
{
	class NoggitProject;

	class ApplicationProjectWriter
	{
	public:
		ApplicationProjectWriter() = default;

		void SaveProject(NoggitProject* project, std::filesystem::path const& projectPath);
	};
}

#endif //NOGGIT_APPLICATION_PROJECT_WRITER_HPP