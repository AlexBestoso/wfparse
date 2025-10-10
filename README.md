# wfparse

This program parses Wavefront .obj and .mtl files allowing you to easily associate vertecies, texture coords, and normalization coords to faces.

wfparse means WaveFront Parse.

compile running <code>make</code>

run the following command to view the manual
<code>./wfparse help</code>

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
