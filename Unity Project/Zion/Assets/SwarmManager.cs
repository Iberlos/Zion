using System;
using System.Collections;
using System.Runtime.InteropServices;
using UnityEngine;

public enum FieldGizmoMode
{
    None = 0,
    Height,
    Radial,
    Angular,
    Near
}

[StructLayout(LayoutKind.Sequential)]
struct FieldConstructionData
{
    //Cells
    public int radialCells;
    public int heightCells;
    public int angularCells;
    //Shapes
    //External
    public float domeRadius;
    public float cilinderRadius;
    public float cilinderHeight;
    public float coneTrunkRadiusTop;
    public float coneTrunkHeight;
    public float coneTrunkRadiusBottom;
    //Internal
    public float pilarRadius;
    public float pilarHeight;
    public float towerBaseRadius;
    public float towerTrunkRadius;
    public float towerTrunkHeight;
    public float towerTopRadius;
    public float bridgeWidth;

    public FieldConstructionData(
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
        float bridgeWidth
        ) {
        this.radialCells = radialCells;
        this.heightCells = heightCells;
        this.angularCells = angularCells;
        this.domeRadius = domeRadius;
        this.radialCells = radialCells;
        this.heightCells = heightCells;
        this.angularCells = angularCells;
        this.domeRadius = domeRadius;
        this.cilinderRadius = cilinderRadius;
        this.cilinderHeight = cilinderHeight;
        this.coneTrunkRadiusTop = coneTrunkRadiusTop;
        this.coneTrunkHeight = coneTrunkHeight;
        this.coneTrunkRadiusBottom = coneTrunkRadiusBottom;
        this.pilarRadius = pilarRadius;
        this.pilarHeight = pilarHeight;
        this.towerBaseRadius = towerBaseRadius;
        this.towerTrunkRadius = towerTrunkRadius;
        this.towerTrunkHeight = towerTrunkHeight;
        this.towerTopRadius = towerTopRadius;
        this.bridgeWidth = bridgeWidth;
    }

    public FieldConstructionData(FieldConstructionData other)
    {
        this.radialCells = other.radialCells;
        this.heightCells = other.heightCells;
        this.angularCells = other.angularCells;
        this.domeRadius = other.domeRadius;
        this.radialCells = other.radialCells;
        this.heightCells = other.heightCells;
        this.angularCells = other.angularCells;
        this.domeRadius = other.domeRadius;
        this.cilinderRadius = other.cilinderRadius;
        this.cilinderHeight = other.cilinderHeight;
        this.coneTrunkRadiusTop = other.coneTrunkRadiusTop;
        this.coneTrunkHeight = other.coneTrunkHeight;
        this.coneTrunkRadiusBottom = other.coneTrunkRadiusBottom;
        this.pilarRadius = other.pilarRadius;
        this.pilarHeight = other.pilarHeight;
        this.towerBaseRadius = other.towerBaseRadius;
        this.towerTrunkRadius = other.towerTrunkRadius;
        this.towerTrunkHeight = other.towerTrunkHeight;
        this.towerTopRadius = other.towerTopRadius;
        this.bridgeWidth = other.bridgeWidth;

    }

    public bool Equals(FieldConstructionData other)
    {
        return this.radialCells == other.radialCells &&
        this.heightCells == other.heightCells &&
        this.angularCells == other.angularCells &&
        this.domeRadius == other.domeRadius &&
        this.radialCells == other.radialCells &&
        this.heightCells == other.heightCells &&
        this.angularCells == other.angularCells &&
        this.domeRadius == other.domeRadius &&
        this.cilinderRadius == other.cilinderRadius &&
        this.cilinderHeight == other.cilinderHeight &&
        this.coneTrunkRadiusTop == other.coneTrunkRadiusTop &&
        this.coneTrunkHeight == other.coneTrunkHeight &&
        this.coneTrunkRadiusBottom == other.coneTrunkRadiusBottom &&
        this.pilarRadius == other.pilarRadius &&
        this.pilarHeight == other.pilarHeight &&
        this.towerBaseRadius == other.towerBaseRadius &&
        this.towerTrunkRadius == other.towerTrunkRadius &&
        this.towerTrunkHeight == other.towerTrunkHeight &&
        this.towerTopRadius == other.towerTopRadius &&
        this.bridgeWidth == other.bridgeWidth;
    }

