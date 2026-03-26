#include "pch.h"
#include <stdio.h>
#include <math.h>
#include <random>
#include "PerlinNoise.hpp"

//General Structures
struct Vec3
{
    Vec3():x(0.0f),y(0.0f),z(0.0f){}
    Vec3(float x, float y, float z): x(x), y(y), z(z) {}
    float x;
    float y;
    float z;
};

//Field Structures
struct FieldConstructionData
{
    //Cells
    const int radialCells;
    const int heightCells;
    const int angularCells;
    //Shapes
    //External
    const float domeRadius;
    const float cilinderRadius;
    const float cilinderHeight;
    const float coneTrunkRadiusTop;
    const float coneTrunkHeight;
    const float coneTrunkRadiusBottom;
    //Internal
    const float pilarRadius;
    const float pilarHeight;
    const float towerBaseRadius;
    const float towerTrunkRadius;
    const float towerTrunkHeight;
    const float towerTopRadius;
    const float bridgeWidth;
};

struct FieldPoint
{
    Vec3 position;
    Vec3 direction;
};

struct RadialField
{
    FieldConstructionData constructionData;
    FieldPoint* fieldPoints;
};

struct FlowField
{
    int sizeX;
    int sizeY;
    int sizeZ;

    float cellSize;

    Vec3* vectors;
};

//Drone Structures
struct Drone
{
    Vec3 position;
    Vec3 velocity;
};

//Field instantiation variables
FlowField field;
//Construction
//Noise
siv::PerlinNoise perlin{ 12345 }; // seed
float noiseOffsetX = 31.34f;
float noiseOffsetY = 47.77f;
float noiseOffsetZ = 12.93f;
//Curl
float curlE = 0.1f;
//Turbulence
Vec3 turbStrength = Vec3(0.1f,0.1f,0.1f);
//Drone Update
float fieldInfluenceStength = 0.5f;
float droneSpeedLimit = 10.0f;

//Drones instantiation varibales
const int MAX_DRONES = 5000;
Drone drones[MAX_DRONES];
int droneCount = 0;

//Field functions
float noiseX(float x, float y, float z, int id)
{
    return float(perlin.noise3D(x * 0.05 + noiseOffsetX * id / droneCount, y * 0.05, z * 0.05));
}

float noiseY(float x, float y, float z, int id)
{
    return float(perlin.noise3D(x * 0.05, y * 0.05 + noiseOffsetY * id / droneCount, z * 0.05));
}

float noiseZ(float x, float y, float z, int id)
{
    return float(perlin.noise3D(x * 0.05, y * 0.05, z * 0.05 + noiseOffsetZ * id / droneCount));
}

Vec3 CurlNoise(float x, float y, float z, int id)
{
    float e = curlE;

    float dFz_dy =
        (noiseZ(x, y + e, z, id) - noiseZ(x, y - e, z, id)) / (2 * e);

    float dFy_dz =
        (noiseY(x, y, z + e, id) - noiseY(x, y, z - e, id)) / (2 * e);

    float dFx_dz =
        (noiseX(x, y, z + e, id) - noiseX(x, y, z - e, id)) / (2 * e);

    float dFz_dx =
        (noiseZ(x + e, y, z, id) - noiseZ(x - e, y, z, id)) / (2 * e);

    float dFy_dx =
        (noiseY(x + e, y, z, id) - noiseY(x - e, y, z, id)) / (2 * e);

    float dFx_dy =
        (noiseX(x, y + e, z, id) - noiseX(x, y - e, z, id)) / (2 * e);

    Vec3 curl(dFz_dy - dFy_dz, dFx_dz - dFz_dx, dFy_dx - dFx_dy);

    return curl;
}

