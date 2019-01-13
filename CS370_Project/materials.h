typedef struct MaterialType {
	GLfloat ambient[4];
	GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
} MaterialType;

//material types
MaterialType cyan_plastic = {{0.0f,0.1f,0.06f,0.1f},
    {0.0f,0.50980392f,0.50980392f,1.0f},
    {0.50196078f,0.50196078f,0.50196078f,1.0f},
    {25.0f}};
						
void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}

