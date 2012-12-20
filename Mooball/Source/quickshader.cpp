#include "quickshader.h"
#include "quickrender.h"


CShaderObject::CShaderObject()
{
    m_shaderObject = 0;
    m_bIsBound = false;
}

CShaderObject::~CShaderObject()
{
    if(m_shaderObject)
		glDeleteProgram(m_shaderObject);

    m_shaderObject = 0;
    m_bIsBound = false;
}

bool CShaderObject::Load(const char* vertFileName, const char* fragFileName)
{
    // Create program object //
	m_shaderObject = glCreateProgram();

    FILE* vFile, *pFile;
	char* vString, *pString;
	long fSize;
	GLint objLogLen;
	GLsizei len;

	if(vertFileName)
		vFile = fopen(vertFileName, "rb");
	if(vFile)
	{
		fseek(vFile, 0, SEEK_END);
		fSize = ftell(vFile);
		fseek(vFile, 0, SEEK_SET);
		vString = new char[fSize + 1];
		fread(vString, 1, fSize, vFile);
		vString[fSize] = '\0';

		// Create the vertex shader object and compile //
		GLenum vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, (const GLcharARB**)&vString, NULL);
		glCompileShader(vs);

		// if the compile succeeded add it to the program object //
		GLint res;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &res);
		if(res)
			glAttachShader(m_shaderObject, vs);
			//glAttachObjectARB(m_shaderObject, vs);

		// Get error string if compile was unsuccessful //
		else
		{
			glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &objLogLen);
			char* errLog = new char[objLogLen];

			glGetShaderInfoLog(vs, objLogLen, &len, errLog);
			fprintf(stderr, "Could not compile %s: %s\n", vertFileName, errLog);

			delete[] errLog;
			return false;
		}

		// once added we can kill it //
		glDeleteShader(vs);
		delete[] vString;
	}

	if(fragFileName)
		pFile = fopen(fragFileName, "rb");
	if(pFile)
	{
		fseek(pFile, 0, SEEK_END);
		fSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		pString = new char[fSize + 1];
		fread(pString, 1, fSize, pFile);
		pString[fSize] = '\0';

		GLenum ps = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(ps, 1, (const GLchar**)&pString, NULL);
		glCompileShader(ps);

		GLint res;
		glGetShaderiv(ps, GL_COMPILE_STATUS, &res);
		if(res)
			glAttachShader(m_shaderObject, ps);

		else
		{
			glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &objLogLen);
			char* errLog1 = new char[objLogLen];

			glGetShaderInfoLog(ps, objLogLen, &len, errLog1);
			fprintf(stderr, "Could not compile %s: %s\n", fragFileName, errLog1);

			delete[] errLog1;
			return false;
		}

		glDeleteShader(ps);
		delete[] pString;

		fclose(pFile);
	}

    // Link the programs into the shader object and verify link status //
    // Fail on unsuccessful link //
	glLinkProgram(m_shaderObject);
	glGetProgramiv(m_shaderObject, GL_INFO_LOG_LENGTH, &objLogLen);
	char* linkErrLog = new char[objLogLen];
	glGetProgramInfoLog(m_shaderObject, objLogLen, &len, linkErrLog);

	if(len > 0)
		fprintf(stderr, "Shader %s failed to link: %s\n", vertFileName, linkErrLog);

	HashSamplers();
	HashAttributes();

	return true;
}

void CShaderObject::HashSamplers()
{
	glUseProgram(m_shaderObject);

	int nUniforms = 0;
	int nSamplers = 0;
	glGetProgramiv(m_shaderObject, GL_ACTIVE_UNIFORMS, &nUniforms);

	GLint* size   = (GLint*)malloc(nUniforms * sizeof(GLint));
    GLenum* type   = (GLenum*)malloc(nUniforms * sizeof(GLenum));
    GLsizei* length = (GLsizei*)malloc(nUniforms * sizeof(GLsizei));
    GLchar** name   = (GLchar**)malloc(nUniforms * sizeof(GLcharARB**));

    for (int i = 0; i < nUniforms; i++)
    {
        name[i] = (GLcharARB*)malloc(MAX_UNIFORM_LEN * sizeof(GLcharARB));
		glGetActiveUniform(m_shaderObject, i, MAX_UNIFORM_LEN, &length[i], &size[i], &type[i], name[i]);

        if(type[i] >= GL_SAMPLER_1D_ARB && type[i] <= GL_SAMPLER_2D_RECT_SHADOW_ARB)
        {
        	m_samplerHash.Add(name[i], nSamplers);
			glUniform1i(glGetUniformLocation(m_shaderObject, name[i]), nSamplers);
        	++nSamplers;
        }
    }

	free(size);
	free(type);
	free(length);
	free(name);
 
	glUseProgram(0);
}

