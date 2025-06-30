#ifndef _IFILE_CONTEXT_HPP_
#define _IFILE_CONTEXT_HPP_
#include <string>
#include <memory>
namespace dragon
{
	class IGeometryCache; 
	class IFileContext
	{
	public: 
		virtual ~IFileContext() {};
	public: 
		virtual std::string getFileType() = 0; 
		virtual IGeometryCache* getGeometryCache() = 0;
	};
}
#endif // !_IFILE_CONTEXT_HPP_
