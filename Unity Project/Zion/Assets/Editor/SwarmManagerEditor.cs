using UnityEditor;
using UnityEngine;


[CustomEditor(typeof(SwarmManager))]
public class SwarmManagerEditor : Editor
{
    SerializedProperty gizmoMode;
    SerializedProperty gizmoHeightLayer;
    SerializedProperty gizmoRadialLayer;
    SerializedProperty gizmoAngularLayer;
    SerializedProperty gizmoViewDistance;
    SerializedProperty gizmoMaxArrowSize;
    SerializedProperty gizmoMinArrowSize;

    bool fieldDirty = false;
    SerializedProperty radialCells;
    SerializedProperty heightCells;
    SerializedProperty angularCells;

    SerializedProperty domeRadius;
    SerializedProperty cilinderRadius;
    SerializedProperty cilinderHeight;
    SerializedProperty coneTrunkRadiusTop;
    SerializedProperty coneTrunkHeight;
    SerializedProperty coneTrunkRadiusBottom;

    SerializedProperty pilarRadius;
    SerializedProperty pilarHeight;
    SerializedProperty towerBaseRadius;
    SerializedProperty towerTrunkRadius;
    SerializedProperty towerTrunkHeight;
    SerializedProperty towerTopRadius;
    SerializedProperty bridgeWidth;

    SerializedProperty cpuDroneCount;
    SerializedProperty dronePrefab;
    SerializedProperty droneSpeedLimit;
    SerializedProperty fieldInfluenceStength;
    SerializedProperty noiseOffsetX;
    SerializedProperty noiseOffsetY;
    SerializedProperty noiseOffsetZ;
    SerializedProperty curlE;
    SerializedProperty turbStrength;

    void OnEnable()
    {
        gizmoMode = serializedObject.FindProperty("gizmoMode");
        gizmoHeightLayer = serializedObject.FindProperty("gizmoHeightLayer");
        gizmoRadialLayer = serializedObject.FindProperty("gizmoRadialLayer");
        gizmoAngularLayer = serializedObject.FindProperty("gizmoAngularLayer");
        gizmoViewDistance = serializedObject.FindProperty("gizmoViewDistance");
        gizmoMaxArrowSize = serializedObject.FindProperty("gizmoMaxArrowSize");
        gizmoMinArrowSize = serializedObject.FindProperty("gizmoMinArrowSize");

        radialCells = serializedObject.FindProperty("radialCells");
        heightCells = serializedObject.FindProperty("heightCells");
        angularCells = serializedObject.FindProperty("angularCells");

        domeRadius = serializedObject.FindProperty("domeRadius");
        cilinderRadius = serializedObject.FindProperty("cilinderRadius");
        cilinderHeight = serializedObject.FindProperty("cilinderHeight");
        coneTrunkRadiusTop = serializedObject.FindProperty("coneTrunkRadiusTop");
        coneTrunkHeight = serializedObject.FindProperty("coneTrunkHeight");
        coneTrunkRadiusBottom = serializedObject.FindProperty("coneTrunkRadiusBottom");
        pilarRadius = serializedObject.FindProperty("pilarRadius");
        pilarHeight = serializedObject.FindProperty("pilarHeight");
        towerBaseRadius = serializedObject.FindProperty("towerBaseRadius");
        towerTrunkRadius = serializedObject.FindProperty("towerTrunkRadius");
        towerTrunkHeight = serializedObject.FindProperty("towerTrunkHeight");
        towerTopRadius = serializedObject.FindProperty("towerTopRadius");
        bridgeWidth = serializedObject.FindProperty("bridgeWidth");

        cpuDroneCount = serializedObject.FindProperty("cpuDroneCount");
        dronePrefab = serializedObject.FindProperty("dronePrefab");
        droneSpeedLimit = serializedObject.FindProperty("droneSpeedLimit");
        fieldInfluenceStength = serializedObject.FindProperty("fieldInfluenceStength");
        noiseOffsetX = serializedObject.FindProperty("noiseOffsetX");
        noiseOffsetY = serializedObject.FindProperty("noiseOffsetY");
        noiseOffsetZ = serializedObject.FindProperty("noiseOffsetZ");
        curlE = serializedObject.FindProperty("curlE");
        turbStrength = serializedObject.FindProperty("turbStrength");
    }

