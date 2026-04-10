#include "pch.h"
#include <cassert>
#include <stdio.h>
#include <string>
#include <math.h>
#include <random>
#include "PerlinNoise.hpp"

const double PI = 3.14159265358979323846;

// Global flag to toggle output
bool verbose = false;

template<typename... Args>
void log(const char* format, Args... args) {
    if (verbose) {
        printf(format, args...);
        printf("\n");
    }
}

//General Structures
struct Vec3
{
    Vec3():x(0.0f),y(0.0f),z(0.0f){}
    Vec3(float x, float y, float z): x(x), y(y), z(z) {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}
    float x;
    float y;
    float z;

    std::string ToString() { return std::string(std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z)); }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Vec3& other) const {
        return !((*this)==other);
    }
    Vec3 operator+(const Vec3& other) const {
        return Vec3( x + other.x, y + other.y, z + other.z);
    }
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    float operator*(const Vec3& other) const {
        return sqrt(x * other.x + y * other.y + z * other.z);
    }
    Vec3 operator*(const float& other) const {
        return Vec3(x * other, y * other, z * other);
    }
    Vec3 operator/(const float& other) const {
        return Vec3(x / other, y / other, z / other);
    }
    Vec3 operator*(const int& other) const {
        return *this * ((float)other);
    }
    Vec3 operator/(const int& other) const {
        return *this / ((float)other);
    }
    Vec3 Normalized() {
        return Vec3(*this)/ mag();
    }
    float sqrMag() {
        return x * x + y * y + z * z;
    }
    float mag() {
        return sqrt(sqrMag());
    }
};

struct RadialVec3
{
    RadialVec3() :h(0.0f), r(0.0f), a(0.0f) { AlignA(); }
    RadialVec3(float h, float r, float a) : h(h), r(r), a(a) { AlignA(); }
    RadialVec3(const RadialVec3& other) : h(other.h), r(other.r), a(other.a) { AlignA(); }
    RadialVec3(const Vec3& vec3) : h(vec3.y), r(sqrt(vec3.x* vec3.x + vec3.z * vec3.z)), a(atan2(vec3.x, vec3.z)) { AlignA(); }
    float h;
    float r;
    float a;
    Vec3 ToVec3() { return Vec3(sin(a)*r, h, cos(a)*r); }
    std::string ToString() { return std::string(std::to_string(h) + "," + std::to_string(r) + "," + std::to_string(a)); }

    bool operator==(const RadialVec3& other) const {
        return h == other.h && r == other.r && a == other.a;
    }
    bool operator!=(const RadialVec3& other) const {
        return !((*this) == other);
    }
    RadialVec3 operator+(const RadialVec3& other) const {
        return RadialVec3(h + other.h, r + other.r, a + other.a);
    }
    RadialVec3 operator-(const RadialVec3& other) const {
        return RadialVec3(h - other.h, r - other.r, a - other.a);
    }
    float operator*(const RadialVec3& other) const {
        return sqrt(h * other.h + r * other.r + a * other.a);
    }
    RadialVec3 operator*(const float& other) const {
        return RadialVec3(h * other, r * other, a * other);
    }
    RadialVec3 operator/(const float& other) const {
        return RadialVec3(h / other, r / other, a / other);
    }
    RadialVec3 operator*(const int& other) const {
        return *this * ((float)other);
    }
    RadialVec3 operator/(const int& other) const {
        return *this / ((float)other);
    }
    RadialVec3 Normalized() {
        return RadialVec3(*this) / mag();
    }
    float sqrMag() {
        return h * h + r * r + a * a;
    }
    float mag() {
        return sqrt(sqrMag());
    }

    //Add an angle to the original vector
    RadialVec3& AlignA() {
        float& a = (*this).a;
        a = std::fmod(a, 2 * PI);
        if (a < 0.0f)
            a += 2 * PI;
        return *this;
    }
    RadialVec3& AddAngle(float a) {
        (*this).a += a;
        return *this;
    }
    //Turns the original vector 90 degrees clockwise
    RadialVec3& TurnRight() { return AddAngle((float)(-PI / 2)).AlignA(); }
    //Turns the original vector 90 degrees counter-clockwise
    RadialVec3& TurnLeft() { return AddAngle((float)(PI / 2)).AlignA(); }
    //Gets a new vector that is turned 90 degrees clockwise
    RadialVec3 RightTurn() { return RadialVec3(*this).TurnRight(); }
    //Gets a new vector that is turned 90 degrees counter-clockwise
    RadialVec3 LeftTurn() { return RadialVec3(*this).TurnLeft(); }
    //Gets a new vector containing only the variable needed
    RadialVec3 H() { return RadialVec3((*this).h, 1.0f, 0.0f); }
    RadialVec3 R() { return RadialVec3(0.0f, (*this).r, 0.0f); }
    RadialVec3 A() { return RadialVec3(0.0f, 1.0f, (*this).a); }
    //Gets a new vector pointing in the direction required based on the original vector
    RadialVec3 RadialIn() { return RadialVec3(0.0f, 1.0f, (*this).a); }
    RadialVec3 RadialOut() { return RadialVec3(0.0f, -1.0f, (*this).a); }
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

