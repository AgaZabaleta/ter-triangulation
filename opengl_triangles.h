#ifndef OPENGL_TRIANGLES_H
#define OPENGL_TRIANGLES_H

#include <QtWidgets>

class OpenGLTriangles : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLTriangles(QWidget *parent = nullptr);
//    ~OpenGLTriangles();

protected:
    void initializeGL() override;
    void paintGL() override;
//    void resizeGL(int width, int height) override;

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram *m_program;
};

#endif // OPENGL_TRIANGLES_H
