# AR Marker Tracking

### Introduction

<img align="right" src=data/AR_Marker_Tracking_Final_Video.gif width="400" height="226"/>

This project was done for EN.601.654 Augmented Reality Course at Johns Hopkins University.
With augmented reality systems, the goal is to place virtual objects in the view of the user.
But without additional information about the scene in question, it is difficult for AR systems
to know where to place virtual objects such that they make sense to the user and are in context.
Hence AR markers are placed in the real world to indicate to the AR system where the virtual 
objects should be placed.

AR markers as essentially visual cues that trigger the display of virtual information, and provides
6DOF to the AR system (3DOF for position, 3DOF for orientation). In order to use these AR markers,
the AR system must be able to identify these 6DOF of the marker by locating it in the scene. Hence
the need for AR marker tracking.

Therefore, this project is just a simple venture into AR marker tracking by attempting to track
ArUco markers in real time, through the creation of a Unity Application.

Please check the powerpoint for details, references, and explanation!
The powerpoint URL is [here](https://drive.google.com/open?id=1_aS8RuEpxBSvm6QSTLZmo-pQbEXdsq4R)

### Usage/Installation 
Please import as a Visual Studio project, and build as .dll for the library.
Note that the library is also within this project for use in the library.

For usage in Unity, add the Marker_Detection.dll, opencv_world430.dll,
opencv_world430d.dll into Assets > Plugins. For importing the DLL into script,
please see the presentation details. OpenCV dlls have to be downloaded from
OpenCV.

The only callable function in the DLL is FindMarkers2. Please see the source
code comments for the parameters and usage.

### Build Version
Versions:
Visual Studio 2017 (15.9.20)
OpenCV 4.3.0
Unity 2018.4.17f1