#ifndef OPENGL_TRIANGLES_H
#define OPENGL_TRIANGLES_H

#include <QtWidgets>

#include "trianguled_image.h"

class OpenGLTriangles : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLTriangles(QWidget *parent = nullptr);
    void linkTrianguledImage(Trianguled_image*);
//    ~OpenGLTriangles();

protected:
    void initializeGL() override;
    void paintGL() override;
//    void resizeGL(int width, int height) override;

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram *m_program;
    Trianguled_image *t_image;

    GLfloat QPointToPositionX(QPointF point);
    GLfloat QPointToPositionY(QPointF point);
    int updateVertices();
};

#endif // OPENGL_TRIANGLES_H