Vec3 SampleField(float x, float y, float z, int id)
{
    x += (field.sizeX - 1) * field.cellSize / 2.0f;
    z += (field.sizeZ - 1) * field.cellSize / 2.0f;

    float gx = x / field.cellSize;
    float gy = y / field.cellSize;
    float gz = z / field.cellSize;

    int x0 = (int)floor(gx);
    int y0 = (int)floor(gy);
    int z0 = (int)floor(gz);

    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;

    x0 = max(0, min(field.sizeX - 1, x0));
    y0 = max(0, min(field.sizeY - 1, y0));
    z0 = max(0, min(field.sizeZ - 1, z0));

    x1 = max(0, min(field.sizeX - 1, x1));
    y1 = max(0, min(field.sizeY - 1, y1));
    z1 = max(0, min(field.sizeZ - 1, z1));

    float tx = gx - floor(gx);
    float ty = gy - floor(gy);
    float tz = gz - floor(gz);

    auto index = [&](int x, int y, int z)
        {
            return x + y * field.sizeX + z * field.sizeX * field.sizeY;
        };

    Vec3 c000 = field.vectors[index(x0, y0, z0)];
    Vec3 c100 = field.vectors[index(x1, y0, z0)];
    Vec3 c010 = field.vectors[index(x0, y1, z0)];
    Vec3 c110 = field.vectors[index(x1, y1, z0)];

    Vec3 c001 = field.vectors[index(x0, y0, z1)];
    Vec3 c101 = field.vectors[index(x1, y0, z1)];
    Vec3 c011 = field.vectors[index(x0, y1, z1)];
    Vec3 c111 = field.vectors[index(x1, y1, z1)];

    auto lerp = [](Vec3 a, Vec3 b, float t)
        {
            return Vec3{
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            };
        };

    Vec3 c00 = lerp(c000, c100, tx);
    Vec3 c10 = lerp(c010, c110, tx);
    Vec3 c01 = lerp(c001, c101, tx);
    Vec3 c11 = lerp(c011, c111, tx);

    Vec3 c0 = lerp(c00, c10, ty);
    Vec3 c1 = lerp(c01, c11, ty);

    Vec3 base = lerp(c0, c1, tz);

    Vec3 curl = CurlNoise(x * 0.01f, y * 0.01f, z * 0.01f, id);

    float curlLen = sqrt(curl.x * curl.x + curl.y * curl.y + curl.z * curl.z);

    if (curlLen > 0)
    {
        curl.x /= curlLen;
        curl.y /= curlLen;
        curl.z /= curlLen;
    }

    base.x += curl.x * turbStrength.x;
    base.y += curl.y * turbStrength.y;
    base.z += curl.z * turbStrength.z;

    float baseIntensity = sqrt(base.x * base.x + base.y * base.y + base.z * base.z);

    //Normalize
    if (baseIntensity > 0.0f) {
        base.x /= baseIntensity;
        base.y /= baseIntensity;
        base.z /= baseIntensity;
    }

    return base;
}

//Drone Functions
void UpdateDrone(Drone& d, float dt, int id)
{
    Vec3 flow = SampleField(
        d.position.x,
        d.position.y,
        d.position.z,
        id
    );

    float strength = fieldInfluenceStength;

    d.velocity.x += flow.x * strength * dt;
    d.velocity.y += flow.y * strength * dt;
    d.velocity.z += flow.z * strength * dt;

    float speedLimit = droneSpeedLimit;

    float speed =
        sqrt(
            d.velocity.x * d.velocity.x +
            d.velocity.y * d.velocity.y +
            d.velocity.z * d.velocity.z
        );

    if (speed > speedLimit)
    {
        d.velocity.x *= speedLimit / speed;
        d.velocity.y *= speedLimit / speed;
        d.velocity.z *= speedLimit / speed;
    }

    d.position.x += d.velocity.x * dt;
    d.position.y += d.velocity.y * dt;
    d.position.z += d.velocity.z * dt;
}

