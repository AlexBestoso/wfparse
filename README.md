# WavewfrontObjImporter
This class was built so that you can import .obj files into openGL on linux.<br>
As of writing the code only supports very simple objects and processes only vertecies, normals, and textures.<br><br>
Over time more features will be added as I develop more things in openGL.<br><br>
All you need to do to use the code is include it in your C++ program.<br><br>
If you like what this program does and want to use it, please do so. If you make some changes to it that make it better, please share :D
<br><br>
<p>
Of the included .obj files, the cube and monkey properly render. Objects that consist of faces with vertex counts of (count % 3 = 1) and/or (count % 3 = 0) will properly render. If your object has a face that produces (count % 3 = 2); the object will be imporperly rendered. 
</p>
