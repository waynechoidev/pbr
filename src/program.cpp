#include "program.h"

Program::Program()
{
	_programID = 0;
	clear();
}

void Program::createFromFiles(std::string vertexLocation, std::string fragmentLocation)
{
	std::string vertexCode = readFile(vertexLocation);
	std::string fragmentCode = readFile(fragmentLocation);

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
		std::cout << "Error creating shader program!\n";
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
	glBufferData(GL_UNIFORM_BUFFER, 196, nullptr, GL_DYNAMIC_DRAW);
	GLuint matricesBlockIndex = glGetUniformBlockIndex(_programID, "Matrices");
	glUniformBlockBinding(_programID, matricesBlockIndex, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboMatrices);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::genFragmentBuffers()
{
	glGenBuffers(1, &_uboFragment);
	glBindBuffer(GL_UNIFORM_BUFFER, _uboFragment);
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_DYNAMIC_DRAW);
	GLuint fragmentBlockIndex = glGetUniformBlockIndex(_programID, "Fragment");
	glUniformBlockBinding(_programID, fragmentBlockIndex, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uboFragment);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::use()
{
	glUseProgram(_programID);
}

void Program::bindVertexBuffers(glm::mat4 model, glm::mat4 projection, glm::mat4 view, float &heightScale)
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboMatrices);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(model));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(projection));

	glBufferSubData(GL_UNIFORM_BUFFER, 192, 4, &heightScale);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::bindFragmentBuffers(glm::vec3 campos, glm::vec3 lightPos, bool useDirectLight, bool useEnvLight)
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboFragment);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, glm::value_ptr(campos));
	int useDirectLightInt = useDirectLight ? 1 : 0;
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &useDirectLightInt);

	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, glm::value_ptr(lightPos));
	int useEnvLightInt = useEnvLight ? 1 : 0;
	glBufferSubData(GL_UNIFORM_BUFFER, 28, 4, &useEnvLightInt);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Program::clear()
{
	if (_programID != 0)
	{
		glDeleteProgram(_programID);
		_programID = 0;
	}

	_uboMatrices = _uboFragment = _uboMaterial = _uboLight = 0;
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