    // Override Object.Equals
    public override bool Equals(object obj)
    {
        return obj is FieldConstructionData other && Equals(other);
    }

    // Override GetHashCode
    public override int GetHashCode()
    {
        return radialCells.GetHashCode();
    }

    //Override operators
    public static bool operator ==(FieldConstructionData _this, FieldConstructionData _other)
    {
        return _this.Equals(_other);
    }
    public static bool operator !=(FieldConstructionData _this, FieldConstructionData _other)
    {
        return !_this.Equals(_other);
    }
}

[StructLayout(LayoutKind.Sequential)]
struct FieldPoint
{
    public Vec3 position;
    public Vec3 direction;
}

[StructLayout(LayoutKind.Sequential)]
struct Drone
{
    public Vec3 position;
    public Vec3 velocity;
}

[StructLayout(LayoutKind.Sequential)]
struct Vec3
{
    public float x;
    public float y;
    public float z;

    public Vector3 ToVector3()
    {
        return new Vector3(x, y, z);
    }

    public static Vec3 FromVector3(Vector3 v)
    {
        Vec3 vec3 = new Vec3();
        vec3.x = v.x;
        vec3.y = v.y;
        vec3.z = v.z;
        return vec3;
    }
}

[DisallowMultipleComponent]
public class SwarmManager : MonoBehaviour
{
    //Visualization
    [SerializeField] public FieldGizmoMode gizmoMode = FieldGizmoMode.None;
    [SerializeField, Range(0.1f, 1.0f)] private float arrowSize = 0.2f;
    [SerializeField, Range(0, 256)] private int gizmoHeightLayer = 1;
    [SerializeField, Range(0, 256)] private int gizmoRadialLayer = 1;
    [SerializeField, Range(0, 256)] private int gizmoAngularLayer = 1;
    [SerializeField, Range(0.0f, 100.0f)] private float gizmoViewDistance = 10.0f;
    [SerializeField, Range(0.5f, 2.0f)] private float gizmoMaxArrowSize = 1.0f;
    [SerializeField, Range(0.1f, 1.0f)] private float gizmoMinArrowSize = 0.5f;

    //Field
    //Cells
    [SerializeField, Range(1, 256)] private int radialCells = 64;
    [SerializeField, Range(1, 256)] private int heightCells = 32;
    [SerializeField, Range(1, 128)] private int angularCells = 64;
    //Shapes
    //External
    [SerializeField, Range(0.0f, 1000.0f)] private float domeRadius = 500.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float cilinderRadius = 500.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float cilinderHeight = 100.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float coneTrunkRadiusTop = 500.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float coneTrunkHeight = 100.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float coneTrunkRadiusBottom = 100.0f;
    //Internal
    [SerializeField, Range(0.0f, 1000.0f)] private float pilarRadius = 37.5f;
    [SerializeField, Range(0.0f, 1000.0f)] private float pilarHeight = 200.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float towerBaseRadius = 37.5f;
    [SerializeField, Range(0.0f, 1000.0f)] private float towerTrunkRadius = 12.5f;
    [SerializeField, Range(0.0f, 1000.0f)] private float towerTrunkHeight = 75.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float towerTopRadius = 25.0f;
    [SerializeField, Range(0.0f, 1000.0f)] private float bridgeWidth = 25.0f;
    //Functions
    [ContextMenu("Regenerate Field")]
    public void RegenerateField()
    {
        FieldConstructionData newFieldConstructionData = new FieldConstructionData(
            radialCells,
            heightCells,
            angularCells,
            domeRadius,
            cilinderRadius,
            cilinderHeight,
            coneTrunkRadiusTop,
            coneTrunkHeight,
            coneTrunkRadiusBottom,
            pilarRadius,
            pilarHeight,
            towerBaseRadius,
            towerTrunkRadius,
            towerTrunkHeight,
            towerTopRadius,
            bridgeWidth
        );
        if(newFieldConstructionData != GetFieldConstructionData())
        {
            Debug.Log("Field Generation is in progress...");
            Debug.Log("Field Generation is not implemented yet");
            GenerateField(newFieldConstructionData);
            Debug.Log("Field Generation is complete!");
        }
        else
        {
            Debug.Log("Field Generation was not necessary.");
        }
    }

