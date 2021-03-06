
#include "BinarySerializer.h"
#include "Objects/Polygon.h"
#include "ObjectProxies/PolygonProxy.h"
#include "ObjectProxies/EntityProxy.h"

#include "Math/Matrix.h"
#include "System/File.h"

using namespace editor;

BinarySerializer::BinarySerializer()
{
    m_polygon_data.version = 1;    
}

void BinarySerializer::WritePolygonFile(const std::string& file_name) const
{
    File::FilePtr file = File::CreateBinaryFile(file_name.c_str());
    world::WriteWorld(file, m_polygon_data);
}

void BinarySerializer::WriteObjects(const std::string& file_name) const
{
    File::FilePtr file = File::CreateBinaryFile(file_name.c_str());
    world::WriteWorldObjects2(file, m_object_data);    
}

void BinarySerializer::Accept(EntityProxy* proxy)
{
    world::WorldObject world_object;
    
    const char* name = proxy->Name();
    const size_t length = std::strlen(name);
    const size_t data_length = std::min(length, size_t(24) -1);

    std::memcpy(world_object.name, name, data_length);
    world_object.name[data_length] = '\0';
    world_object.attributes = proxy->GetAttributes();

    m_object_data.objects.push_back(world_object);
}

void BinarySerializer::Accept(PathProxy* proxy)
{

}

void BinarySerializer::Accept(PolygonProxy* proxy)
{
    auto polygon_entity = proxy->m_polygon;

    world::PolygonData polygon_data;

    polygon_data.position = polygon_entity->Position();
    polygon_data.local_offset = polygon_entity->BasePoint();
    polygon_data.rotation = polygon_entity->Rotation();

    const char* texture = polygon_entity->GetTexture();
    const std::size_t string_length = std::strlen(texture);

    std::memcpy(polygon_data.texture, texture, string_length);

    const math::Matrix& local_to_world = polygon_entity->Transformation();

    for(const math::Vector& vertex : polygon_entity->GetVertices())
        polygon_data.vertices.push_back(math::Transform(local_to_world, vertex));

    m_polygon_data.polygons.push_back(polygon_data);
}

void BinarySerializer::Accept(PrefabProxy* proxy)
{

}
