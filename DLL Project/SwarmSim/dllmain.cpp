#include "pch.h"
#include <stdio.h>
#include <math.h>
#include <random>
#include "PerlinNoise.hpp"

const double PI = 3.14159265358979323846;

//General Structures
struct Vec3
{
    Vec3():x(0.0f),y(0.0f),z(0.0f){}
    Vec3(float x, float y, float z): x(x), y(y), z(z) {}
    float x;
    float y;
    float z;
    Vec3 ToRadialVec3() { return Vec3(y, sqrt(x*x+z*z), atan2(x, z)); }
};

struct RadialVec3
{
    RadialVec3() :h(0.0f), r(0.0f), a(0.0f) {}
    RadialVec3(float h, float r, float a) : h(h), r(r), a(a) {}
    float h;
    float r;
    float a;
    Vec3 ToVec3() { return Vec3(sin(a)*r, h, cos(a)*r); }
};

//Field Structures
struct FieldConstructionData
{
    //Cells
    const int radialCells = 0;
    const int heightCells = 0;
    const int angularCells = 0;
    //Shapes
    //External
    const float domeRadius = 0;
    const float cilinderRadius = 0;
    const float cilinderHeight = 0;
    const float coneTrunkRadiusTop = 0;
    const float coneTrunkHeight = 0;
    const float coneTrunkRadiusBottom = 0;
    //Internal
    const float pilarRadius = 0;
    const float pilarHeight = 0;
    const float towerBaseRadius = 0;
    const float towerTrunkRadius = 0;
    const float towerTrunkHeight = 0;
    const float towerTopRadius = 0;
    const float bridgeWidth = 0;

    // Default constructor (all zeros)
    FieldConstructionData()
        : radialCells(0), heightCells(0), angularCells(0),
        domeRadius(0), cilinderRadius(0), cilinderHeight(0),
        coneTrunkRadiusTop(0), coneTrunkHeight(0), coneTrunkRadiusBottom(0),
        pilarRadius(0), pilarHeight(0),
        towerBaseRadius(0), towerTrunkRadius(0), towerTrunkHeight(0), towerTopRadius(0),
        bridgeWidth(0)
    {
    }

    // Parameter constructor
    FieldConstructionData(
        int radialCells,
        int heightCells,
        int angularCells,
        float domeRadius,
        float cilinderRadius,
        float cilinderHeight,
        float coneTrunkRadiusTop,
        float coneTrunkHeight,
        float coneTrunkRadiusBottom,
        float pilarRadius,
        float pilarHeight,
        float towerBaseRadius,
        float towerTrunkRadius,
        float towerTrunkHeight,
        float towerTopRadius,
        float bridgeWidth)
        : radialCells(radialCells), heightCells(heightCells), angularCells(angularCells),
        domeRadius(domeRadius), cilinderRadius(cilinderRadius), cilinderHeight(cilinderHeight),
        coneTrunkRadiusTop(coneTrunkRadiusTop), coneTrunkHeight(coneTrunkHeight), coneTrunkRadiusBottom(coneTrunkRadiusBottom),
        pilarRadius(pilarRadius), pilarHeight(pilarHeight),
        towerBaseRadius(towerBaseRadius), towerTrunkRadius(towerTrunkRadius), towerTrunkHeight(towerTrunkHeight), towerTopRadius(towerTopRadius),
        bridgeWidth(bridgeWidth)
    {
    }

    // Copy constructor
    FieldConstructionData(const FieldConstructionData& other)
        : radialCells(other.radialCells), heightCells(other.heightCells), angularCells(other.angularCells),
        domeRadius(other.domeRadius), cilinderRadius(other.cilinderRadius), cilinderHeight(other.cilinderHeight),
        coneTrunkRadiusTop(other.coneTrunkRadiusTop), coneTrunkHeight(other.coneTrunkHeight), coneTrunkRadiusBottom(other.coneTrunkRadiusBottom),
        pilarRadius(other.pilarRadius), pilarHeight(other.pilarHeight),
        towerBaseRadius(other.towerBaseRadius), towerTrunkRadius(other.towerTrunkRadius), towerTrunkHeight(other.towerTrunkHeight), towerTopRadius(other.towerTopRadius),
        bridgeWidth(other.bridgeWidth)
    {
    }
};