    //Swarm
    [SerializeField, Range(1, 5000)] private int cpuDroneCount = 200;
    //Drone
    [SerializeField] private GameObject dronePrefab;
    [SerializeField, Range(1.0f, 100.0f)] private float droneSpeedLimit = 10.0f;
    //Flow
    //Field influence
    [SerializeField, Range(0.0f, 100.0f)] private float fieldInfluenceStength = 0.5f;
    //Noise
    [SerializeField, Range(0.0f, 100.0f)] private float noiseOffsetX = 31.34f;
    [SerializeField, Range(0.0f, 100.0f)] private float noiseOffsetY = 47.77f;
    [SerializeField, Range(0.0f, 100.0f)] private float noiseOffsetZ = 12.93f;
    //Curl
    [SerializeField, Range(0.0f, 0.2f)] private float curlE = 0.1f;
    //Turbulence
    [SerializeField] private Vector3 turbStrength = new Vector3(0.1f, 0.1f, 0.1f);

    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    /*TODO: REMOVE*/
    static extern void InitializeField(int x, int y, int z, float cellSize);

    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    static extern void InitializeDrones(int count);

    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    static extern void UpdateSwarm(float dt);

    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr GetDroneArray();

    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    static extern int GetDroneCount();

    [DllImport("SwarmSim")]
    /*TODO: REMOVE*/
    static extern IntPtr GetFieldVectors();

    [DllImport("SwarmSim")]
    /*TODO: REMOVE*/
    static extern int GetFieldSizeX();

    [DllImport("SwarmSim")]
    /*TODO: REMOVE*/
    static extern int GetFieldSizeY();

    [DllImport("SwarmSim")]
    /*TODO: REMOVE*/
    static extern int GetFieldSizeZ();

    [DllImport("SwarmSim")]
    /*TODO: REMOVE*/
    static extern float GetFieldCellSize();

    //Construction
    [DllImport("SwarmSim", CallingConvention = CallingConvention.Cdecl)]
    /*TODO: ADD*/
    static extern void GenerateField(FieldConstructionData data);
    [DllImport("SwarmSim")]
    /*TODO: ADD*/
    static extern FieldConstructionData GetFieldConstructionData();
    [DllImport("SwarmSim")]
    /*TODO: ADD*/
    static extern int GetHeightCellCount();
    [DllImport("SwarmSim")]
    /*TODO: ADD*/
    static extern int GetRadialCellCount();
    [DllImport("SwarmSim")]
    /*TODO: ADD*/
    static extern int GetAngularCellCount();
    [DllImport("SwarmSim")]
    /*TODO: ADD*/
    static extern IntPtr GetField();

    //Noise
    [DllImport("SwarmSim")]
    static extern float GetNoiseOffsetX();
    [DllImport("SwarmSim")]
    static extern void SetNoiseOffsetX(float a_noiseOffsetX);
    [DllImport("SwarmSim")]
    static extern float GetNoiseOffsetY();
    [DllImport("SwarmSim")]
    static extern void SetNoiseOffsetY(float a_noiseOffsetY);
    [DllImport("SwarmSim")]
    static extern float GetNoiseOffsetZ();
    [DllImport("SwarmSim")]
    static extern void SetNoiseOffsetZ(float a_noiseOffsetZ);
    //Curl
    [DllImport("SwarmSim")]
    static extern float GetCurlE();
    [DllImport("SwarmSim")]
    static extern void SetCurlE(float a_curlE);
    //Turbulence
    [DllImport("SwarmSim")]
    static extern Vec3 GetTurbStrength();
    [DllImport("SwarmSim")]
    static extern void SetTurbStrength(Vec3 a_turbStrength);
    //Drone Update
    [DllImport("SwarmSim")]
    static extern float GetFieldInfluenceStength();
    [DllImport("SwarmSim")]
    static extern void SetFieldInfluenceStength(float a_fieldInfluenceStength);
    [DllImport("SwarmSim")]
    static extern float GetDroneSpeedLimit();
    [DllImport("SwarmSim")]
    static extern void SetDroneSpeedLimit(float a_droneSpeedLimit);

    private GameObject[] droneObjects;
    Drone[] droneData;

    void OnValidate()
    {
        SetNoiseOffsetX(noiseOffsetX);
        SetNoiseOffsetY(noiseOffsetY);
        SetNoiseOffsetZ(noiseOffsetZ);

        SetCurlE(curlE);
        SetTurbStrength(Vec3.FromVector3(turbStrength));
        SetFieldInfluenceStength(fieldInfluenceStength);
        SetDroneSpeedLimit(droneSpeedLimit);
    }

