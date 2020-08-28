Please check the powerpoint for details, references, and explanation!
The powerpoint URL is [here](https://drive.google.com/open?id=1_aS8RuEpxBSvm6QSTLZmo-pQbEXdsq4R)

Please import as a Visual Studio project, and build as .dll for the library.
Note that the library is also within this project for use in the library.

For usage in Unity, add the Marker_Detection.dll, opencv_world430.dll,
opencv_world430d.dll into Assets > Plugins. For importing the DLL into script,
please see the presentation details. OpenCV dlls have to be downloaded from
OpenCV

The only callable function in the DLL is FindMarkers2. Please see the source
code comments for the parameters and usage.

Versions:
Visual Studio 2017 (15.9.20)
OpenCV 4.3.0
Unity 2018.4.17f1