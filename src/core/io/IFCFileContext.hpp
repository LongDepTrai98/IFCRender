#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
namespace dragon
{
	class IFCFileContext : public IFileContext
	{
	public: 
		IFCFileContext();
		~IFCFileContext(); 
	public: 
		std::string getFileType() override; 
		IGeometryCache* getGeometryCache(); 
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_