    public override void OnInspectorGUI()
    {
        //Find script
        SwarmManager generator = (SwarmManager)target;

        //Update serialized fields
        serializedObject.Update();

        //Visualization
        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("GUI", EditorStyles.boldLabel);

        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(gizmoMode);
        switch (generator.gizmoMode)
        {
            case FieldGizmoMode.Height:
                {
                    EditorGUILayout.PropertyField(gizmoHeightLayer);
                    break;
                }
            case FieldGizmoMode.Radial:
                {
                    EditorGUILayout.PropertyField(gizmoRadialLayer);
                    break;
                }
            case FieldGizmoMode.Angular:
                {
                    EditorGUILayout.PropertyField(gizmoAngularLayer);
                    break;
                }
            case FieldGizmoMode.Near:
                {
                    EditorGUILayout.PropertyField(gizmoViewDistance);
                    EditorGUILayout.PropertyField(gizmoMaxArrowSize);
                    EditorGUILayout.PropertyField(gizmoMinArrowSize);
                    break;
                }
        }
        EditorGUILayout.EndVertical();

        //Field
        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Field", EditorStyles.boldLabel);

        //Detect changes in Field Variables
        EditorGUI.BeginChangeCheck();

        EditorGUILayout.BeginVertical("box");
        if (!Application.isPlaying)
        {
            EditorGUILayout.Space(5);
            EditorGUILayout.LabelField("Cells", EditorStyles.boldLabel);

            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.PropertyField(radialCells);
            EditorGUILayout.PropertyField(heightCells);
            EditorGUILayout.PropertyField(angularCells);
            EditorGUILayout.EndVertical();

            EditorGUILayout.Space(5);
            EditorGUILayout.LabelField("Shapes", EditorStyles.boldLabel);

            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.Space(5);
            EditorGUILayout.LabelField("Internal", EditorStyles.boldLabel);
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.PropertyField(domeRadius);
            EditorGUILayout.PropertyField(cilinderRadius);
            EditorGUILayout.PropertyField(cilinderHeight);
            EditorGUILayout.PropertyField(coneTrunkRadiusTop);
            EditorGUILayout.PropertyField(coneTrunkHeight);
            EditorGUILayout.PropertyField(coneTrunkRadiusBottom);
            EditorGUILayout.EndVertical();

            EditorGUILayout.Space(5);
            EditorGUILayout.LabelField("External", EditorStyles.boldLabel);
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.PropertyField(pilarRadius);
            EditorGUILayout.PropertyField(pilarHeight);
            EditorGUILayout.PropertyField(towerBaseRadius);
            EditorGUILayout.PropertyField(towerTrunkRadius);
            EditorGUILayout.PropertyField(towerTrunkHeight);
            EditorGUILayout.PropertyField(towerTopRadius);
            EditorGUILayout.PropertyField(bridgeWidth);
            EditorGUILayout.EndVertical();
            EditorGUILayout.EndVertical();
            if (EditorGUI.EndChangeCheck())
            {
                fieldDirty = true;
            }
            EditorGUILayout.Space();
            if(fieldDirty)
            {
                EditorGUILayout.HelpBox("Warning: Field regeneration required for changes to apply.", MessageType.Warning);
                if (GUILayout.Button("Regenerate"))
                {
                    fieldDirty = false;
                    generator.RegenerateField();
                }
            }
        }
        else
        {
            EditorGUILayout.HelpBox("Warning: Connot edit field during at run time.", MessageType.Warning);
        }
        EditorGUILayout.EndVertical();

        //Swarm
        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Swarm", EditorStyles.boldLabel);

        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(cpuDroneCount);
        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Drone", EditorStyles.boldLabel);
        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(dronePrefab);
        EditorGUILayout.PropertyField(droneSpeedLimit);
        EditorGUILayout.EndVertical();

        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Field Influence", EditorStyles.boldLabel);
        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(fieldInfluenceStength);
        EditorGUILayout.EndVertical();

        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Noise", EditorStyles.boldLabel);
        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(noiseOffsetX);
        EditorGUILayout.PropertyField(noiseOffsetY);
        EditorGUILayout.PropertyField(noiseOffsetZ);
        EditorGUILayout.EndVertical();

        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Curl", EditorStyles.boldLabel);
        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(curlE);
        EditorGUILayout.EndVertical();

        EditorGUILayout.Space(5);
        EditorGUILayout.LabelField("Turbulence", EditorStyles.boldLabel);
        EditorGUILayout.BeginVertical("box");
        EditorGUILayout.PropertyField(turbStrength);
        EditorGUILayout.EndVertical();
        EditorGUILayout.EndVertical();


        //Apply serialized fields
        serializedObject.ApplyModifiedProperties();
    }
}