void CShaderObject::HashAttributes()
{
	glUseProgram(m_shaderObject);

	int nAttributes = 0;
	glGetProgramiv(m_shaderObject, GL_ACTIVE_ATTRIBUTES, &nAttributes);

	GLint* size = (GLint*)malloc(nAttributes * sizeof(GLint));
	GLenum* type = (GLenum*)malloc(nAttributes * sizeof(GLenum));
	GLsizei* len = (GLsizei*)malloc(nAttributes * sizeof(GLsizei));
	GLchar** name = (GLchar**)malloc(nAttributes * sizeof(GLchar**));

	for(int i = 0; i < nAttributes; ++i)
	{
		name[i] = (GLchar*)malloc(MAX_UNIFORM_LEN * sizeof(GLchar));
		glGetActiveAttrib(m_shaderObject, i, MAX_UNIFORM_LEN, &len[i], &size[i], &type[i], name[i]);
		m_attributeHash.Add(name[i], glGetAttribLocation(m_shaderObject, name[i]));
	}

	free(size);
	free(type);
	free(len);
	free(name);

	glUseProgram(0);
}

bool CShaderObject::Bind()
{
	// Check if linked then set as current //
	GLint res;
	glGetProgramiv(m_shaderObject, GL_LINK_STATUS, &res);
	if(res)
	{
		glUseProgram(m_shaderObject);
        m_bIsBound = true;
		return true;
	}

	return false;
}


void CShaderObject::Unbind()
{
	glUseProgram(0);
    m_bIsBound = false;
}


bool CShaderObject::BindTexture(const char* paramName, int texHandle)
{
    // Check bind status //
	if(!m_bIsBound)
		return false;

	int residence = m_samplerHash[paramName];
	if(residence < 0)
		return false;
	glActiveTexture(GL_TEXTURE0_ARB + residence);
	glBindTexture(GL_TEXTURE_2D, g_pTexInterface->GetGLHandle(texHandle));

	return true;
}


void CShaderObject::EvictTextures()
{
	g_pTexInterface->EvictTextures();
}



void CShaderObject::BindUniformFloat(const char* paramName, const float* f, int nFloats)
{
    // Validate bind status //
	if(!m_bIsBound)
		return;

    // Locate bind point within shader and validate it //
	int bind_point = glGetUniformLocation(m_shaderObject, paramName);
	if(bind_point < 0)
		return;

	else
	{
		switch(nFloats)
		{
			case 1:
				glUniform1f(bind_point, *f);
				break;

			case 2:
				glUniform2f(bind_point, f[0], f[1]);
				break;

			case 3:
				glUniform3f(bind_point, f[0], f[1], f[2]);
				break;

			case 4:
				glUniform4f(bind_point, f[0], f[1], f[2], f[3]);
				break;
		}
	}
}

void CShaderObject::BindUniformInt(const char* paramName, const int* i, int nInts)
{
    // Validate bind status //
	if(!m_bIsBound)
		return;

    // Locate bind point within shader and validate it //
	int bind_point = glGetUniformLocation(m_shaderObject, paramName);
	if(bind_point < 0)
		return;

	else
	{
		switch(nInts)
		{
			case 1:
				glUniform1i(bind_point, *i);
				break;

			case 2:
				glUniform2i(bind_point, i[0], i[1]);
				break;

			case 3:
				glUniform3i(bind_point, i[0], i[1], i[2]);
				break;

			case 4:
				glUniform4i(bind_point, i[0], i[1], i[2], i[3]);
				break;
		}
	}
}


void CShaderObject::BindUniformVec2Array(const char* paramName, const vec2* v, int nVecs)
{
    // Validate bind status //
	if(!m_bIsBound)
		return;

    // Obtain and validate bind point //
	int bind_point = glGetUniformLocation(m_shaderObject, paramName);
	if(bind_point < 0)
		return;

	glUniform2fv(bind_point, nVecs, (float*)v);
}

void CShaderObject::BindUniformVec4Array(const char* paramName, const vec4* v, int nVecs)
{
    // Validate bind status //
	if(!m_bIsBound)
		return;

    // Obtain and validate bind point //
	int bind_point = glGetUniformLocation(m_shaderObject, paramName);
	if(bind_point < 0)
		return;

	glUniform4fv(bind_point, nVecs, (float*)v);
}

