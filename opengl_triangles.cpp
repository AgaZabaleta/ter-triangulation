#include "opengl_triangles.h"

static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main()\n"
    "{\n"
    "   vec4 vertex = vec4(aPos, 1.0);\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\0";

static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

OpenGLTriangles::OpenGLTriangles(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{
}

void OpenGLTriangles::linkTrianguledImage(Trianguled_image* t_i){
    t_image = t_i;
}

void OpenGLTriangles::initializeGL()
{
//    std::vector<GLfloat> vertices;
//    vertices.clear();
//    if(t_image->getTriangles().size() == 0){
//        vertices.push_back(0.0f);
//        vertices.push_back(0.25f);
//        vertices.push_back(0.0f);

//        vertices.push_back(-0.5f);
//        vertices.push_back(-0.25f);
//        vertices.push_back(0.0f);

//        vertices.push_back(0.5f);
//        vertices.push_back(-0.25f);
//        vertices.push_back(0.0f);
//    }

    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");

    m_camera.setToIdentity();
    m_camera.translate(0, 0, -5);

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
//    m_vbo.bind();
//    m_vbo.allocate(&vertices[0], vertices.size()*sizeof(GLfloat));

//    m_vbo.bind();
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3* sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
//    m_vbo.release();

    m_program->release();
}

int OpenGLTriangles::updateVertices(){
    std::vector<GLfloat> vertices;

    double scale_x, scale_y, adapt_x, adapt_y;
    scale_x = t_image->getScaleX();
    scale_y = t_image->getScaleY();

    if(scale_x > scale_y) {
        adapt_y = scale_y / scale_x;
        adapt_x = 1.0;
    } else {
        adapt_x = scale_x / scale_y;
        adapt_y = 1.0;
    }

    for(Triangle* t : t_image->getTriangles()){
        vertices.push_back(QPointToPositionX(*(t_image->getPoints()[t->getP1()])) * adapt_x);
        vertices.push_back(QPointToPositionY(*(t_image->getPoints()[t->getP1()])) * adapt_y);
        vertices.push_back(0.0f);

        vertices.push_back(QPointToPositionX(*(t_image->getPoints()[t->getP2()])) * adapt_x);
        vertices.push_back(QPointToPositionY(*(t_image->getPoints()[t->getP2()])) * adapt_y);
        vertices.push_back(0.0f);

        vertices.push_back(QPointToPositionX(*(t_image->getPoints()[t->getP3()])) * adapt_x);
        vertices.push_back(QPointToPositionY(*(t_image->getPoints()[t->getP3()])) * adapt_y);
        vertices.push_back(0.0f);
    }

//    m_program = new QOpenGLShaderProgram(this);
//    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
//    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
//    m_program->link();

//    m_program->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.bind();
    m_vbo.allocate(&vertices[0], vertices.size()*sizeof(GLfloat));

    m_vbo.bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();

//    m_program->release();

    return t_image->getTriangles().size();
}

void OpenGLTriangles::paintGL()
{
    int nbTriangles = 1;
    if(t_image->getTriangles().size() != 0){
        nbTriangles = updateVertices();
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

        m_program->bind();
        m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
        m_program->setUniformValue(m_projMatrixLoc, m_proj);
        glDrawArrays(GL_TRIANGLES, 0, 3*nbTriangles);


        m_program->release();
    } 
}

void OpenGLTriangles::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

GLfloat OpenGLTriangles::QPointToPositionX(QPointF point){
    return static_cast<GLfloat>(point.x()) * 2.0f - 1.0f;
}

GLfloat OpenGLTriangles::QPointToPositionY(QPointF point){
    return -1.0f * (static_cast<GLfloat>(point.y()) * 2.0f - 1.0f);
}
