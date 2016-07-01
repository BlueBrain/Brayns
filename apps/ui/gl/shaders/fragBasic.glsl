
varying vec3 vNormal;

void main()
{
	//gl_FragColor = vec4(0.4,0.4,0.8,1.0);
	gl_FragData[0] = vec4(vNormal.x,vNormal.y,vNormal.z,1.0);
	gl_FragData[1] = vec4(1.0,0.0,0.0,1.0);
	gl_FragData[2] = vec4(0.0,1.0,0.0,1.0);
	gl_FragData[3] = vec4(0.0,0.0,1.0,1.0);
	gl_FragData[4] = vec4(1.0,1.0,0.0,1.0);
	gl_FragData[5] = vec4(0.0,1.0,1.0,1.0);
	gl_FragData[6] = vec4(1.0,0.0,1.0,1.0);
	gl_FragData[7] = vec4(0.5,1.0,0.5,1.0);

	//gl_FragColor = vec4(vNormal.x,vNormal.y,vNormal.z,1.0);
}
