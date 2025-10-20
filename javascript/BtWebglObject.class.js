class BtWebglObject {
	textures=null;
	name=null;
	material=null;
	stride=null;
	data=null;
	gl=null;
	vao=null;
	vbo=null;
	ebo=null;
	
	constructor(n, s, m, d){
		this.name = n;
		this.stride = s;
		this.material = m;
		this.data = d;
	}
	
	generateBufferObjects(gl){
		this.gl = gl;
		this.vao = this.gl.createVertexArray();
                this.vbo = this.gl.createBuffer();
                this.ebo = this.gl.createBuffer();
	}

	generateCubemap(){
		console.log('Binding cubemap.');
		obj.texture = this.gl.createTexture();
		this.gl.bindTexture(this.gl.TEXTURE_CUBE_MAP, obj.texture);

		for(var n=0; n<obj.image.length; n++){
			var img = obj.image[n];
			this.gl.texImage2D(
				this.gl.TEXTURE_CUBE_MAP_POSITIVE_X + n,
				0,
				this.gl.RGBA,
				img.width,
				img.height,
				0,
				this.gl.RGBA,
				this.gl.UNSIGNED_BYTE,
				img
			);
		}

		this.gl.texParameteri(this.gl.TEXTURE_CUBE_MAP, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(this.gl.TEXTURE_CUBE_MAP, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(this.gl.TEXTURE_CUBE_MAP, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
		this.gl.texParameteri(this.gl.TEXTURE_CUBE_MAP, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);
		this.gl.texParameteri(this.gl.TEXTURE_CUBE_MAP, this.gl.TEXTURE_WRAP_R, this.gl.CLAMP_TO_EDGE);
	}

	generateTexture(){
		console.log('Binding textures.');
		obj.texture = this.gl.createTexture();
		this.gl.bindTexture(this.gl.TEXTURE_2D, obj.texture);

		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.MIRRORED_REPEAT);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.MIRRORED_REPEAT);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR_MIPMAP_LINEAR);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);

		this.gl.texImage2D(
			this.gl.TEXTURE_2D,
			0,
			this.gl.RGBA,
			obj.image.width,
			obj.image.height,
			0,
			this.gl.RGBA,
			this.gl.UNSIGNED_BYTE,
			obj.image
		);
		this.gl.generateMipmap(this.gl.TEXTURE_2D);
	}

	bindVao(){
		this.gl.bindVertexArray(this.vao);
	}

	bindVbo(){
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.vbo);
	}
	
	bind(){
		this.bindVao();
		this.bindVbo();
	}
}
