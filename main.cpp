#include "bibutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>

GLfloat angle, fAspect;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;

GLfloat dirX = 0.0f;
GLfloat dirZ = -1.0f;

int windowWidth;
int windowHeight;

int lastX = 0;
int lastY = 0;
bool primeiraExecucao = true;
int x_ini,y_ini,bot;

float velocidadeVertical = 0.0f; // Velocidade vertical inicial
float aceleracaoGravidade = -0.05f; 

// Apontador para objeto
OBJ *objeto;

// Flags para o estado das teclas
bool keyW = false;
bool keyA = false;
bool keyS = false;
bool keyD = false;

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
    // Limpa a janela de visualização com a cor de fundo definida previamente
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    // Desenha as linhas de referência dos eixos x, y e z
    glBegin(GL_LINES);
    // Eixo x (vermelho)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    // Eixo y (verde)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Eixo z (azul)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();

    // Desenha a mira no centro da tela
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desabilita a iluminação apenas para a mira
    glDisable(GL_LIGHTING);

    glColor3f(0.0f, 1.0f, 0.0f);
    float tamanhoMira = 10.0f; // Define o tamanho da mira
    float espacoCentral = 3.0f; // Define o tamanho do espaço em branco no centro

    glLineWidth(2.3f); // Define a largura das linhas da mira

    glBegin(GL_LINES);
    // Linhas verticais
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 - tamanhoMira);
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 - espacoCentral);

    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 + espacoCentral);
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2 + tamanhoMira);

    // Linhas horizontais
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2 - tamanhoMira, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2 - espacoCentral, glutGet(GLUT_WINDOW_HEIGHT) / 2);

    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2 + espacoCentral, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    glVertex2f(glutGet(GLUT_WINDOW_WIDTH) / 2 + tamanhoMira, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);

    // Altera a cor do desenho para rosa
    glColor3f(1.0f, 0.0f, 1.0f);

    // Desenha o objeto 3D lido do arquivo com a cor corrente
    glPushMatrix();
    DesenhaObjeto(objeto);
    glPopMatrix();

    // Executa os comandos OpenGL
    glutSwapBuffers();
}

void PosicionaObservador(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    dirX = sin(rotY * M_PI / 180.0);
    dirZ = -cos(rotY * M_PI / 180.0);

    gluLookAt(obsX, obsY + 20.0f, obsZ,
              obsX + dirX, obsY + 20.0f - sin(rotX * M_PI / 180.0), obsZ + dirZ,
              0.0, 1.0, 0.0);
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

	// Atualiza as dimensões da janela
    windowWidth = w;
    windowHeight = h;

	EspecificaParametrosVisualizacao();
}

// Fun��o callback chamada para gerenciar eventos de teclas normais (ESC)
void TeclasNormais(unsigned char tecla, int x, int y)
{
    switch (tecla){
		case 'w':
		case 'W':
			keyW = true;
			break;
		case 'a':
		case 'A':
			keyA = true;
			break;
		case 's':
		case 'S':
			keyS = true;
			break;
		case 'd':
		case 'D':
			keyD = true;
			break;
		case 32: // Tecla ESPAÇO
            if (obsY == 0.0f) // Verifica se o objeto está no chão antes de pular
                velocidadeVertical = 0.7f; // Define a velocidade vertical do pulo
            break;
		case 27:
			exit(0);
			break;
    }
}

// Função callback chamada para gerenciar o evento de soltar as teclas normais
void TecladoUp(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        keyW = false;
        break;
    case 'a':
    case 'A':
        keyA = false;
        break;
    case 's':
    case 'S':
        keyS = false;
        break;
    case 'd':
    case 'D':
        keyD = false;
        break;
    }
}

void AtualizaCamera()
{
    if (keyW)
    {
        obsX += 0.5f * dirX;
        obsZ += 0.5f * dirZ;
    }
    if (keyA)
    {
        obsX += 0.5f * dirZ;  // Trocar para adição
        obsZ -= 0.5f * dirX;  // Trocar para subtração
    }
    if (keyS)
    {
        obsX -= 0.5f * dirX;
        obsZ -= 0.5f * dirZ;
    }
    if (keyD)
    {
        obsX -= 0.5f * dirZ;  // Trocar para subtração
        obsZ += 0.5f * dirX;  // Trocar para adição
    }

    glutPostRedisplay();
}

// Fun��o callback para eventos de movimento do mouse
#define SENS_ROT	4.0
#define SENS_OBS	10.0
#define SENS_TRANSL	15.0

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

	// Limita a rotação vertical entre -90 e 90 graus
    if (rotX > 90.0f) rotX = 90.0f;
    if (rotX < -90.0f) rotX = -90.0f;

    lastX = x;
    lastY = y;

    PosicionaObservador();
    glutPostRedisplay();
}

void Inicializa(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_SMOOTH);

    angle = 55;

    obsX = obsY = 0;
    obsZ = 100;

    objeto = CarregaObjeto("teapot.obj", true);
    printf("Objeto carregado!");

    // E calcula o vetor normal em cada face
    if (objeto->normais)
    {
        // Se já existirem normais no arquivo, apaga elas
        free(objeto->normais);
        objeto->normais_por_vertice = false;
    }
    CalculaNormaisPorFace(objeto);
}


void AtualizaPulo(int valor)
{
    obsY += velocidadeVertical; // Aplica a velocidade vertical para mover o objeto para cima
    velocidadeVertical += aceleracaoGravidade; // Aplica a aceleração da gravidade

    if (obsY < 0.0f) // Verifica se o objeto chegou ao chão
    {
        obsY = 0.0f; // Mantém o objeto no chão
        velocidadeVertical = 0.0f; // Reseta a velocidade vertical
    }

    PosicionaObservador();
    glutPostRedisplay();

    glutTimerFunc(16, AtualizaPulo, 0); // Chama a função novamente após um intervalo de tempo
}

// Programa Principal
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Projeto 3D");
	// glutSetCursor(GLUT_CURSOR_NONE); 
	glutFullScreen();
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutKeyboardFunc(TeclasNormais);
	glutKeyboardUpFunc(TecladoUp);
	glutPassiveMotionFunc(GerenciaMovimPassivo);
	glutIdleFunc(AtualizaCamera);
	glutTimerFunc(16, AtualizaPulo, 0);
	Inicializa();
	glutMainLoop();
	return 0;
}
