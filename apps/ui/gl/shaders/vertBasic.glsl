varying vec3 vNormal;

void main()
{
    vNormal=gl_Normal;
	gl_Position = ftransform();
}
