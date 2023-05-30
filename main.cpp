#include "bibutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>

GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
int lastX = 0;
int lastY = 0;
bool primeiraExecucao = true;
int x_ini,y_ini,bot;

// Apontador para objeto
OBJ *objeto;

// Fun��o respons�vel pela especifica��o dos par�metros de ilumina��o
void DefineIluminacao (void)
{
	GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
	GLfloat luzDifusa[4]={1.0,1.0,1.0,1.0};	  
	GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};
	GLfloat posicaoLuz[4]={0.0, 10.0, 100.0, 1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 60;


	// Define a reflet�ncia do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentra��o do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os par�metros da luz de n�mero 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
}

// Fun��o callback de redesenho da janela de visualiza��o
void Desenha(void)
{
	// Limpa a janela de visualiza��o com a cor
	// de fundo definida previamente
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// Altera a cor do desenho para rosa
	glColor3f(1.0f, 0.0f, 1.0f);

	// Desenha o objeto 3D lido do arquivo com a cor corrente
	glPushMatrix();
    glRotatef(rotX,1,0,0);
	glRotatef(rotY,0,1,0);

	DesenhaObjeto(objeto);
    glPopMatrix();

	// Executa os comandos OpenGL
	glutSwapBuffers();
}

void PosicionaObservador(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posiciona a câmera
    gluLookAt(obsX, obsY, obsZ, obsX + sin(rotY * M_PI / 180.0), obsY - sin(rotX * M_PI / 180.0), obsZ - cos(rotY * M_PI / 180.0), 0.0, 1.0, 0.0);
}

// Fun��o usada para especificar o volume de visualiza��o
void EspecificaParametrosVisualizacao(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(angle,fAspect,0.01,1200);

	PosicionaObservador();
	DefineIluminacao();

}

// Fun��o callback chamada quando o tamanho da janela � alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divis�o por zero
	if ( h == 0 ) h = 1;

	// Especifica as dimens�es da viewport
	glViewport(0, 0, w, h);

	// Calcula a corre��o de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
}

// Fun��o callback chamada para gerenciar eventos de teclas normais (ESC)
void TeclasNormais(unsigned char tecla, int x, int y)
{
    float velocidadeMovimento = 1.0f;

    switch (tecla)
    {
        case 'w': // Movimento para frente
            obsX += velocidadeMovimento * sin(rotY * M_PI / 180.0);
            obsZ -= velocidadeMovimento * cos(rotY * M_PI / 180.0);
            break;
        case 's': // Movimento para trás
            obsX -= velocidadeMovimento * sin(rotY * M_PI / 180.0);
            obsZ += velocidadeMovimento * cos(rotY * M_PI / 180.0);
            break;
        case 'a': // Movimento para o lado esquerdo
            obsX -= velocidadeMovimento * cos(rotY * M_PI / 180.0);
            obsZ -= velocidadeMovimento * sin(rotY * M_PI / 180.0);
            break;
        case 'd': // Movimento para o lado direito
            obsX += velocidadeMovimento * cos(rotY * M_PI / 180.0);
            obsZ += velocidadeMovimento * sin(rotY * M_PI / 180.0);
            break;
        case 27: // Tecla ESC
            LiberaObjeto(objeto);
            exit(0);
            break;
    }

    PosicionaObservador();
    glutPostRedisplay();
}

void TeclasEspeciais(int tecla, int x, int y)
{
    float velocidadeMovimento = 1.0f;

    switch (tecla)
    {
        case 32: // Tecla ESPAÇO (mover para cima)
            obsY += velocidadeMovimento;
            break;
        case 16: // Tecla SHIFT (mover para baixo)
            obsY -= velocidadeMovimento;
            break;
    }

    PosicionaObservador();
    glutPostRedisplay();
}

void GerenciaMouse(int button, int state, int x, int y)
{
	if(state==GLUT_DOWN)
	{
		// Salva os par�metros atuais
		x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		bot = button;
	}
	else bot = -1;
}

// Fun��o callback para eventos de movimento do mouse
#define SENS_ROT	5.0
#define SENS_OBS	15.0
#define SENS_TRANSL	20.0

void GerenciaMovim(int x, int y)
{
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    rotY += deltaX / SENS_ROT;
    rotX += deltaY / SENS_ROT;

    lastX = x;
    lastY = y;

    PosicionaObservador();
    glutPostRedisplay();
}

void GerenciaMovimPassivo(int x, int y)
{
    if (primeiraExecucao) {
        lastX = x;
        lastY = y;
        primeiraExecucao = false;
    }

    int deltaX = x - lastX;
    int deltaY = y - lastY;

    rotY += deltaX / SENS_ROT;
    rotX += deltaY / SENS_ROT;

    lastX = x;
    lastY = y;

    PosicionaObservador();
    glutPostRedisplay();
}

void Inicializa (void)
{
	char nomeArquivo[30];

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	angle=55;

	obsX = obsY = 0;
	obsZ = 100;

	objeto = CarregaObjeto("teapot.obj",true);
    printf("Objeto carregado!");

	// E calcula o vetor normal em cada face
	if(objeto->normais)
	{
		// Se j� existirem normais no arquivo, apaga elas
		free(objeto->normais);
		objeto->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(objeto);
}

// Programa Principal
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(5,5);
	glutInitWindowSize(450,450);
	glutCreateWindow("Projeto 3D");
	glutSetCursor(GLUT_CURSOR_NONE); 
	glutFullScreen();
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutKeyboardFunc(TeclasNormais);
	glutSpecialFunc(TeclasEspeciais);
	glutMouseFunc(GerenciaMouse);
	glutMotionFunc(GerenciaMovim);
	glutPassiveMotionFunc(GerenciaMovimPassivo);
	Inicializa();
	glutMainLoop();
	return 0;
}
