# WavewfrontObjImporter
This class was built so that you can import .obj and .mtl data into your applications<br>

The provided program utilizes the importer class to showcase how it works.

compile running <code>make</code>

sample usage: 

To generate useable glut buffers
<code>./wfparse ./experiment.obj --buf-gen --format=vnt --javascript</code>

To enumerate object data
<code>./wfparse ./experiment.obj --full-dump</code>
