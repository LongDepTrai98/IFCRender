#ifndef _FILE_CONTEXT_FACTORY_HPP_
#define _FILE_CONTEXT_FACTORY_HPP_
#include <memory>
namespace dragon
{
	class IFileContext;
	class FileContextFactory
	{
	public:
		enum type
		{
			IFC,
		};
	public:
		static std::unique_ptr<IFileContext> create(type type);
	private:
		FileContextFactory() = default;
	};
}
#endif // !_FILE_CONTEXT_FACTORY_HPP_