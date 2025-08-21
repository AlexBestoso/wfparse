# WavewfrontObjImporter
This class was built so that you can import .obj and .mtl data into your applications<br>

The provided program utilizes the importer class to showcase how it works.

compile running <code>make</code>

sample usage: 

To generate javascript classes for use in WebGl, contains material and formatted vertex data, run:
<code>./wfparse ./experiment.obj --buf-gen --format=vnt --javascript</code>

To enumerate object data
<code>./wfparse ./experiment.obj --full-dump</code>

To use the javascript code, you will need to use a class for the materials. Here is the class:

```
class BtWebglMaterial {
    name=null;
    ns=null;
    ka=null;
    kd=null;
    ks=null;
    ke=null;
    ni=null;
    d=null;
    illum=null;
    mapKd=null;
    constructor(name, ns, ka, kd, ks, ke, ni, d, illum, mapKd){
	this.name=name;
	this.ns = ns;
	this.ka = ka;
	this.kd = kd;
	this.ks = ks;
	this.ke = ke;
	this.ni = ni;
	this.d = d;
	this.illum = illum;
	this.mapKd = mapKd;
    }
}
```