struct FieldPoint
{
    Vec3 position;
    Vec3 direction;
};

struct RadialField
{
    FieldConstructionData* constructionData = nullptr;
    FieldPoint* fieldPoints = nullptr;
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
RadialField field;
/*DEPRECATED*/FlowField flowField;
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
    x += (flowField.sizeX - 1) * flowField.cellSize / 2.0f;
    z += (flowField.sizeZ - 1) * flowField.cellSize / 2.0f;

    float gx = x / flowField.cellSize;
    float gy = y / flowField.cellSize;
    float gz = z / flowField.cellSize;

    int x0 = (int)floor(gx);
    int y0 = (int)floor(gy);
    int z0 = (int)floor(gz);

    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;

    x0 = max(0, min(flowField.sizeX - 1, x0));
    y0 = max(0, min(flowField.sizeY - 1, y0));
    z0 = max(0, min(flowField.sizeZ - 1, z0));

    x1 = max(0, min(flowField.sizeX - 1, x1));
    y1 = max(0, min(flowField.sizeY - 1, y1));
    z1 = max(0, min(flowField.sizeZ - 1, z1));

    float tx = gx - floor(gx);
    float ty = gy - floor(gy);
    float tz = gz - floor(gz);

    auto index = [&](int x, int y, int z)
        {
            return x + y * flowField.sizeX + z * flowField.sizeX * flowField.sizeY;
        };

    Vec3 c000 = flowField.vectors[index(x0, y0, z0)];
    Vec3 c100 = flowField.vectors[index(x1, y0, z0)];
    Vec3 c010 = flowField.vectors[index(x0, y1, z0)];
    Vec3 c110 = flowField.vectors[index(x1, y1, z0)];