    void OnEnable()
    {
        //When the script is activated generate the field in the C++ side
        RegenerateField();
    }

    void Start()
    {
        InitializeField(64, 32, 64, 2.0f);

        InitializeDrones(cpuDroneCount);

        int count = GetDroneCount();

        droneObjects = new GameObject[count];
        droneData = new Drone[count];

        StartCoroutine(SpawnDrones());
    }

    void Update()
    {
        UpdateSwarm(Time.deltaTime);

        int count = GetDroneCount();

        IntPtr ptr = GetDroneArray();

        int size = Marshal.SizeOf(typeof(Drone));

        for (int i = 0; i < count; i++)
        {
            if (droneObjects[i] == null) return;
            IntPtr dronePtr = ptr + i * size;

            droneData[i] =
                Marshal.PtrToStructure<Drone>(dronePtr);

            droneObjects[i].transform.position =
                droneData[i].position.ToVector3();

            droneObjects[i].transform.rotation = Quaternion.FromToRotation(Vector3.forward, droneData[i].velocity.ToVector3().normalized);
        }
    }

    void OnDrawGizmos()
    {
        return;
        if (gizmoMode != FieldGizmoMode.None)
        {
            //Get the number of cells for each radial axis
            int hcc = GetHeightCellCount();
            int rcc = GetRadialCellCount();
            int acc = GetAngularCellCount();
            //Get the array from DLL
            IntPtr ptr = GetField();

            //Draw each vector starting from the top, then circling arround for each radius
            int size = Marshal.SizeOf(typeof(FieldPoint));
            int hpc = hcc + 2; //Height Point Count
            int rpc = rcc + 2; //Radius Point Count
            int apc = acc + 2; //Angle Point Count
            for (int h = 0; h < hpc + 2; h++)
                for (int r = 0; r < rpc + 2; r++)
                    for (int a = 0; a < apc + 2; a++)
                    {
                        //Skip based on mode
                        switch (gizmoMode)
                        {
                            case FieldGizmoMode.Height:
                                {
                                    if (h != gizmoHeightLayer) continue;
                                    break;
                                }
                            case FieldGizmoMode.Radial:
                                {
                                    if (r != gizmoRadialLayer) continue;
                                    break;
                                }
                            case FieldGizmoMode.Angular:
                                {
                                    if (a != gizmoAngularLayer) continue;
                                    break;
                                }
                        }
                        //Find the index
                        int index = a + r * (apc + 2) + h * (apc + 2) * (rpc + 2);
                        //Find the address in the array
                        IntPtr vecPtr = ptr + index * size;
                        //Convert it to a Field Point
                        FieldPoint p = Marshal.PtrToStructure<FieldPoint>(vecPtr);
                        //Variable Grooming
                        Vector3 pos = p.position.ToVector3();
                        Vector3 dir = p.direction.ToVector3();
                        float arrowS = arrowSize;
                        Transform camera = UnityEditor.SceneView.lastActiveSceneView.camera.transform;
                        //Near Case
                        switch (gizmoMode)
                        {
                            case FieldGizmoMode.Near:
                                {
                                    float sqrDist = (camera.position - pos).sqrMagnitude;
                                    float sqrViewDist = gizmoViewDistance * gizmoViewDistance;
                                    if (sqrDist > sqrViewDist)
                                    {
                                        continue;
                                    }
                                    else
                                    {
                                        arrowS = gizmoMinArrowSize + sqrDist / sqrViewDist * (gizmoMaxArrowSize-gizmoMinArrowSize);
                                    }
                                    break;
                                }
                        }
                        //Draw Arrow
                        DrawArrow.ForGizmo(
                        pos,
                        dir,
                        new Color(dir.x, dir.y, dir.z, 1.0f),
                        arrowS
                        );
                    }
        }
    }

    IEnumerator SpawnDrones()
    {
        int count = GetDroneCount();

        droneObjects = new GameObject[count];
        //droneTransforms = new Transform[count];

        //float delay = 0.01f; // 10 milliseconds

        for (int i = 0; i < count; i++)
        {
            GameObject obj =
                Instantiate(dronePrefab, Vector3.zero, Quaternion.identity);

            droneObjects[i] = obj;
            //droneTransforms[i] = obj.transform;

            Debug.Log("Spawned drone: " + i);

            if(i %100 == 0)
            {
                yield return null;
            }
            //yield return new WaitForSeconds(delay);
        }
    }
}