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

    // ��ȡ�����ļ�����
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try {
        // ����JSON
        json root = json::parse(content);
        std::cout << "Successfully parsed JSON" << std::endl;

        // ��ȡ��ͼ������Ϣ
        tilemap->width = root["width"].get<int>();
        tilemap->height = root["height"].get<int>();
        tilemap->tileSize = root["tilewidth"].get<float>();
        std::cout << "Map dimensions: " << tilemap->width << "x" << tilemap->height 
                  << ", tile size: " << tilemap->tileSize << std::endl;

        // ����ͼ�鼯
        const json& tilesets = root["tilesets"];
        std::cout << "Found " << tilesets.size() << " tilesets" << std::endl;
        
        for (const json& tileset : tilesets)
        {
            Tileset ts;
            
            // ֻ��ȡ���ڵ��ֶ�
            ts.firstgid = tileset["firstgid"].get<int>();
            
            // ��ȡsource·��
            std::string sourcePath = tileset["source"].get<std::string>();
            std::cout << "Loading tileset from source: " << sourcePath << std::endl;
            
            // ����Ĭ��ֵ
            ts.name = "tileset_" + std::to_string(ts.firstgid);
            ts.tileWidth = root["tilewidth"].get<int>();
            ts.tileHeight = root["tileheight"].get<int>();
            ts.tileCount = 0; // Ĭ��ֵ
            ts.columns = 0;   // Ĭ��ֵ
            ts.imageWidth = 0; // Ĭ��ֵ
            ts.imageHeight = 0; // Ĭ��ֵ
            
            // ����.tsj�ļ�
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
                    
                    // ��.tsj�ļ��л�ȡ��Ϣ
                    ts.name = tsjJson["name"].get<std::string>();
                    ts.tileWidth = tsjJson["tilewidth"].get<int>();
                    ts.tileHeight = tsjJson["tileheight"].get<int>();
                    ts.tileCount = tsjJson["tilecount"].get<int>();
                    ts.columns = tsjJson["columns"].get<int>();
                    ts.imageWidth = tsjJson["imagewidth"].get<int>();
                    ts.imageHeight = tsjJson["imageheight"].get<int>();
                    
                    // ��ȡͼƬ·��
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
                
                // ����޷�����.tsj�ļ���ʹ��Ĭ��ֵ
                fs::path path(sourcePath);
                ts.imagePath = "assets/tiles/" + path.filename().string();
                std::cout << "Using default image path: " << ts.imagePath << std::endl;
            }
            
            tilemap->tilesets.push_back(ts);
        }

        // ����ͼ��
        const json& layers = root["layers"];
        std::cout << "Found " << layers.size() << " layers" << std::endl;
        
        for (const json& layer : layers)
        {
            std::string layerName = layer["name"].get<std::string>();
            std::cout << "Processing layer: " << layerName << std::endl;
            
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