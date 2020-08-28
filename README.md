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
</p>

<p align="justify">
AR markers as essentially visual cues that trigger the display of virtual information, and provides
6DOF to the AR system (3DOF for position, 3DOF for orientation). In order to use these AR markers,
the AR system must be able to identify these 6DOF of the marker by locating it in the scene. Hence
the need for AR marker tracking.
</p>

<p align="justify">
Therefore, this project is just a simple venture into AR marker tracking by attempting to track
ArUco markers in real time, through the creation of a Unity Application.
</p>

<p align="justify">
Please check the powerpoint for details, references, and explanation!
The powerpoint URL is [here](https://drive.google.com/open?id=1_aS8RuEpxBSvm6QSTLZmo-pQbEXdsq4R)
</p>

____

### Usage/Installation 
<p align="justify">
For direct usage, please go to Release and see Version 1.0. Download 
AR_Marker_Tracking_Release_V1.0.zip, unzip, and
then double click on Marker_Detection_Try_2.exe
</p>

<p align="justify">
If you want to use the marker tracking C++ code, please import as a Visual 
Studio project, and build as .dll for the library. Note that the library 
is also within this project for use in the library. For usage in Unity, 
add the Marker_Detection.dll, opencv_world430.dll, opencv_world430d.dll into 
Assets > Plugins. For importing the DLL into script, please see the presentation 
details. OpenCV dlls have to be downloaded from OpenCV.

The only callable function in the DLL is FindMarkers2. Please see the source
code comments for the parameters and usage.
</p>


____

### Build Version
Versions: <br/>
Visual Studio 2017 (15.9.20)<br/>
OpenCV 4.3.0<br/>
Unity 2018.4.17f1


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
into polygons. These polygons were then filtered by size (too small would be noise), and shape (we 
know that the markers should be square and hence have four corners). These polygons are
displayed in image (c). 
</p>

<p align="justify">
Once the contours are found, edge refinement is needed to get a more exact position of the marker
rather than having the edges be a polygonal approximation of the contours found. This is done
by taking a stripe of the image perpendicular to each of the edges. Here, six stripes off each
edge is taken, shown in image (d) above. At each stripe, a Sobel filter is applied to find
the areas of highest intensity change. To obtain subpixel accuracy, a parabola is fitted over
the intensity changes to find the location of maximum intensity change. Doing so, we are able to
identify six points on each line that is the refined edge location in the image (e).
</p>

#### 2. Rectify Marker
<img align="center" src=data/Theory_Step_2_Process.jpg width="900" height="165"/>

<p align="justify">
Once the six refined edge positions on each edge is found, an edge is fitted to those six points
to yield the refined edge positions (f). Once these refined edges are found, the refined corner
positions can be found by taking the cross product of the refined edges to find the intersection
(g). With the refined corner locations, a homography can be computed (as a 2D homography requires
four points, and there are four corners per marker), to bring the marker to be planar (h).
This is done via OpenCV's getPerspectiveTransform to a fixed set of coordinates to find the
transformation, then warpPerspective to perform the actual transformation. A final thresholding
is then performed to separate the white and black squares.
</p>

#### 3. Identify Marker ID
<img align="center" src=data/Theory_Step_3_Process.png width="518" height="220"/>

<p align="justify">
With the marker becoming a planar image and thresholded, the marker ID can then be deduced by
treating each row as a binary number, with white squares denoting "0" and black squares denoting 
"1". Since there is no inherent orientation in the marker, there are technically four different
marker IDs depending on the orientation (in the marker above, it could be 0512, 6140, 48A0, or
0286). We use the smallest ID as the marker ID (by convention) - which means the "correct"
orientation is the marker rotated by 90 degrees clockwise with marker ID 0286.
</p>

#### 4. Estimate Marker Pose
<img align="center" src=data/Theory_Step_4_Process.png width="389" height="218"/>

<p align="justify">
Once we have the position of the four corners, we can compute the position and orientation
of the marker in the image. Though homographies are only up to scale, because we know 
beforehand the size of the marker in question, we can use this to find the distance of the
marker from the camera. Doing so, we are able to determine the 6DOF (position, orientation)
of the marker in the image.
</p>

#### 5. Integration with Unity

<p align="justify">
The C++ code is compiled as a dynamic linked library (dll). This was then integrated with 
Unity. The Unity application is able to grab the image of what the default camera of the 
system sees, and then passes the image to the C++ code for marker detection. The .dll that
performs this detection passes back the image with the borders of the markers outlined in 
green, along with a stream of Marker objects, one for each detected marker (capped at four
markers per image), containing the marker ID, marker position, orientation, and marker
image corner coordinates. The Unity application displays the output image real time, along
with the marker ID and distance on the left side of the screen. 
</p>

<p align="justify">
The Unity Application also places virtual shapes (green cube, blue sphere, red cylinder,
yellow pill shape) over the first four markers detected. Some processing is done such that 
these virtual objects rotate as the marker rotates, and scales up and down as the markers
move closer or further from the camera.
</p>