    void PrintLog(std::string* addTo = nullptr) {
        if (!verbose) return;
        std::string logMessage = std::string(
            addTo == nullptr ? "SwarmSim - ConstructionData Log:\n\n" : "\n\nConstructionData Log:\n"
            "radialCells = " + std::to_string(radialCells) + "\n"
            "heightCells = " + std::to_string(heightCells) + "\n"
            "angularCells = " + std::to_string(angularCells) + "\n"
            "domeRadius = " + std::to_string(domeRadius) + "\n"
            "cilinderRadius = " + std::to_string(cilinderRadius) + "\n"
            "cilinderHeight = " + std::to_string(cilinderHeight) + "\n"
            "coneTrunkRadiusTop = " + std::to_string(coneTrunkRadiusTop) + "\n"
            "coneTrunkHeight = " + std::to_string(coneTrunkHeight) + "\n"
            "coneTrunkRadiusBottom = " + std::to_string(coneTrunkRadiusBottom) + "\n"
            "pilarRadius = " + std::to_string(pilarRadius) + "\n"
            "pilarHeight = " + std::to_string(pilarHeight) + "\n"
            "towerBaseRadius = " + std::to_string(towerBaseRadius) + "\n"
            "towerTrunkRadius = " + std::to_string(towerTrunkRadius) + "\n"
            "towerTrunkHeight = " + std::to_string(towerTrunkHeight) + "\n"
            "towerTopRadius = " + std::to_string(towerTopRadius) + "\n"
            "bridgeWidth = " + std::to_string(bridgeWidth) + "\n"
        );

        if (addTo != nullptr)
        {
            *addTo += logMessage;
        }
        else
        {
            log(logMessage.c_str());
        }
    }
};

struct FieldPoint
{
    Vec3 position;
    Vec3 direction;

    std::string ToString(int printPosOrDir = 0) { 
        return std::string((printPosOrDir>=0?("P:" + position.ToString()):"") + (printPosOrDir<=0?(" D:" + direction.ToString()):""));
    }
};

struct FieldPlane
{
    FieldPlane(
        FieldPoint& pMin, 
        FieldPoint& pFirst, 
        FieldPoint& pSecond, 
        FieldPoint& pMax
    ): pMin(pMin), 
       pFirst(pFirst), 
       pSecond(pSecond), 
       pMax(pMax)
    {}
    /*
    **READ**
    This struct holds references to FieldPoints that create a quad.
    This quad can be oriented in any cardinal direction in the radial field (or a cardinal field if one is implemented),
    so in order to keep naming generec the following convention was created:
    pSecond ___ pMax
            |\|
       pMin --- pFirst

    pMin -> holds the point withe the smallest position values. On a 2d plane that would be the smallest x && y. For example (0,0)
    pFirst -> holds the first point used to create a tri in a conventional quad. On a 2d plane that would be the smallest y and second smallest x. For example (1,0)
    pSecond -> holds the second point used to create a tri in a conventional quad. On a 2d plane that would be the smallest x and second smallest y. For example (0,1)
    pMax -> holds the point with the highest position values. On a 2d plane that would be the largest x && y. for example (1,1)

    OBS: The creation of the default cell planes in the FieldCell struct may be a good way to understand this struct in 2.5D Radial coordinates. 
    */
    FieldPoint& pMin;
    FieldPoint& pFirst;
    FieldPoint& pSecond;
    FieldPoint& pMax;
};

