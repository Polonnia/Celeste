#include "Tilemap.h"
#include "assets.h"
#include "render.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include "../single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool load_tilemap(const char* levelName, Tilemap* tilemap)
{
    std::string filename = "assets/levels/" + std::string(levelName) + ".tmj";
    std::cout << "Attempting to load tilemap from: " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // 读取整个文件内容
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try {
        // 解析JSON
        json root = json::parse(content);
        std::cout << "Successfully parsed JSON" << std::endl;

        // 获取地图基本信息
        tilemap->width = root["width"].get<int>();
        tilemap->height = root["height"].get<int>();
        tilemap->tileSize = root["tilewidth"].get<float>();
        std::cout << "Map dimensions: " << tilemap->width << "x" << tilemap->height 
                  << ", tile size: " << tilemap->tileSize << std::endl;

        // 加载图块集
        const json& tilesets = root["tilesets"];
        std::cout << "Found " << tilesets.size() << " tilesets" << std::endl;
        
        for (const json& tileset : tilesets)
        {
            Tileset ts;
            
            // 只获取存在的字段
            ts.firstgid = tileset["firstgid"].get<int>();
            
            // 获取source路径
            std::string sourcePath = tileset["source"].get<std::string>();
            std::cout << "Loading tileset from source: " << sourcePath << std::endl;
            
            // 设置默认值
            ts.name = "tileset_" + std::to_string(ts.firstgid);
            ts.tileWidth = root["tilewidth"].get<int>();
            ts.tileHeight = root["tileheight"].get<int>();
            ts.tileCount = 0; // 默认值
            ts.columns = 0;   // 默认值
            ts.imageWidth = 0; // 默认值
            ts.imageHeight = 0; // 默认值
            
            // 加载.tsj文件
            std::string tsjPath = "assets/tiles/" + sourcePath;
            std::cout << "Loading .tsj file: " << tsjPath << std::endl;
            
            std::ifstream tsjFile(tsjPath);
            if (tsjFile.is_open())
            {
                std::stringstream tsjBuffer;
                tsjBuffer << tsjFile.rdbuf();
                std::string tsjContent = tsjBuffer.str();
                
                try {
                    json tsjJson = json::parse(tsjContent);
                    
                    // 从.tsj文件中获取信息
                    ts.name = tsjJson["name"].get<std::string>();
                    ts.tileWidth = tsjJson["tilewidth"].get<int>();
                    ts.tileHeight = tsjJson["tileheight"].get<int>();
                    ts.tileCount = tsjJson["tilecount"].get<int>();
                    ts.columns = tsjJson["columns"].get<int>();
                    ts.imageWidth = tsjJson["imagewidth"].get<int>();
                    ts.imageHeight = tsjJson["imageheight"].get<int>();
                    
                    // 获取图片路径
                    std::string imageName = tsjJson["image"].get<std::string>();
                    ts.imagePath = "assets/tiles/" + imageName;
                    
                    std::cout << "Loaded tileset: " << ts.name 
                              << ", image: " << ts.imagePath 
                              << ", size: " << ts.tileWidth << "x" << ts.tileHeight 
                              << ", count: " << ts.tileCount << std::endl;
                }
                catch (const json::exception& e) {
                    std::cout << "Error parsing .tsj file: " << e.what() << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to open .tsj file: " << tsjPath << std::endl;
                
                // 如果无法加载.tsj文件，使用默认值
                fs::path path(sourcePath);
                ts.imagePath = "assets/tiles/" + path.filename().string();
                std::cout << "Using default image path: " << ts.imagePath << std::endl;
            }
            
            tilemap->tilesets.push_back(ts);
        }

        // 加载图层
        const json& layers = root["layers"];
        std::cout << "Found " << layers.size() << " layers" << std::endl;
        
        for (const json& layer : layers)
        {
            std::string layerName = layer["name"].get<std::string>();
            std::cout << "Processing layer: " << layerName << std::endl;
            
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

        std::cout << "Successfully loaded tilemap" << std::endl;
        return true;
    }
    catch (const json::exception& e) {
        std::cout << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cout << "Error loading tilemap: " << e.what() << std::endl;
        return false;
    }
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