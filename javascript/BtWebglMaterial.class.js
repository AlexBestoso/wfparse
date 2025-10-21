class BtWebglMaterial{
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
