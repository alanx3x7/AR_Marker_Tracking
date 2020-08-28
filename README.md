# AR Marker Tracking

### Introduction

<img align="right" src=data/AR_Marker_Tracking_Final_Video.gif width="500" height="282"/>

<p align="justify">
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
</p>
<br />

____

### Usage/Installation 
<p align="justify">
Please import as a Visual Studio project, and build as .dll for the library.
Note that the library is also within this project for use in the library.

For usage in Unity, add the Marker_Detection.dll, opencv_world430.dll,
opencv_world430d.dll into Assets > Plugins. For importing the DLL into script,
please see the presentation details. OpenCV dlls have to be downloaded from
OpenCV.

The only callable function in the DLL is FindMarkers2. Please see the source
code comments for the parameters and usage.
</p>
<br />

____

### Build Version
Versions: <br/>
Visual Studio 2017 (15.9.20)<br/>
OpenCV 4.3.0<br/>
Unity 2018.4.17f1
<br />

____

### Theory
There are five main steps to this project.

#### 1. Locate Marker in Image
<img align="center" src=data/Theory_Step_1_Process.jpg width="895" height="149"/>

<p align="justify">
First, the image (a) is gray-scaled, then binarized using thresholding (b) with a fixed value (105).
Adaptive thresholds were not used since we know that the binary markers are black and white, so
adaptive thresholds would cause more background objects to be highlighted and seem like markers,
increasing processing time. Once binarized, contours for the image were found using OpenCV's
findContours function. These contours were then passed into OpenCV's approxPolyDP to approximate
into polygons. These polygons were then filtered by size (too small would be noise), and shape (
we know that the markers should be square and hence have four corners). These polygons are
displayed in image (c). 

Once the contours are found, edge refinement is needed to get a more exact position of the marker
rather than having the edges be a polygonal approximation of the contours found. This is done
by taking a stripe of the image perpendicular to each of the edges. Here, six stripes off each
edge is taken, shown in image (d) above. At each stripe, a Sobel filter is applied to find
the areas of highest intensity change. To obtain subpixel accuracy, a parabola is fitted over
the intensity changes to find the location of maximum intensity change. Doing so, we are able to
identify six points on each line that is the refined edge location in the image.
</p>

#### 2. Rectify Marker
<img align="center" src=data/Theory_Step_1_Process.jpg width="1312" height="255"/>

#### 3. Identify Marker ID

#### 4. Estimate Marker Pose

#### 5. Integration with Unity