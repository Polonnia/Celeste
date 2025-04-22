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

    // 读取整个文件内容
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // 解析JSON
    json root = json::parse(content);

    // 获取地图基本信息
    tilemap->width = root["width"].get<int>();
    tilemap->height = root["height"].get<int>();
    tilemap->tileSize = root["tilewidth"].get<float>();

    // 加载图块集
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

        // 获取图片路径
        std::string imagePath = tileset["image"].get<std::string>();
        // 转换为相对路径
        fs::path path(imagePath);
        ts.imagePath = "assets/tiles/" + path.filename().string();

        // 获取图片尺寸
        ts.imageWidth = tileset["imagewidth"].get<int>();
        ts.imageHeight = tileset["imageheight"].get<int>();

        tilemap->tilesets.push_back(ts);
    }

    // 加载图层
    const json& layers = root["layers"];
    for (const json& layer : layers)
    {
        std::string layerName = layer["name"].get<std::string>();
        const json& data = layer["data"];

        // 创建对应层的数据
        std::vector<std::vector<int>>& targetLayer = 
            (layerName == "terrain") ? tilemap->terrain :
            (layerName == "decorations") ? tilemap->decorations :
            tilemap->spikes;

        // 初始化层数据
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

    // 加载地图属性
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
    // 渲染地形层
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            int tileID = tilemap->terrain[y][x];
            if (tileID > 0)
            {
                // 计算渲染位置
                float renderX = x * tilemap->tileSize;
                float renderY = y * tilemap->tileSize;

                // 渲染地形块
                draw_tile(tilemap, tileID, renderX, renderY);
            }
        }
    }

    // 渲染装饰层
    for (int y = 0; y < tilemap->height; y++)
    {
        for (int x = 0; x < tilemap->width; x++)
        {
            int tileID = tilemap->decorations[y][x];
            if (tileID > 0)
            {
                // 计算渲染位置
                float renderX = x * tilemap->tileSize;
                float renderY = y * tilemap->tileSize;

                // 渲染装饰物
                draw_tile(tilemap, tileID, renderX, renderY);
            }
        }
    }
} 