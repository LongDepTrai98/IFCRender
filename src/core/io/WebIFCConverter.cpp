#define FMT_UNICODE 0
#include "threepp/threepp.hpp"
#include "WebIFCConverter.hpp"
#include "web-ifc/parsing/IfcLoader.h"
#include "web-ifc/schema/IfcSchemaManager.h"
#include "web-ifc/schema/ifc-schema.h"
#include "io_helpers.h"
#include "core/utils/StringHelper.hpp"
#include "web-ifc/geometry/IfcGeometryProcessor.h"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include <glm/gtc/type_ptr.hpp>
namespace dragon
{
    std::shared_ptr<threepp::Group> WebIFCConverter::convert(const std::filesystem::path& path)
	{
		std::string content = StringHelper::ReadFile(path.string());
        struct LoaderSettings
        {
            bool COORDINATE_TO_ORIGIN = false;
            uint16_t CIRCLE_SEGMENTS = 12;
            uint32_t TAPE_SIZE = 67108864; // probably no need for anyone other than web-ifc devs to change this
            uint32_t MEMORY_LIMIT = 2147483648;
            uint16_t LINEWRITER_BUFFER = 10000;
        };
        LoaderSettings set;
        set.COORDINATE_TO_ORIGIN = true;

        webifc::schema::IfcSchemaManager schemaManager;
        webifc::parsing::IfcLoader loader(set.TAPE_SIZE, set.MEMORY_LIMIT, set.LINEWRITER_BUFFER, schemaManager);

        loader.LoadFile([&](char* dest, size_t sourceOffset, size_t destSize)
            {
                uint32_t length = std::min(content.size() - sourceOffset, destSize);
                memcpy(dest, &content[sourceOffset], length);
	               return length; });
        webifc::geometry::IfcGeometryProcessor geometryLoader(loader, schemaManager, set.CIRCLE_SEGMENTS, set.COORDINATE_TO_ORIGIN);
        std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{}; 
        std::vector<std::shared_ptr<threepp::Material>> materials{}; 
        loadAllMesh(loader, geometryLoader,geometries, materials);
        std::shared_ptr<threepp::Group> container = threepp::Group::create(); 
        std::shared_ptr<threepp::Mesh> mergeMesh = Mergeo(geometries, materials); 
        container->add(mergeMesh); 
        return container; 
	}
    void WebIFCConverter::loadAllMesh(webifc::parsing::IfcLoader& loader,
        webifc::geometry::IfcGeometryProcessor& geometryLoader,
        std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, 
        std::vector<std::shared_ptr<threepp::Material>>& materials)
    {
        webifc::schema::IfcSchemaManager schema;
        for (auto type : schema.GetIfcElementList())
        {
            auto elements = loader.GetExpressIDsWithType(type);

            for (unsigned int i = 0; i < elements.size(); i++)
            {
                auto flatMesh = geometryLoader.GetFlatMesh(elements[i]);
                for (auto& geom : flatMesh.geometries)
                {
                    auto flatGeom = geometryLoader.GetGeometry(geom.geometryExpressID);
                    auto flatTrans = geom.transformation; 
                    
                    const double* ptr = glm::value_ptr(flatTrans);
                    std::array<float, 16> matrix_f; 

                    for (size_t i = 0; i < 16; ++i) {
                        matrix_f[i] = static_cast<float>(ptr[i]);
                    }

                   /* for (size_t i = 0; i < 16; ++i) {
                        matrix_f[i] = static_cast<float>(flatTrans[i]); 
                    }*/
                    threepp::Matrix4 flatTransformation = threepp::Matrix4(matrix_f);
                    auto& color = geom.color; 
                    float r = color[0];
                    float g = color[1];
                    float b = color[2];
                    float opacity = color[3]; 
                    /*CRATE GEOMETRY*/
                    std::vector<float> vertices;
                    std::vector<float> normals;
                    std::vector<int> indices;
                    const std::vector<uint32_t>& indexData = flatGeom.indexData;
                    indices.reserve(indexData.size()); 
                    for (uint32_t val : indexData)
                    {
                        indices.emplace_back(static_cast<int>(val)); 
                    }
                    flatGeom.fvertexData;
                    for(int j = 0; j < flatGeom.numPoints; ++j)
                    {
                        auto point = flatGeom.GetPoint(j);
                        vertices.insert(vertices.end(), { static_cast<float>(point.x),static_cast<float>(point.y),static_cast<float>(point.z) });
                        normals.insert(normals.end(), { static_cast<float>(point.x),static_cast<float>(point.y),static_cast<float>(point.z) });
                    }
                    std::shared_ptr<threepp::BufferGeometry> geometry = threepp::BufferGeometry::create();
                    geometry->applyMatrix4(flatTransformation); 
                    geometry->setIndex(indices); 
                    geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
                    geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
                    geometries.emplace_back(geometry); 
                    /*CREATE MATERIAL*/
                    std::shared_ptr<threepp::MeshBasicMaterial> material = threepp::MeshBasicMaterial::create();
                    material->color = threepp::Color(r,g,b);
                    bool isTransparent{ false }; 
                    if(opacity < 1.0)
                    { 
                        isTransparent = true; 
                    }
                    material->transparent = isTransparent; 
                    material->opacity = opacity; 
                    material->polygonOffset = true;
                    material->polygonOffsetFactor = 1.0f;
                    material->polygonOffsetUnits = 1.0f;
                    material->needsUpdate();
                    materials.emplace_back(material); 
                }
            }
        }
    }
    std::shared_ptr<threepp::Mesh> WebIFCConverter::Mergeo(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, const std::vector<std::shared_ptr<threepp::Material>>& materials)
    {
        std::cout << "Merging Phase:" << "----------------------------------------------" << std::endl;
        std::shared_ptr<threepp::BufferGeometry> buffMerge = threepp::mergeBufferGeometries(geometries);
        std::string mess{};
        int32_t indexOffset{ 0 };
        for (int i = 0; i < materials.size(); ++i)
        {
            int index_count = geometries[i]->getIndex()->count();
            buffMerge->addGroup(indexOffset, index_count, i);
            std::stringstream strs;
            for (int ii = 0; ii < mess.size(); ++ii)
            {
                std::cout << '\b';
            }
            mess = std::format("Merge geo {}, total {}", i + 1, materials.size());
            strs << mess;
            std::cout << strs.str();
            indexOffset += index_count;
        }
        std::cout << std::endl;
        std::cout << "Merged Done:" << "----------------------------------------------" << std::endl;
        return threepp::Mesh::create(buffMerge, materials);
    }
}