struct FieldCell
{
    FieldCell(
        FieldPoint& p000,
        FieldPoint& p100, 
        FieldPoint& p010,
        FieldPoint& p110,
        FieldPoint& p001,
        FieldPoint& p101,
        FieldPoint& p011,
        FieldPoint& p111
    ) : p000(p000),
        p100(p100),
        p010(p010),
        p110(p110),
        p001(p001),
        p101(p101),
        p011(p011),
        p111(p111)
    {}

    //Counter Clockwise plane
    FieldPoint& p000;
    FieldPoint& p100;
    FieldPoint& p010;
    FieldPoint& p110;
    //Clockwise Plane
    FieldPoint& p001;
    FieldPoint& p101;
    FieldPoint& p011;
    FieldPoint& p111;

    //Default planes (Naming convention seen in radial direction
    FieldPlane FrontPlane() { return FieldPlane(p000,p001,p100,p101); }
    FieldPlane BackPlane() { return FieldPlane(p010, p011, p110, p111); }
    FieldPlane TopPlane() { return FieldPlane(p100, p101, p110, p111); }
    FieldPlane BottomPlane() { return FieldPlane(p000, p001, p010, p011); }
    FieldPlane RightPlane() { return FieldPlane(p000, p010, p100, p110); }
    FieldPlane LeftPlane() { return FieldPlane(p001, p011, p101, p111); }
    //Diagonals (Probably not used)
    FieldPlane DiagonalAPos() { return FieldPlane(p000, p010, p101, p111); }
    FieldPlane DiagonalANeg() { return FieldPlane(p001, p011, p100, p110); }
    FieldPlane DiagonalRPos() { return FieldPlane(p000, p001, p110, p111); }
    FieldPlane DiagonalRNeg() { return FieldPlane(p010, p011, p100, p101); }
    //Axis limits
    float Top() { return p111.position.y; }
    float Bottom() { return p000.position.y; }

    void PrintLog(int printTopOrBottom = 0, int printPosOrDir = 0, std::string* addTo = nullptr) {
        if (!verbose) return;
        std::string logMessage = "";
        //Print top
        if(printTopOrBottom >= 0)logMessage += std::string(
            "    " + p111.ToString(printPosOrDir) + "    " + p110.ToString(printPosOrDir) + "\n"
            "" + p101.ToString(printPosOrDir) + "    " + p100.ToString(printPosOrDir) + "\n"
        );
        //Print bottom
        if (printTopOrBottom <= 0)logMessage += std::string(
            "    " + p011.ToString(printPosOrDir) + "    " + p010.ToString(printPosOrDir) + "\n"
            "" + p001.ToString(printPosOrDir) + "    " + p000.ToString(printPosOrDir) + "\n"
        );

        if (addTo != nullptr)
        {
            *addTo += logMessage;
        }
        else
        {
            log(logMessage.c_str());
        }
    }
};

struct RadialField
{
    FieldConstructionData* constructionData = nullptr;
    float* heightValues = nullptr;
    float* radialValuesPerHeightValue = nullptr;
    float* angularValues = nullptr;
    FieldPoint* fieldPoints = nullptr;

