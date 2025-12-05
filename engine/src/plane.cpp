#include "plane.h"

void Plane::initPlane(glm::vec2 pos, glm::vec2 size) {
    m_positionsVbo = 0;
    m_vao = 0;
    m_position = glm::vec3(0.0f);
    m_scale = glm::vec3(1.0f);

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    color[3] = 1.0f;

    m_position.x = pos.x;
    m_position.y = pos.y;
    m_position.z = 0.0f;

    float vertices [] = {
        // 1. trig coords
        m_position.x, m_position.y, m_position.z, 			//lower left
        m_position.x + size.x, m_position.y , m_position.z, 			//lower right
        m_position.x + size.x, m_position.y + size.y, m_position.z, //upper right

        // 2. trig coords
        m_position.x, m_position.y, m_position.z, //lower left
        m_position.x + size.x, m_position.y + size.y, m_position.z, //upper right
        m_position.x, m_position.y + size.y, m_position.z, //upper left
    };

    float texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
     };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_positionsVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionsVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_texCoordsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Plane::Plane(float xPos, float yPos, float width, float height) {
    initPlane(glm::vec2{xPos, yPos}, glm::vec2{width, height});
 }

 Plane::Plane(glm::vec2 pos, glm::vec2 size) {
    initPlane(pos, size);
 }

 Plane::~Plane()
{
	// Delete Vertex Buffer Objects
	glDeleteBuffers(1, &m_positionsVbo);
	glDeleteBuffers(1, &m_texCoordsVbo);
	// Delete Vertex Array Object
	glDeleteVertexArrays(1, &m_vao);
}

void Plane::render(Shader* shader, Camera* camera, GLuint textureId)
{
	// Bind the shader program
	shader->bind();					// Aktivoi k�ytett�v� shader
	// Asetetaan v�ri shaderille
	shader->setUniform4f("u_color", color[0], color[1], color[2], color[3]);
	// Set the MVP Matrix -uniform
	shader->setUniformMat4("u_mvpMatrix", camera->getProjectionMatrix() * camera->getViewMatrix() * getModelMatrix());
    //shader->setUniform3f("a_position", 0.1f, 0.1f, 0.1f);
    shader->setUniform1i("usingSampler", 0);
	// Set the texture uniform, if texture exists
	if (textureId > 0) {
		shader->setUniform1i("usingSampler", 1);
		glActiveTexture(GL_TEXTURE0);
	 	glBindTexture(GL_TEXTURE_2D, textureId);
        shader->setUniform1i("uTexture", 0);
	}

	glBindVertexArray(m_vao);		// Aktivoi VAO, joka m��ritt�� piirtotiedot
	
	// Draw 6 vertices as triangles
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Plane::setScale(glm::vec2 scale)
{
	m_scale.x = scale.x;
	m_scale.y = scale.y;
    m_scale.z = 0.0f;
}

void Plane::setScale(float newScale)
{
	m_scale.x = newScale;
	m_scale.y = newScale;
    m_scale.z = 0.0f;
}

void Plane::setScale(float newX, float newY)
{
	m_scale.x = newX;
	m_scale.y = newY;
    m_scale.z = 0.0f;
}

void Plane::setPosition(const glm::vec2 pos)
{
	m_position.x = pos.x;
	m_position.y = pos.y;
    m_position.z = 0;
}

void Plane::setPosition(float x, float y)
{
	m_position.x = x;
    m_position.y = y;
    m_position.z = 0;
}

void Plane::setColor(float r, float g, float b, float a)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}
void Plane::setColor(glm::vec4 newColor)
{
	color[0] = newColor.r;
	color[1] = newColor.g;
	color[2] = newColor.b;
	color[3] = newColor.a;
}
void Plane::setColor(glm::vec3 newColor)
{
	color[0] = newColor.r;
	color[1] = newColor.g;
	color[2] = newColor.b;
	color[3] = 1.0f;
}

glm::mat4 Plane::getModelMatrix() const
{
	glm::mat4 translation = glm::translate(m_position); // this fixes the mirroring issue
	glm::mat4 scaling = glm::scale(m_scale);
    return translation * scaling;
}

void Plane::setTexture(GLuint tex)
{
	m_texID = tex;
}