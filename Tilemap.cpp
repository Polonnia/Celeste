#include "Tilemap.h"
#include "assets.h"
#include "render.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "../single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool load_tilemap(const char* levelName, Tilemap* tilemap)
{
    std::string filename = "assets/levels/" + std::string(levelName) + ".tmj";
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    // ��ȡ�����ļ�����
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // ����JSON
    json root = json::parse(content);

    // ��ȡ��ͼ������Ϣ
    tilemap->width = root["width"].get<int>();
    tilemap->height = root["height"].get<int>();
    tilemap->tileSize = root["tilewidth"].get<float>();

    // ����ͼ�鼯
    const json& tilesets = root["tilesets"];
    for (const json& tileset : tilesets)
    {
        Tileset ts;
        ts.name = tileset["name"].get<std::string>();
        ts.firstgid = tileset["firstgid"].get<int>();
        ts.tileWidth = tileset["tilewidth"].get<int>();
        ts.tileHeight = tileset["tileheight"].get<int>();
        ts.tileCount = tileset["tilecount"].get<int>();
        ts.columns = tileset["columns"].get<int>();

        // ��ȡͼƬ·��
        std::string imagePath = tileset["image"].get<std::string>();
        // ת��Ϊ���·��
        fs::path path(imagePath);
        ts.imagePath = "assets/tiles/" + path.filename().string();

        // ��ȡͼƬ�ߴ�
        ts.imageWidth = tileset["imagewidth"].get<int>();
        ts.imageHeight = tileset["imageheight"].get<int>();

        tilemap->tilesets.push_back(ts);
    }

    // ����ͼ��
    const json& layers = root["layers"];
    for (const json& layer : layers)
    {
        std::string layerName = layer["name"].get<std::string>();
        const json& data = layer["data"];

        // ������Ӧ�������
        std::vector<std::vector<int>>& targetLayer = 
            (layerName == "terrain") ? tilemap->terrain :
            (layerName == "decorations") ? tilemap->decorations :
            tilemap->spikes;

        // ��ʼ��������
        targetLayer.resize(tilemap->height);
        for (int y = 0; y < tilemap->height; y++)
        {
            targetLayer[y].resize(tilemap->width);
            for (int x = 0; x < tilemap->width; x++)
            {
                int index = y * tilemap->width + x;
                targetLayer[y][x] = data[index].get<int>();
            }
        }
    }

    // ���ص�ͼ����
    const json& properties = root["properties"];
    if (properties.is_array())
    {
        for (const json& prop : properties)
        {
            std::string name = prop["name"].get<std::string>();
            std::string value = prop["value"].get<std::string>();
            tilemap->properties[name] = value;
        }
    }

    return true;
}

bool is_solid(const Tilemap* tilemap, float x, float y)
{
    int tileX = (int)(x / tilemap->tileSize);
    int tileY = (int)(y / tilemap->tileSize);

    if (tileX < 0 || tileX >= tilemap->width || 
        tileY < 0 || tileY >= tilemap->height)
    {
        return false;
    }

    return tilemap->terrain[tileY][tileX] > 0;
}

bool is_spike(const Tilemap* tilemap, float x, float y)
{
    int tileX = (int)(x / tilemap->tileSize);
    int tileY = (int)(y / tilemap->tileSize);

    if (tileX < 0 || tileX >= tilemap->width || 
        tileY < 0 || tileY >= tilemap->height)
    {
        return false;
    }

    return tilemap->spikes[tileY][tileX] > 0;
}

void render_tilemap(const Tilemap* tilemap)
{
    // ��Ⱦ���β�
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            int tileID = tilemap->terrain[y][x];
            if (tileID > 0)
            {
                // ������Ⱦλ��
                float renderX = x * tilemap->tileSize;
                float renderY = y * tilemap->tileSize;

                // ��Ⱦ���ο�
                draw_tile(tilemap, tileID, renderX, renderY);
            }
        }
    }

    // ��Ⱦװ�β�
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            int tileID = tilemap->decorations[y][x];
            if (tileID > 0)
            {
                // ������Ⱦλ��
                float renderX = x * tilemap->tileSize;
                float renderY = y * tilemap->tileSize;

                // ��Ⱦװ����
                draw_tile(tilemap, tileID, renderX, renderY);
            }
        }
    }
} 