    Vec3 c001 = flowField.vectors[index(x0, y0, z1)];
    Vec3 c101 = flowField.vectors[index(x1, y0, z1)];
    Vec3 c011 = flowField.vectors[index(x0, y1, z1)];
    Vec3 c111 = flowField.vectors[index(x1, y1, z1)];

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
    /*TODO: IMPLEMENT*/
    __declspec(dllexport) void GenerateField(FieldConstructionData data)
    {
        //Memory alocation
        FieldConstructionData* oldData = field.constructionData;
        field.constructionData = new FieldConstructionData(data);

        const int heightVerts = field.constructionData->heightCells + 1;
        const int radialVerts = field.constructionData->radialCells + 1;
        const int angularVerts = field.constructionData->angularCells + 1;
        if (oldData != nullptr)
        { 
            if (oldData->heightCells != field.constructionData->heightCells || oldData->radialCells != field.constructionData->radialCells || oldData->angularCells != field.constructionData->angularCells)
            {
                if (field.fieldPoints != nullptr)
                {
                    delete field.fieldPoints;
                }
                int size = heightVerts * radialVerts * angularVerts;
                field.fieldPoints = new FieldPoint[size];
            }
            delete oldData;
        }
        else
        {
            int size = heightVerts * radialVerts * angularVerts;
            field.fieldPoints = new FieldPoint[size];
        }

        //Cell divisions
        //Height
        float cellHeightDelta = -(field.constructionData->domeRadius + field.constructionData->cilinderHeight + field.constructionData->coneTrunkHeight) / field.constructionData->heightCells;
        float* heightValues = new float[heightVerts];
        for (int i = 0; i < heightVerts; i++)
        {
            heightValues[i] = i * cellHeightDelta;
        }
        //TODO: Compress height values arround bridge height using function

        //Radius
        //Height radial sections (Refer to documentation)
        float bottom = - field.constructionData->domeRadius - field.constructionData->cilinderHeight - field.constructionData->coneTrunkHeight;
        float coneTrunkTop = bottom + field.constructionData->coneTrunkHeight;
        float pilarTop = bottom + field.constructionData->pilarHeight;
        float towerBaseTop = pilarTop + field.constructionData->towerBaseRadius;
        float towerTrunkTop = towerBaseTop + field.constructionData->towerTrunkHeight;
        float towerTopCenter = towerTrunkTop + field.constructionData->towerTopRadius;
        float towerTop = towerTopCenter + field.constructionData->towerTopRadius;
        
        const int heightRadialSectionCount = 7;
        std::pair<float, float> heightRadiusSections[heightRadialSectionCount] = {
            {0.0f, towerTop},
            {towerTop, towerTopCenter},
            {towerTopCenter, towerTrunkTop},
            {towerTrunkTop, towerBaseTop},
            {towerBaseTop, pilarTop},
            {pilarTop, coneTrunkTop},
            {coneTrunkTop, bottom}
        };
        float* radialValuesPerHeightValue = new float[heightVerts * radialVerts];
        //Helper lambdas
        auto circularRadiusFromHeight = [](float circRadius, float circCenterX, float circCenterY, float height) {
            return (height <= circCenterY + circRadius && height >= circCenterY - circRadius) ?
                sqrt(circRadius * circRadius - (height - circCenterY) * (height - circCenterY)) + circCenterX :
                height;
            };
        auto coneTrunkRadiusFromHeight = [](float baseRadius, float topradius, float baseYPos, float trunkHeight, float height) {
            if (topradius - baseRadius == 0 || trunkHeight == 0) {
                return height;
            }
            float a = trunkHeight / (topradius - baseRadius);
            float b = baseYPos - a * baseRadius;
            return (height - b) / a;
            };
        //Section logic
        for (int hi = 0; hi < heightVerts; hi++)
        {
            int si = 0;
            for (; si <= heightRadialSectionCount; si++) {
                if (heightValues[hi] <= heightRadiusSections[si].first && heightValues[hi] > heightRadiusSections[si].second) break;
            }
            
            float sectionInnerRadiusForHeight = 0.0f;
            float sectionRadiusForHeight = 0.0f;
            switch (si) {
            case 0: {
                //S0 radius (Refer to documentation)
                //***The function is defined only for height values between 0 and -field.constructionData->domeRadius***
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->domeRadius, 0.0f, -field.constructionData->domeRadius, heightValues[hi]);
                break;
            }
            case 1: {
                //S1 radius (Refer to documentation)
                //***The inner function is only defined for height values between -375 and -400***
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->towerTopRadius, 0.0f, towerTop - field.constructionData->towerTopRadius, heightValues[hi]);
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->domeRadius, 0.0f, -field.constructionData->domeRadius, heightValues[hi]);
                break;
            }
            case 2: {
                //S2 radius (Refer to documentation)
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->towerTopRadius, 0.0f, towerTop - field.constructionData->towerTopRadius, heightValues[hi]);
                sectionInnerRadiusForHeight = max(sectionInnerRadiusForHeight, field.constructionData->towerTrunkRadius);
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->domeRadius, 0.0f, -field.constructionData->domeRadius, heightValues[hi]);
                break;
            }
            case 3: {
                //S3 radius (Refer to documentation)
                sectionInnerRadiusForHeight = field.constructionData->towerTrunkRadius;
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->domeRadius, 0.0f, -field.constructionData->domeRadius, heightValues[hi]);
                break;
            }
            case 4: {
                //S4 radius (Refer to documentation)
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->towerBaseRadius, 0.0f, - field.constructionData->domeRadius, heightValues[hi]);
                sectionInnerRadiusForHeight = max(sectionInnerRadiusForHeight, field.constructionData->towerTrunkRadius);
                sectionRadiusForHeight = circularRadiusFromHeight(field.constructionData->domeRadius, 0.0f, -field.constructionData->domeRadius, heightValues[hi]);
                break;
            }
            case 5: {
                //S5 radius (Refer to documentation)
                sectionInnerRadiusForHeight = field.constructionData->pilarRadius;
                sectionRadiusForHeight = field.constructionData->cilinderRadius;
                break;
            }
            default: {
                //S6 radius (Refer to documentation)
                sectionInnerRadiusForHeight = field.constructionData->pilarRadius;
                sectionRadiusForHeight = coneTrunkRadiusFromHeight(field.constructionData->coneTrunkRadiusBottom, field.constructionData->coneTrunkRadiusTop, bottom, field.constructionData->coneTrunkHeight, heightValues[hi]);
                break;
            }
            }

            float cellRadiusDelta = (sectionRadiusForHeight - sectionInnerRadiusForHeight) / field.constructionData->radialCells;
            for (int ri = 0; ri < radialVerts; ri++)
            {
                radialValuesPerHeightValue[hi * radialVerts + ri] = sectionInnerRadiusForHeight + ri * cellRadiusDelta;
            }
        }
        //TODO: Compress radial values arround the min and max values

        //Angular
        float cellAngularDelta = (2 * PI) / field.constructionData->angularCells;
        float* angularValues = new float[angularVerts];
        for (int i = 0; i < angularVerts; i++)
        {
            angularValues[i] = i * cellAngularDelta;
        }
        //TODO: Compress angular values  arround 0, 90, 180, 270 and 360 degrees using function

        //Point Location and velocity
        for(int hi = 0; hi < heightVerts; hi++)
            for(int ri = 0; ri < radialVerts; ri++)
                for (int ai = 0; ai < angularVerts; ai++)
                {
                    int index = hi * radialVerts * angularVerts + ri * angularVerts + ai;
                    field.fieldPoints[index].position = RadialVec3(heightValues[hi], radialValuesPerHeightValue[hi*radialVerts+ri], angularValues[ai]).ToVec3();
                    field.fieldPoints[index].direction = Vec3(1.0f, 0.0f, 0.0f);
                }
    }

    /*TODO: REMOVE*/
    __declspec(dllexport) void InitializeField(int sx, int sy, int sz, float cellSize)
    {
        flowField.sizeX = sx;
        flowField.sizeY = sy;
        flowField.sizeZ = sz;
        flowField.cellSize = cellSize;

        int total = sx * sy * sz;

        flowField.vectors = new Vec3[total];

        for (int z = 0; z < sz; z++)
            for (int y = 0; y < sy; y++)
                for (int x = 0; x < sx; x++)
                {
                    int i = x + y * sx + z * sx * sy;

                    float worldPosX = (float)x * cellSize - sx * cellSize / 2;
                    float worldPosY = (float)y * cellSize - sy * cellSize / 2;
                    float worldPosZ = (float)z * cellSize - sz * cellSize / 2;

                    if (x == 0 || x == sx - 1 || y == 0 || y == sy - 1 || z == 0 || z == sz - 1) {
                        flowField.vectors[i].x = -worldPosX;
                        flowField.vectors[i].y = -worldPosY;
                        flowField.vectors[i].z = -worldPosZ;
                    }
                    else {
                        flowField.vectors[i].x = +worldPosZ;
                        flowField.vectors[i].y = -worldPosY;
                        flowField.vectors[i].z = -worldPosX;
                    }

                    float baseIntensity = sqrt(flowField.vectors[i].x * flowField.vectors[i].x + flowField.vectors[i].y * flowField.vectors[i].y + flowField.vectors[i].z * flowField.vectors[i].z);

                    //Normalize
                    if (baseIntensity > 0.0f) {
                        flowField.vectors[i].x /= baseIntensity;
                        flowField.vectors[i].y /= baseIntensity;
                        flowField.vectors[i].z /= baseIntensity;
                    }
                    else
                    {
                        flowField.vectors[i].x = 0.0f;
                        flowField.vectors[i].y = 0.0f;
                        flowField.vectors[i].z = 1.0f;
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

    __declspec(dllexport)  FieldConstructionData* GetFieldConstructionData() 
    {
        return field.constructionData;
    }

    __declspec(dllexport)  int GetHeightCellCount()
    {
        return field.constructionData->heightCells;
    }

    __declspec(dllexport) int GetRadialCellCount()
    {
        return field.constructionData->radialCells;
    }

    __declspec(dllexport) int GetAngularCellCount() 
    {
        return field.constructionData->angularCells;
    }

    __declspec(dllexport) FieldPoint* GetField()
    {
        return field.fieldPoints;
    }

    /*TODO: REMOVE*/
    __declspec(dllexport) Vec3* GetFieldVectors()
    {
        return flowField.vectors;
    }
    /*TODO: REMOVE*/
    __declspec(dllexport) int GetFieldSizeX()
    {
        return flowField.sizeX;
    }
    /*TODO: REMOVE*/
    __declspec(dllexport) int GetFieldSizeY()
    {
        return flowField.sizeY;
    }
    /*TODO: REMOVE*/
    __declspec(dllexport) int GetFieldSizeZ()
    {
        return flowField.sizeZ;
    }
    /*TODO: REMOVE*/
    __declspec(dllexport) float GetFieldCellSize()
    {
        return flowField.cellSize;
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