    FieldCell FindCellFromVec3(Vec3 position)
    {
        //Turn cardinal position into 2.5d position
        RadialVec3 rp(position);
        log("Cardinal postion converted to radial position %f, %f, %f.\n", rp.h, rp.r, rp.a);
        //Find coordinates for each radial axis for position
        int heightPointCount = constructionData->heightCells + 1;
        int radialPointCount = constructionData->radialCells + 1;
        int angularPointCount = constructionData->angularCells + 1;


        int hiPrevious = 0;
        int hiNext = heightPointCount-1;
        log("\nBinary Height Search starting with P=%d,N=%d,S=%f", hiPrevious, hiNext, rp.h);
        BinaryIndexSearch(rp.h, heightValues, heightPointCount, hiPrevious, hiNext, false);
        int riPrevious = 0;
        int riNext = radialPointCount-1;
        log("\nBinary Radial Search starting with P=%d,N=%d,S=%f,Bi=%d", riPrevious, riNext, rp.r, hiPrevious);
        BinaryIndexSearch(rp.r, radialValuesPerHeightValue, radialPointCount * heightPointCount, riPrevious, riNext, true, hiPrevious); //TODO: use closest hi value not previous
        int aiPrevious = 0;
        int aiNext = angularPointCount-1;
        log("\nBinary Angular Search starting with P=%d,N=%d,S=%f", aiPrevious, aiNext, rp.a);
        BinaryIndexSearch(rp.a, angularValues, angularPointCount, aiPrevious, aiNext);
        log("\nBinary search returned indexes:\nH: %d, %d\nR: %d, %d\nA: %d, %d\n", hiPrevious, hiNext, riPrevious, riNext, aiPrevious, aiNext);
        //Create FieldCell
        int maxIndex = hiNext * radialPointCount * angularPointCount + riNext * angularPointCount + aiNext;
        assert(maxIndex < heightPointCount * radialPointCount * angularPointCount && "maxIndex out of bounds!");
        return FieldCell(
            /*p000*/fieldPoints[hiPrevious * radialPointCount * angularPointCount + riPrevious * angularPointCount + aiPrevious],
            /*p100*/fieldPoints[hiNext * radialPointCount * angularPointCount + riPrevious * angularPointCount + aiPrevious],
            /*p010*/fieldPoints[hiPrevious * radialPointCount * angularPointCount + riNext * angularPointCount + aiPrevious],
            /*p110*/fieldPoints[hiNext * radialPointCount * angularPointCount + riNext * angularPointCount + aiPrevious],
            /*p001*/fieldPoints[hiPrevious * radialPointCount * angularPointCount + riPrevious * angularPointCount + aiNext],
            /*p101*/fieldPoints[hiNext * radialPointCount * angularPointCount + riPrevious * angularPointCount + aiNext],
            /*p011*/fieldPoints[hiPrevious * radialPointCount * angularPointCount + riNext * angularPointCount + aiNext],
            /*p111*/fieldPoints[hiNext * radialPointCount * angularPointCount + riNext * angularPointCount + aiNext]
        );
    }

    //Helper function to find the indexes for each axis right below and right above the value
    void BinaryIndexSearch(float searchValue, float* values, int valuesSize, int& iPreviousOut, int& iNextOut, bool ascendingArray = true,  int baseIndex = 0) {
        //If there is a base index align with it
        if (baseIndex != 0)
        {
            iPreviousOut += baseIndex;
            iNextOut += baseIndex;
            log("\nBase index present -> P=%d,N=%d", iPreviousOut, iNextOut);
        }
        //Check if the array is in descending order or ascending order
        bool AscendingLogic = ascendingArray;
        //If the search value is smaller than the minimum value force iNextOut and exit
        if (AscendingLogic ? searchValue < values[iPreviousOut] : searchValue > values[iPreviousOut])
        {
            iNextOut = iPreviousOut;
            log("\nS=%f %s V[P]=%f ->  P=%d,N=%d", searchValue, AscendingLogic?"<":">", values[iPreviousOut], iPreviousOut, iNextOut);
            //If the logic was flipped the indexes need to be flipped, since values[iNextOut] > values[iPreviousOut] has to be true for cell creation
            if (!AscendingLogic) {
                int n = iNextOut;
                int p = iPreviousOut;
                iPreviousOut = n;
                iNextOut = p;
                log("\nReturning with logic flipped -> P=%d,N=%d", iPreviousOut, iNextOut);
            }
            //If there is a base index de-align with it before returning
            if (baseIndex != 0)
            {
                iPreviousOut -= baseIndex;
                iNextOut -= baseIndex;
                log("\nBase index present -> P=%d,N=%d", iPreviousOut, iNextOut);
            }
            return;
        }
        //If the search value is bigger than the maximum value force iPreviousOut and exit
        if (AscendingLogic ? searchValue > values[iNextOut] : searchValue < values[iNextOut])
        {
            iPreviousOut = iNextOut;
            log("\nS=%f %s V[N]=%f ->  P=%d,N=%d", searchValue, AscendingLogic ? ">" : "<", values[iNextOut], iPreviousOut, iNextOut);
            //If the logic was flipped the indexes need to be flipped, since values[iNextOut] > values[iPreviousOut] has to be true for cell creation
            if (!AscendingLogic) {
                int n = iNextOut;
                int p = iPreviousOut;
                iPreviousOut = n;
                iNextOut = p;
                log("\nReturning with logic flipped -> P=%d,N=%d", iPreviousOut, iNextOut);
            }
            //If there is a base index de-align with it before returning
            if (baseIndex != 0)
            {
                iPreviousOut -= baseIndex;
                iNextOut -= baseIndex;
                log("\nBase index present -> P=%d,N=%d", iPreviousOut, iNextOut);
            }
            return;
        }
        //Binary search
        int step = 0;
        int searchIndex;
        while (iPreviousOut < iNextOut - 1) {
            step++;
            log("\nStep %d", step);
            searchIndex = iPreviousOut + (int)floor((iNextOut - iPreviousOut) / 2.0f);
            assert(searchIndex < valuesSize && searchIndex >= 0 && "searchIndex out of bounds!");
            float currentValue = values[searchIndex];
            if (AscendingLogic ? searchValue < currentValue : searchValue > currentValue) {
                iNextOut = searchIndex;
                log("\nS=%f %s V[S=%d]=%f ->  P=%d,N=%d", searchValue, AscendingLogic ? "<" : ">", searchIndex, values[searchIndex], iPreviousOut, iNextOut);
            }
            else if(AscendingLogic ? searchValue > currentValue : searchValue < currentValue){
                iPreviousOut = searchIndex;
                log("\nS=%f %s V[S=%d]=%f ->  P=%d,N=%d", searchValue, AscendingLogic ? ">" : "<", searchIndex, values[searchIndex], iPreviousOut, iNextOut);
            }
            else {
                //Handle sameness although it should not be possible
                iPreviousOut = searchIndex;
                iNextOut = searchIndex + 1;
                log("\nS=%f = V[S=%d]=%f ->  P=%d,N=%d", searchValue, searchIndex, values[searchIndex], iPreviousOut, iNextOut);
            }
            searchIndex = iPreviousOut + (int)floor((iNextOut-iPreviousOut) / 2.0f);
        }
        //If the logic was flipped the indexes need to be flipped, since values[iNextOut] > values[iPreviousOut] has to be true for cell creation
        if (!AscendingLogic) {
            int n = iNextOut;
            int p = iPreviousOut;
            iPreviousOut = n;
            iNextOut = p;
            log("\nReturning with logic flipped -> P=%d,N=%d", iPreviousOut, iNextOut);
        }
        //If there is a base index de-align with it before returning
        if (baseIndex != 0)
        {
            iPreviousOut -= baseIndex;
            iNextOut -= baseIndex;
            log("\nBase index present -> P=%d,N=%d", iPreviousOut, iNextOut);
        }
    }

