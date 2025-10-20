class BtWebglCollection{
	name=null;
	objects=Array();
	objectCount=0;

	constructor(collectionList, collectionName){
		this.objects = collectionList;
		this.objectCount = collectionList.length;
		this.name = collectionName;
	}

	initObjectBufferObjects(gl){
		for(var i=0; i<this.objectCount; i++){
			this.objects[i].generateBufferObjects(gl);
		}
	}

	setObjectList(collectionList){
		this.objects = collectionList;
	}
	
	pushObjectCollection(object){
		this.objects.push(object);
		this.objectCount = this.objects.length;
	}
	
	setCollectionName(name){
		this.name = name;
	}
}
