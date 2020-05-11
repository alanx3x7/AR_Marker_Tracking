using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

// Define the struture to be sequential and with the correct byte size (1 int = 2 bytes, 1 float = 4 bytes)
[StructLayout(LayoutKind.Sequential, Size = 62)]
public struct Marker2
{
    public int id;
    public float dist;
    public float center_x;
    public float center_y;
    public float translate_x, translate_y, translate_z;
    public float rotate_11, rotate_12, rotate_13; 
    public float rotate_21, rotate_22, rotate_23;
    public float rotate_31, rotate_32, rotate_33;
}

public class DisplayWebCam2 : MonoBehaviour
{
    // Import the Library
    [DllImport("Marker_Detection")]
    private unsafe static extern void FindMarkers2(ref Marker2[] outMarks, ref Color32[] rawImage, int width, int height, int maxOutMarkerCount, ref int outMarkerDetected);

    WebCamTexture webcam;                   // Webcam object to see what the camera sees
    Texture2D output;                       // Texture of the plane to display camera image
    Color32[] data;                         // Container to hold the pixels of the image
    Marker2[] markDists = new Marker2[4];   // List of marker attributes
    Matrix4x4 matrix = new Matrix4x4();     // Matrix containing transformation to marker

    public Text markerInfoText;             // Display text for distance and Marker ID
    public GameObject cube;                 // Cube object, connected to first marker
    public GameObject sphere;               // Sphere object, connected to second marker
    public GameObject cylinder;             // Cylinder object, connected to third marker
    public GameObject capsule;              // Capsule object, connected to fourth marker

    void Start()
    {
        // Setup and begin the webcam device
        WebCamDevice[] devices = WebCamTexture.devices;
        webcam = new WebCamTexture(devices[0].name);
        webcam.Play();

        // Set a variable that represents the canvas image
        output = new Texture2D(webcam.width, webcam.height);
        GetComponent<Renderer>().material.mainTexture = output;
        data = new Color32[webcam.width * webcam.height];

        // Initialize the text
        markerInfoText.text = "No markers found";

    }

    void Update()
    {
        // Get the pixel values of the camera image
        webcam.GetPixels32(data);
        int detectedFaceCount = 0;

        // Run our find marker algorithm
        FindMarkers2(ref markDists, ref data, webcam.width, webcam.height, 4, ref detectedFaceCount);

        // Loop through each marker detected
        for (int i = 0; i < detectedFaceCount; i++)
        {
            // Compute its position on the computer screen
            float real_x_pos = (markDists[i].center_x - 320.0f) / 40.7f;
            float real_y_pos = (markDists[i].center_y - 198.7f) / 40.2f;

            // Find the rotation and position of the marker in quaternions
            matrix.SetColumn(0, new Vector4(markDists[i].rotate_11, markDists[i].rotate_21, markDists[i].rotate_31, 0));
            matrix.SetColumn(1, new Vector4(markDists[i].rotate_12, markDists[i].rotate_22, markDists[i].rotate_32, 0));
            matrix.SetColumn(2, new Vector4(markDists[i].rotate_13, markDists[i].rotate_23, markDists[i].rotate_33, 0));
            matrix.SetColumn(3, new Vector4(markDists[i].translate_x, markDists[i].translate_y, markDists[i].translate_z, 1));
            Quaternion rotation = matrix.rotation;
            Debug.Log(rotation);
            Vector3 eulerAngles = rotation.eulerAngles;
            Vector3 temp_dir = rotation * new Vector3(0, 0, -1f);

            // Compute the scale and real distance given the camera and screen disparity
            float object_scale = 0.8f * (float)(3.189f * Math.Exp(0.03107f * markDists[i].translate_z) + 7.557f * Math.Exp(0.1924f * markDists[i].translate_z));
            float real_distance = 1.8622f * markDists[i].dist + 0.4143f;

            // If it's the first marker, we change the cube properties
            if (i == 0)
            {
                markerInfoText.text = "Marker ID: " + markDists[i].id.ToString() + "\nDistance: " + real_distance.ToString() + "\n\n";
                cube.transform.position = new Vector3(real_x_pos + temp_dir[0], real_y_pos - temp_dir[1], cube.transform.position.z);
                cube.transform.rotation = Quaternion.Euler(eulerAngles[0], -eulerAngles[1], -eulerAngles[2]);
                cube.transform.localScale = new Vector3(object_scale, object_scale, object_scale);

            }

            // If it's the second marker, we change the sphere properties
            else if (i == 1)
            {
                markerInfoText.text += "Marker ID: " + markDists[i].id.ToString() + "\nDistance: " + real_distance.ToString() + "\n\n";
                sphere.transform.position = new Vector3(real_x_pos + temp_dir[0], real_y_pos - temp_dir[1], sphere.transform.position.z);
                sphere.transform.rotation = Quaternion.Euler(eulerAngles[0], -eulerAngles[1], -eulerAngles[2]);
                sphere.transform.localScale = new Vector3(object_scale, object_scale, object_scale);
            }

            // If it's the third marker, we change the cylinder properties
            else if (i == 2)
            {
                markerInfoText.text += "Marker ID: " + markDists[i].id.ToString() + "\nDistance: " + real_distance.ToString() + "\n\n";
                cylinder.transform.position = new Vector3(real_x_pos + temp_dir[0], real_y_pos - temp_dir[1], cylinder.transform.position.z);
                cylinder.transform.rotation = Quaternion.Euler(eulerAngles[0], -eulerAngles[1], -eulerAngles[2]);
                cylinder.transform.localScale = new Vector3(object_scale, object_scale / 2, object_scale);
            }

            // If it's the fourth marker, we change the capsule properties
            else if (i == 3)
            {
                markerInfoText.text += "Marker ID: " + markDists[i].id.ToString() + "\nDistance: " + real_distance.ToString() + "\n\n";
                capsule.transform.position = new Vector3(real_x_pos + temp_dir[0], real_y_pos - temp_dir[1], capsule.transform.position.z);
                capsule.transform.rotation = Quaternion.Euler(eulerAngles[0], -eulerAngles[1], -eulerAngles[2]);
                capsule.transform.localScale = new Vector3(object_scale, object_scale / 2, object_scale);
            }

            markDists[i].dist = 0;
        }

        // If there is no fourth marker, move capsule off screen
        if (detectedFaceCount < 4)
        {
            capsule.transform.position = new Vector3(-10, -10, capsule.transform.position.z);
        }

        // If there is no third marker, move cylinder off screen
        if (detectedFaceCount < 3)
        {
            cylinder.transform.position = new Vector3(-10, -10, cylinder.transform.position.z);
        }

        // If there is no second marker, move sphere off screen
        if (detectedFaceCount < 2)
        {
            sphere.transform.position = new Vector3(-10, -10, sphere.transform.position.z);
        }

        // If there is no marker, move cube off screen, and update text
        if (detectedFaceCount < 1)
        {
            cube.transform.position = new Vector3(-10, -10, cube.transform.position.z);
            markerInfoText.text = "No markers found";
        }


        // Set the canvas to be the output image with marker boundaries drawn
        output.SetPixels32(data);
        output.Apply();


    }
}