    void PrintLog(std::string* addTo = nullptr) {
        if (!verbose) return;
        std::string logMessage = std::string(addTo == nullptr ? "SwarmSim Log:\n\n" : "");
        constructionData->PrintLog(&logMessage);

        int heightValueCount = constructionData->heightCells + 1;
        int radialValueCount = constructionData->radialCells + 1;
        int radialValuePerHeightCount = heightValueCount * radialValueCount;
        int angularValueCount = constructionData->angularCells + 1;
        std::string heightAndRadialValuesText = "";
        for (int hi = 0; hi < heightValueCount; hi++)
        {
            heightAndRadialValuesText += "H: " + std::to_string(heightValues[hi]) + " -> R:";
            for (int ri = 0; ri < radialValueCount; ri++)
            {
                heightAndRadialValuesText += std::to_string(radialValuesPerHeightValue[hi * radialValueCount + ri]) + " ,";
            }
            heightAndRadialValuesText += "\n";
        }
        std::string angularValuesText = "A: ";
        for (int ai = 0; ai < angularValueCount; ai++)
        {
            angularValuesText += std::to_string(angularValues[ai]) + " ,";
        }
        angularValuesText += "\n";
        int fieldPointCount = heightValueCount * radialValueCount * angularValueCount;

        logMessage += std::string(
            "\n\nField Log:\n"
            "heightValueCount = " + std::to_string(heightValueCount) + "\n"
            "radialValueCount = " + std::to_string(radialValueCount) + "\n"
            "angularValueCount = " + std::to_string(angularValueCount) + "\n"
            + heightAndRadialValuesText.c_str()
            + angularValuesText.c_str() +
            "fieldPointCount = " + std::to_string(fieldPointCount) + "\n"
        );

        if (addTo != nullptr)
        {
            *addTo += logMessage;
        }
        else
        {
            log(logMessage.c_str());
        }
    }
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

Vec3 SampleField(Vec3 dp, int id)
{
    log("Sampling Field for cardinal position %f, %f, %f...\n", dp.x, dp.y, dp.z);
    //Find cell in which drone is in
    FieldCell fc = field.FindCellFromVec3(dp);
    if (verbose) {
        std::string cellLog = "Currently sampling cell:";
        if (dp.y > fc.Top()) cellLog += "\n---------------------> DP:" + dp.ToString() + "\n";
        fc.PrintLog(1, 1, &cellLog);
        if (dp.y < fc.Top() && dp.y > fc.Bottom())cellLog += "---------------------> DP:" + dp.ToString() + "\n";
        fc.PrintLog(-1, 1, &cellLog);
        if (dp.y < fc.Bottom()) cellLog += "---------------------> DP:" + dp.ToString() + "\n";
        log(cellLog.c_str());
    }

    //Must take care with division by zero in collapsed cells, zero value assumed
    float t = 0.0f;
    if (fc.p111.position != fc.p000.position) { 
        t = (dp-fc.p000.position).sqrMag()/(fc.p111.position-fc.p000.position).sqrMag(); 
    }
    log("\n\nT:%f", t);

    //Lerp directions
    auto lerp = [](Vec3 a, Vec3 b, float t)
        {
            return Vec3{
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            };
        };

    Vec3 c00 = lerp(fc.p000.direction, fc.p100.direction, t);
    Vec3 c10 = lerp(fc.p010.direction, fc.p110.direction, t);
    Vec3 c01 = lerp(fc.p001.direction, fc.p101.direction, t);
    Vec3 c11 = lerp(fc.p011.direction, fc.p111.direction, t);
    log("\n\nc00:%s\nc10:%s\nc01:%s\nc11:%s\n", c00.ToString().c_str(), c10.ToString().c_str(), c01.ToString().c_str(), c11.ToString().c_str());

    Vec3 c0 = lerp(c00, c10, t);
    Vec3 c1 = lerp(c01, c11, t);
    log("\n\nc0:%s\nc1:%s", c0.ToString().c_str(), c1.ToString().c_str());

    Vec3 base = lerp(c0, c1, t);
    log("\n\nbase:%s", base.ToString().c_str());

    //Curl
    Vec3 curl = CurlNoise(dp.x * 0.01f, dp.y * 0.01f, dp.z * 0.01f, id);

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
    log("Updating drone %d...\n", id);
    Vec3 flow = SampleField(
        d.position,
        id
    );
    log("\n\nflow:%s", flow.ToString().c_str());

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
    log("\n\nd.velocity:%s", d.velocity.ToString().c_str());

    d.position.x += d.velocity.x * dt;
    d.position.y += d.velocity.y * dt;
    d.position.z += d.velocity.z * dt;
    log("\n\nd.position:%s", d.position.ToString().c_str());
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
        if (field.heightValues != nullptr) delete[] field.heightValues;
        field.heightValues = new float[heightVerts];
        float* heightValues = field.heightValues;
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
        if (field.radialValuesPerHeightValue != nullptr) delete[] field.radialValuesPerHeightValue;
        field.radialValuesPerHeightValue = new float[heightVerts * radialVerts];
        float* radialValuesPerHeightValue = field.radialValuesPerHeightValue;
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
        float cellAngularDelta = (float)((2 * PI) / field.constructionData->angularCells);
        if (field.angularValues != nullptr) delete[] field.angularValues;
        field.angularValues = new float[angularVerts];
        float* angularValues = field.angularValues;
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
                    RadialVec3 radialPos = RadialVec3(heightValues[hi], radialValuesPerHeightValue[hi * radialVerts + ri], angularValues[ai]);
                    field.fieldPoints[index].position = radialPos.ToVec3();
                    
                    Vec3 dir;
                    if(ri != 0 && ri != radialVerts-1 && hi != 0 && hi != heightVerts -1)
                        dir = radialPos.RightTurn().A().ToVec3();
                    else {
                    
                        if (hi == 0) { //If on the top, point down
                            dir = Vec3(0.0f, -1.0f, 0.0f);
                        }
                        else if (hi == heightVerts - 1) { //If on the bottom point up
                            dir = Vec3(0.0f, 1.0f, 0.0f);
                        }
                        else if (ri == 0) { //If on internal radial limit point out
                            dir = radialPos.RadialIn().ToVec3();
                        }
                        else if (ri == radialVerts - 1) { //If on external radial limit point out
                            dir = radialPos.RadialOut().ToVec3();
                        }
                    }
                    field.fieldPoints[index].direction = dir;
                }

        //Log field status
        field.PrintLog();
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
        log("\n\nSwarmSim - SwarmUpdate START\n");
        for (int i = 0; i < droneCount; i++)
        {
            UpdateDrone(drones[i], dt, i);
        }
        log("\nSwarmSim - SwarmUpdate END\n");
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