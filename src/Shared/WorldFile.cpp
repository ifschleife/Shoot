
#include "WorldFile.h"
#include "Math/Serialize.h"

#include "nlohmann_json/json.hpp"
#include <cstdio>


bool world::WriteWorld(File::FilePtr& file, const LevelFileHeader& level)
{
    std::fwrite(&level.version, sizeof(int), 1, file.get());

    const int n_polygons = static_cast<int>(level.polygons.size());
    std::fwrite(&n_polygons, sizeof(int), 1, file.get());

    for(int index = 0; index < n_polygons; ++index)
    {
        const world::PolygonData& polygon = level.polygons[index];
        const int n_vertices = static_cast<int>(polygon.vertices.size());

        std::fwrite(&polygon.position,          sizeof(math::Vector), 1,          file.get());
        std::fwrite(&polygon.local_offset,      sizeof(math::Vector), 1,          file.get());
        std::fwrite(&polygon.rotation,          sizeof(float),        1,          file.get());
        std::fwrite(&polygon.texture,           sizeof(char),        64,          file.get());
        std::fwrite(&n_vertices,                sizeof(int),          1,          file.get());
        std::fwrite(polygon.vertices.data(),    sizeof(math::Vector), n_vertices, file.get());
    }

    return true;
}

bool world::ReadWorld(File::FilePtr& file, LevelFileHeader& level)
{
    std::vector<byte> bytes;
    File::FileRead(file, bytes);

    int offset = 0;

    std::memcpy(&level.version,     bytes.data() + offset, sizeof(int));
    offset += sizeof(int);

    int n_polygons = 0;
    std::memcpy(&n_polygons,        bytes.data() + offset, sizeof(int));
    offset += sizeof(int);

    level.polygons.resize(n_polygons);

    for(int index = 0; index < n_polygons; ++index)
    {
        world::PolygonData& polygon = level.polygons[index];

        std::memcpy(&polygon.position, bytes.data() + offset, sizeof(math::Vector));
        offset += sizeof(math::Vector);

        std::memcpy(&polygon.local_offset, bytes.data() + offset, sizeof(math::Vector));
        offset += sizeof(math::Vector);

        std::memcpy(&polygon.rotation, bytes.data() + offset, sizeof(float));
        offset += sizeof(float);

        std::memcpy(&polygon.texture, bytes.data() + offset, sizeof(char) * 64);
        offset += sizeof(char) * 64;

        int n_vertices = 0;
        std::memcpy(&n_vertices,        bytes.data() + offset, sizeof(int));
        offset += sizeof(int);

        polygon.vertices.resize(n_vertices);

        std::memcpy(polygon.vertices.data(), bytes.data() + offset, sizeof(math::Vector) * n_vertices);
        offset += sizeof(math::Vector) * n_vertices;
    }

    return true;
}

bool world::WriteWorldObjects2(File::FilePtr& file, const world::WorldObjectsHeader& objects_header)
{
    std::fwrite(&objects_header.version, sizeof(int), 1, file.get());

    const int n_objects = static_cast<int>(objects_header.objects.size());
    std::fwrite(&n_objects, sizeof(int), 1, file.get());

    for(int index = 0; index < n_objects; ++index)
    {
        const world::WorldObject& world_object = objects_header.objects[index];
        const int n_attributes = static_cast<int>(world_object.attributes.size());

        std::fwrite(&world_object.name, sizeof(char), 24, file.get());
        std::fwrite(&n_attributes, sizeof(int), 1, file.get());
        std::fwrite(world_object.attributes.data(), sizeof(ID_Attribute), n_attributes, file.get());
    }

    return true;
}

bool world::ReadWorldObjects2(File::FilePtr& file, world::WorldObjectsHeader& objects)
{
    if(!file)
        return false;

    std::vector<byte> bytes;
    File::FileRead(file, bytes);

    if(bytes.empty())
        return false;

    int offset = 0;

    std::memcpy(&objects.version, bytes.data() + offset, sizeof(int));
    offset += sizeof(int);

    int n_objects = 0;
    std::memcpy(&n_objects, bytes.data() + offset, sizeof(int));
    offset += sizeof(int);

    objects.objects.resize(n_objects);

    for(int index = 0; index < n_objects; ++index)
    {
        world::WorldObject& world_object = objects.objects[index];
        
        std::memcpy(&world_object.name, bytes.data() + offset, sizeof(char) * 24);
        offset += sizeof(char) * 24;
        
        int n_attributes = 0;
        std::memcpy(&n_attributes, bytes.data() + offset, sizeof(int));
        offset += sizeof(int);

        world_object.attributes.resize(n_attributes);

        std::memcpy(world_object.attributes.data(), bytes.data() + offset, sizeof(ID_Attribute) * n_attributes);
        offset += sizeof(ID_Attribute) * n_attributes;
    }

    return true;
}


bool world::WriteWorldObjects(File::FilePtr& file, const std::vector<WorldObject>& objects)
{
    nlohmann::json json_object_collection;
    
    for(auto& object : objects)
    {
        nlohmann::json json_object;

        json_object["name"] = object.name;
        nlohmann::json& attributes = json_object["attributes"];

        for(auto& attribute : object.attributes)
        {
            nlohmann::json json_attribute;
            json_attribute["id"] = attribute.id;
            json_attribute["type"] = static_cast<int>(attribute.attribute.type);
            json_attribute["data"] = attribute.attribute.data.string_value;
            attributes.push_back(json_attribute);
        }

        json_object_collection.push_back(json_object);
    }

    nlohmann::json json;
    json["objects"] = json_object_collection;

    const std::string& serialized_json = json.dump(4);
    std::fwrite(serialized_json.data(), serialized_json.length(), sizeof(char), file.get());
    
    return true;
}

bool world::ReadWorldObjects(File::FilePtr& file, std::vector<WorldObject>& objects)
{    
    std::vector<byte> file_data;
    File::FileRead(file, file_data);

    const nlohmann::json& json = nlohmann::json::parse(file_data);
    const nlohmann::json& json_objects = json["objects"];

    for(const auto& json_object : json_objects)
    {
        WorldObject object;

        //object.name = json_object["name"];

        for(auto& json_attribute : json_object["attributes"])
        {
            ID_Attribute attribute;
            attribute.id = json_attribute["id"];
                        
            const std::string data = json_attribute["data"];
            attribute.attribute = data.c_str();
            
            const int type = static_cast<int>(json_attribute["type"]);
            attribute.attribute.type = ObjectAttribute::Type(type);
            
            object.attributes.push_back(attribute);
        }

        objects.emplace_back(object);
    }

    return true;
}