//Field external functions
extern "C"
{
    __declspec(dllexport) void InitializeField(int sx, int sy, int sz, float cellSize)
    {
        field.sizeX = sx;
        field.sizeY = sy;
        field.sizeZ = sz;
        field.cellSize = cellSize;

        int total = sx * sy * sz;

        field.vectors = new Vec3[total];

        for (int z = 0; z < sz; z++)
            for (int y = 0; y < sy; y++)
                for (int x = 0; x < sx; x++)
                {
                    int i = x + y * sx + z * sx * sy;

                    float worldPosX = (float)x * cellSize - sx * cellSize / 2;
                    float worldPosY = (float)y * cellSize - sy * cellSize / 2;
                    float worldPosZ = (float)z * cellSize - sz * cellSize / 2;

                    if (x == 0 || x == sx - 1 || y == 0 || y == sy - 1 || z == 0 || z == sz - 1) {
                        field.vectors[i].x = -worldPosX;
                        field.vectors[i].y = -worldPosY;
                        field.vectors[i].z = -worldPosZ;
                    }
                    else {
                        field.vectors[i].x = +worldPosZ;
                        field.vectors[i].y = -worldPosY;
                        field.vectors[i].z = -worldPosX;
                    }

                    float baseIntensity = sqrt(field.vectors[i].x * field.vectors[i].x + field.vectors[i].y * field.vectors[i].y + field.vectors[i].z * field.vectors[i].z);

                    //Normalize
                    if (baseIntensity > 0.0f) {
                        field.vectors[i].x /= baseIntensity;
                        field.vectors[i].y /= baseIntensity;
                        field.vectors[i].z /= baseIntensity;
                    }
                    else
                    {
                        field.vectors[i].x = 0.0f;
                        field.vectors[i].y = 0.0f;
                        field.vectors[i].z = 1.0f;
                    }
                }
    }
}

//Drone external functions
extern "C"
{
    __declspec(dllexport) void InitializeDrones(int count)
    {
        droneCount = count;

        for (int i = 0; i < count; i++)
        {
            drones[i].position =
            {
                (float)(rand() % 50-25),
                (float)(rand() % 10),
                (float)(rand() % 50-25)
            };

            drones[i].velocity = { 0,0,0 };
        }
    }

    __declspec(dllexport) void UpdateSwarm(float dt)
    {
        for (int i = 0; i < droneCount; i++)
        {
            UpdateDrone(drones[i], dt, i);
        }
    }

    __declspec(dllexport) Drone* GetDroneArray()
    {
        return drones;
    }

    __declspec(dllexport) int GetDroneCount()
    {
        return droneCount;
    }

    __declspec(dllexport) Vec3* GetFieldVectors()
    {
        return field.vectors;
    }

    __declspec(dllexport) int GetFieldSizeX()
    {
        return field.sizeX;
    }

    __declspec(dllexport) int GetFieldSizeY()
    {
        return field.sizeY;
    }

    __declspec(dllexport) int GetFieldSizeZ()
    {
        return field.sizeZ;
    }

    __declspec(dllexport) float GetFieldCellSize()
    {
        return field.cellSize;
    }

    //Construction
    //Noise
    __declspec(dllexport) float GetNoiseOffsetX()
    {
        return noiseOffsetX;
    }
    __declspec(dllexport) void SetNoiseOffsetX(float a_noiseOffsetX)
    {
        noiseOffsetX = a_noiseOffsetX;
    }

    __declspec(dllexport) float GetNoiseOffsetY()
    {
        return noiseOffsetY;
    }
    __declspec(dllexport) void SetNoiseOffsetY(float a_noiseOffsetY)
    {
        noiseOffsetY = a_noiseOffsetY;
    }

    __declspec(dllexport) float GetNoiseOffsetZ()
    {
        return noiseOffsetZ;
    }
    __declspec(dllexport) void SetNoiseOffsetZ(float a_noiseOffsetZ)
    {
        noiseOffsetZ = a_noiseOffsetZ;
    }
    //Curl
    __declspec(dllexport) float GetCurlE()
    {
        return curlE;
    }
    __declspec(dllexport) void SetCurlE(float a_curlE)
    {
        curlE = a_curlE;
    }
    //Turbulence
    __declspec(dllexport) Vec3 GetTurbStrength()
    {
        return turbStrength;
    }
    __declspec(dllexport) void SetTurbStrength(Vec3 a_turbStrength)
    {
        turbStrength = a_turbStrength;
    }
    //Drone Update
    __declspec(dllexport) float GetFieldInfluenceStength()
    {
        return fieldInfluenceStength;
    }
    __declspec(dllexport) void SetFieldInfluenceStength(float a_fieldInfluenceStength)
    {
        fieldInfluenceStength = a_fieldInfluenceStength;
    }

    __declspec(dllexport) float GetDroneSpeedLimit()
    {
        return droneSpeedLimit;
    }
    __declspec(dllexport) void SetDroneSpeedLimit(float a_droneSpeedLimit)
    {
        droneSpeedLimit = a_droneSpeedLimit;
    }
}