void CShaderObject::BindStateMatrix(const char* paramName, unsigned int matrix)
{
    // Check and validate bind status //
	if(!m_bIsBound)
		return;

    // Get and validate bind point //
	int bind_point = glGetUniformLocation(m_shaderObject, paramName);
	if(bind_point < 0)
		return;

	mat4 MV, P, F;
	mat4_load_identity(MV);
	mat4_load_identity(P);

	if(matrix & MATRIX_MODELVIEW)
		glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	if(matrix & MATRIX_PROJECTION)
		glGetFloatv(GL_PROJECTION_MATRIX, P);

	mat4_mul_mat4(P, MV, F);

	if(matrix & MATRIX_TRANSPOSE)
		mat4_transpose(F);

	glUniformMatrix4fv(bind_point, 1, false, F);
}

GLint CShaderObject::GetAttribLoc(const char* attribName)
{
	return m_attributeHash[attribName];
}





CShaderInterface::CShaderInterface()
{
	m_vShaderResources = new std::vector <CShaderObject*>;

	m_vShaderResources->reserve( MAX_SHADERS );

	m_curShader = INVALID_HANDLE;
	m_pCurShader = NULL;
	m_nShaders = 0;

	m_bIsShaderBound = false;
}


CShaderInterface::~CShaderInterface()
{
	if( m_vShaderResources )
	{
		int nShaders = m_vShaderResources->size();
		for( unsigned i = 0; i < nShaders; ++i )
		{
			if( m_vShaderResources->at( i ) )
				delete m_vShaderResources->at( i );
		}

		m_vShaderResources->clear();
		delete m_vShaderResources;
	}

	m_nShaders = 0;
	m_pCurShader = NULL;
	m_curShader = INVALID_HANDLE;

	m_bIsShaderBound = false;
}

int CShaderInterface::AddShader( const std::string& vertProg, const std::string& pixelProg )
{
	if( vertProg.length() <= 0 || pixelProg.length() <= 0 )
		return INVALID_HANDLE;

	int slot = FindEmptySlot( *m_vShaderResources );
	CShaderObject* fx = new CShaderObject;
	if( !fx->Load( vertProg.c_str(), pixelProg.c_str() ) )
	{
		delete fx;
		return INVALID_HANDLE;
	}

	if( slot != INVALID_HANDLE )
	{
		m_vShaderResources->at( slot ) = fx;
		++m_nShaders;
		return slot;
	}

	else
	{
		m_vShaderResources->push_back( fx );
		++m_nShaders;
		return m_vShaderResources->size() - 1;
	}
}


void CShaderInterface::DeleteShader( int handle )
{
	int nShaders = m_vShaderResources->size();
	if( handle == INVALID_HANDLE || handle >= nShaders )
		return;

	if( m_vShaderResources->at( handle ) != NULL )
	{
		delete m_vShaderResources->at( handle );
		m_vShaderResources->at( handle ) = NULL;

		--m_nShaders;
	}
}


bool CShaderInterface::BindShader( int handle )
{
	if( handle == INVALID_HANDLE || handle >= m_nShaders )
		return false;

	m_pCurShader = m_vShaderResources->at( handle );
	m_curShader = handle;
	m_bIsShaderBound = true;

	return m_vShaderResources->at( handle )->Bind();
}

void CShaderInterface::UnbindShader()
{
	if( !m_pCurShader || m_curShader == INVALID_HANDLE )
		return;

	m_bIsShaderBound = false;

	m_pCurShader->Unbind();
	m_pCurShader->EvictTextures();

	m_pCurShader = NULL;
	m_curShader = INVALID_HANDLE;
}

bool CShaderInterface::SetStateMatrix( const std::string& name, unsigned int mat )
{
	if( m_pCurShader )
	{
		m_pCurShader->BindStateMatrix( name.c_str(), mat );
		return true;
	}

	return false;
}

bool CShaderInterface::SetFloat( const std::string& name, float* f, int nFloats )
{
	if( m_pCurShader )
	{
		m_pCurShader->BindUniformFloat( name.c_str(), f, nFloats );
		return true;
	}

	return false;
}

bool CShaderInterface::SetInt( const std::string& name, int* i, int nInts )
{
	if( m_pCurShader )
	{
		m_pCurShader->BindUniformInt( name.c_str(), i, nInts );
		return true;
	}

	return false;
}

bool CShaderInterface::SetTexture( const std::string& name, int texHandle )
{
	if( m_pCurShader )
	{
		m_pCurShader->BindTexture( name.c_str(), texHandle );
		return true;
	}

	return false;
}


int CShaderInterface::GetCurrentShaderIndex()
{
	if( IS_HANDLE_VALID( m_curShader ) )
		return m_curShader;

	return INVALID_HANDLE;
}

GLint CShaderInterface::GetAttribLoc(const char* attribName)
{
	return m_pCurShader->GetAttribLoc(attribName);
}


bool CShaderInterface::IsShaderBound()
{
	return m_bIsShaderBound;
}
