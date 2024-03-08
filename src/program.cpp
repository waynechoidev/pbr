#include "program.h"

Program::Program()
{
	_programID = 0;
	clear();
}

void Program::createFromString(std::string vertexCode, std::string fragmentCode)
{
	compileShader(vertexCode, fragmentCode);
}

void Program::createFromFiles(std::string vertexLocation, std::string fragmentLocation)
{
	std::string vertexString = readFile(vertexLocation);
	std::string fragmentString = readFile(fragmentLocation);
	const char *vertexCode = vertexString.c_str();
	const char *fragmentCode = fragmentString.c_str();

	compileShader(vertexCode, fragmentCode);
}

std::string Program::readFile(std::string fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation.c_str(), std::ios::in);

	if (!fileStream.is_open())
	{
		std::cout << "Failed to read " << fileLocation << "! File doesn't exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Program::compileShader(std::string vertexCode, std::string fragmentCode)
{
	_programID = glCreateProgram();

	if (!_programID)
	{
		std::cout << "Error creating shader program!" << std::endl;
		return;
	}

	addShader(_programID, vertexCode, GL_VERTEX_SHADER);
	addShader(_programID, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = {0};

	glLinkProgram(_programID);
	glGetProgramiv(_programID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_programID, sizeof(eLog), NULL, eLog);
		std::cout << "Error linking program: " << eLog << std::endl;
		return;
	}

	glValidateProgram(_programID);
	glGetProgramiv(_programID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_programID, sizeof(eLog), NULL, eLog);
		std::cout << "Error validating program: " << eLog << std::endl;
		return;
	}
}

void Program::genVertexBuffers()
{
	glGenBuffers(1, &_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 192, nullptr, GL_STATIC_DRAW);
	GLuint matricesBlockIndex = glGetUniformBlockIndex(_programID, "Matrices");
	glUniformBlockBinding(_programID, matricesBlockIndex, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboMatrices);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::genFragmentBuffers()
{
	glGenBuffers(1, &_uboFragment);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboFragment);
	glBufferData(GL_UNIFORM_BUFFER, 16, nullptr, GL_DYNAMIC_DRAW);
	GLuint fragmentBlockIndex = glGetUniformBlockIndex(_programID, "Fragment");
	glUniformBlockBinding(_programID, fragmentBlockIndex, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uboFragment);

	glGenBuffers(1, &_uboMaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboMaterial);
	glBufferData(GL_UNIFORM_BUFFER, 16, nullptr, GL_DYNAMIC_DRAW);
	GLuint materialBlockIndex = glGetUniformBlockIndex(_programID, "Material");
	glUniformBlockBinding(_programID, materialBlockIndex, 2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, _uboMaterial);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::use()
{
	glUseProgram(_programID);
}

void Program::bindVertexBuffers(glm::mat4 model, glm::mat4 projection, glm::mat4 view)
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(model));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(projection));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::bindFragmentBuffers(bool useTexture, glm::vec3 viewPosition, const Material &material)
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboFragment);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, glm::value_ptr(viewPosition));
	int useTextureInt = useTexture ? 1 : 0;
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &useTextureInt);

	glBindBuffer(GL_UNIFORM_BUFFER, _uboMaterial);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &material.ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &material.shininess);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &material.diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &material.specular);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::clear()
{
	if (_programID != 0)
	{
		glDeleteProgram(_programID);
		_programID = 0;
	}

	_uboMatrices = _uboFragment = _uboMaterial = 0;
}

void Program::addShader(GLuint theProgram, std::string shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar *theCode[1];
	theCode[0] = shaderCode.c_str();

	GLint codeLength[1];
	codeLength[0] = (GLint)strlen(shaderCode.c_str());

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = {0};

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		std::cout << "Error compiling the " << shaderType << " shader: " << eLog << std::endl;
		return;
	}

	glAttachShader(theProgram, theShader);
}

Program::~Program()
{
	clear();
}
