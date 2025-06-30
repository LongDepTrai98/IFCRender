#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
namespace dragon
{
	class IGeometryCache; 
	class IFCFileContext : public IFileContext
	{
	public: 
		IFCFileContext();
		~IFCFileContext(); 
	public: 
		std::string getFileType() override; 
		IGeometryCache* getGeometryCache(); 
	private: 
		std::unique_ptr<IGeometryCache> m_Geometry_Offset_Cache{ nullptr };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_
