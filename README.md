# wfparse

<h1>This Version Still Needs Testing, Compression looks like it works; but decompression needs to be checked.</h1>

This program parses Wavefront .obj and .mtl files allowing you to easily associate vertecies, texture coords, and normalization coords to faces.

wfparse means WaveFront Parse.

compile running <code>make && make install</code>

To prevent shady people from injecting shared objects, the compiled .so files need to be owned by root. Hence the install running chmod. (call me paranoid)

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
