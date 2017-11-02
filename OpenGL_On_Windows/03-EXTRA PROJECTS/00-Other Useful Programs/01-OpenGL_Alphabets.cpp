#include<Windows.h>
#include<gl/GL.h>
#include<math.h>

#define PI 3.1415

void Draw_A(void)
{
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(-0.3f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glVertex3f(-0.17f, -0.1f, 0.0f);
	glVertex3f(0.17f, -0.1f, 0.0f);
	glEnd();
}

void Draw_B(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.2f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_B = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_B*cos(angle) - 0.24f, fRadius_For_B*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();

	glBegin(GL_POINTS);
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_B*cos(angle) + 0.24f, fRadius_For_B*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_C(void)
{
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_C = 0.45f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_C*cos(angle), fRadius_For_C*sin(angle), 0.0f);
	}
	glEnd();
}

void Draw_D(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.1f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.1f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_D = 0.5f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_D*cos(angle), fRadius_For_D*sin(angle) + 0.1f, 0.0f);
	}
	glEnd();
}

void Draw_E(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.35f, 0.0f, 0.0f);
	glEnd();
}

void Draw_F(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.35f, 0.0f, 0.0f);
	glEnd();
}

void Draw_G(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.1f, 0.0f, 0.0f);
	glVertex3f(0.1f, -0.5f, 0.0f);
	glVertex3f(-0.1f, 0.0f, 0.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glEnd();

	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_G = 0.42f;
	for (GLfloat angle = 0.0f; angle < 1.05f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_G*cos(angle) + 0.1f, fRadius_For_G*sin(angle), 0.0f);
	}
	glEnd();
}

void Draw_H(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();
}

void Draw_I(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();
}


void Draw_J(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.4f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_J = 0.125f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_J*cos(angle) - 0.12f, fRadius_For_J*sin(angle) + 0.4f, 0.0f);
	}
	glEnd();
}

void Draw_K(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, -0.1f, 0.0f);
	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(0.1f, 0.03f, 0.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glEnd();
}

void Draw_L(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();
}

void Draw_M(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glEnd();
}

void Draw_N(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();
}

void Draw_O(void)
{
	glScalef(0.6f, 1.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_O = 0.45f;
	for (GLfloat angle = 0.0f; angle < 2.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_O*cos(angle), fRadius_For_O*sin(angle), 0.0f);
	}
	glEnd();
}

void Draw_P(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.2f, -0.01f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_P = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_P*cos(angle) - 0.24f, fRadius_For_P*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_Q(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, -0.2f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glEnd();

	glScalef(0.6f, 1.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_Q = 0.45f;
	for (GLfloat angle = 0.0f; angle < 2.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_Q*cos(angle), fRadius_For_Q*sin(angle), 0.0f);
	}
	glEnd();
}

void Draw_R(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.2f, -0.01f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_R = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_R*cos(angle) - 0.24f, fRadius_For_R*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_S(void)
{
	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_S = 0.25f;
	for (GLfloat angle = 1.8f; angle < 2.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_S*cos(angle) - 0.24f, fRadius_For_S*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();

	glBegin(GL_POINTS);
	for (GLfloat angle = -0.9f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_S*cos(angle) + 0.25f, fRadius_For_S*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_T(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();
}

void Draw_U(void)
{
	glBegin(GL_LINES);
	glVertex3f(-0.01f, 0.5f, 0.0f);
	glVertex3f(-0.01f, -0.2f, 0.0f);
	glVertex3f(0.49f, 0.5f, 0.0f);
	glVertex3f(0.49f, -0.2f, 0.0f);
	glEnd();

	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_U = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_U*cos(angle) - 0.24f, fRadius_For_U*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_V(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void Draw_W(void)
{
	glPointSize(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.75f, -0.5f, 0.0f);
	glVertex3f(0.75f, -0.5f, 0.0f);
	glVertex3f(1.0f, 0.5f, 0.0f);
	glEnd();
}

void Draw_X(void)
{
	glPointSize(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glEnd();
}

void Draw_Y(void)
{
	glPointSize(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void Draw_Z(void)
{
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0, -0.5f, 0.0f);
	glEnd();
}
