#include "libs/bibutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <GL/freeglut.h>
#include <SOIL.h>

#define DEBUG 0

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

OBJ *coisa;

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
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

GLuint texturaTerreno = 0; 

void DesenharTerreno(float tamanhoTerreno)
{
     // Verifica se a textura do terreno já foi carregada
    if (texturaTerreno == 0)
    {
        // Carregar a textura do terreno
        texturaTerreno = SOIL_load_OGL_texture(
            "textures/floor.png",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
        );

        if (texturaTerreno == 0)
        {
            printf("Erro ao carregar a textura do terreno: %s\n", SOIL_last_result());
            return;
        }
    }

    // Habilitar o uso de texturas
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaTerreno);

    // Configurar os parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Definir a altura do terreno em relação à origem
    float alturaTerreno = -1.0f;

    // Desenhar o terreno como uma malha de quadrados
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamanhoTerreno / 2, alturaTerreno, -tamanhoTerreno / 2);

    glTexCoord2f(0.0f, tamanhoTerreno / 100.0f);
    glVertex3f(-tamanhoTerreno / 2, alturaTerreno, tamanhoTerreno / 2);

    glTexCoord2f(tamanhoTerreno / 100.0f, tamanhoTerreno / 100.0f);
    glVertex3f(tamanhoTerreno / 2, alturaTerreno, tamanhoTerreno / 2);

    glTexCoord2f(tamanhoTerreno / 100.0f, 0.0f);
    glVertex3f(tamanhoTerreno / 2, alturaTerreno, -tamanhoTerreno / 2);
    glEnd();
    
    glDisable(GL_BLEND);
    // Desabilitar o uso de texturas
    glDisable(GL_TEXTURE_2D);
}

GLuint texturaLua = 0;

void ConfigurarTexturaLua()
{
    if(texturaLua == 0){
        // Carregar a textura da lua
        texturaLua = SOIL_load_OGL_texture(
            "textures/lua.jpg",
            SOIL_LOAD_RGBA,  // Carregar a textura com canal alfa (transparência)
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS
        );

        if (texturaLua == 0)
        {
            printf("Erro ao carregar a textura da lua: %s\n", SOIL_last_result());
            return;
        }
    }
}

void DesenharRetangulo3D(float largura, float altura, float profundidade)
{
    // Ajustar a escala do cubo para criar um retângulo
    glScalef(largura, altura, profundidade);

    // Desenhar o cubo
    glutSolidCube(1.0f);
}

void DesenharCuboRua(float comprimentoRua, float larguraRua)
{
    // Definir a altura da rua em relação à origem
    float alturaRua = 3.5f;

    // Definir a cor preta
    glColor3f(0.4f, 0.4f, 0.4f);

    // Redimensionar e desenhar o cubo da rua
    glPushMatrix();
    glScalef(larguraRua + 8, alturaRua, comprimentoRua);
    glutSolidCube(1.0f);
    glPopMatrix();
}

GLuint texturaRua = 0;

void DesenharRua(float comprimentoRua, float larguraRua)
{
    // Verifica se a textura da rua já foi carregada
    if (texturaRua == 0)
    {
        // Carregar a textura da rua
        texturaRua = SOIL_load_OGL_texture(
            "textures/rua.png",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_TEXTURE_REPEATS
        );

        if (texturaRua == 0)
        {
            printf("Erro ao carregar a textura da rua: %s\n", SOIL_last_result());
            return;
        }
    }

    // Habilitar o uso de texturas
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaRua);

    // Configurar os parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Definir a altura da rua em relação à origem
    float alturaRua = 2.0f;

    // Desenhar a rua como um paralelepípedo
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    // Face frontal
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-larguraRua / 2, alturaRua, -comprimentoRua / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(larguraRua / 2, alturaRua, -comprimentoRua / 2);

    // Face traseira
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(larguraRua / 2, alturaRua, -comprimentoRua / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-larguraRua / 2, alturaRua, -comprimentoRua / 2);

    // Restantes das faces (laterais)
    // Face esquerda
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-larguraRua / 2, alturaRua, -comprimentoRua / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-larguraRua / 2, alturaRua + 0.01f, comprimentoRua / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-larguraRua / 2, alturaRua + 0.01f, -comprimentoRua / 2);

    // Face direita
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(larguraRua / 2, alturaRua, comprimentoRua / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(larguraRua / 2, alturaRua, -comprimentoRua / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(larguraRua / 2, alturaRua + 0.01f, -comprimentoRua / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(larguraRua / 2, alturaRua + 0.01f, comprimentoRua / 2);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void DesenhaLua()
{
    // Define as propriedades da lua
    float raio = 40.0f;
    int numMeridianos = 36;
    int numParalelos = 18;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaLua);

    // Configurar os parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Desenhar a esfera da lua
    for (int i = 0; i < numParalelos; i++)
    {
        float lat1 = i * (180.0f / (numParalelos - 1)) - 90.0f;
        float lat2 = (i + 1) * (180.0f / (numParalelos - 1)) - 90.0f;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= numMeridianos; j++)
        {
            float lon = j * (360.0f / numMeridianos);

            float x1 = raio * cos(lat1 * 3.14159f / 180.0f) * cos(lon * 3.14159f / 180.0f);
            float y1 = raio * cos(lat1 * 3.14159f / 180.0f) * sin(lon * 3.14159f / 180.0f);
            float z1 = raio * sin(lat1 * 3.14159f / 180.0f);

            float x2 = raio * cos(lat2 * 3.14159f / 180.0f) * cos(lon * 3.14159f / 180.0f);
            float y2 = raio * cos(lat2 * 3.14159f / 180.0f) * sin(lon * 3.14159f / 180.0f);
            float z2 = raio * sin(lat2 * 3.14159f / 180.0f);

            // Definir as coordenadas de textura
            float s = (float)j / numMeridianos;
            float t1 = (float)i / (numParalelos - 1);
            float t2 = (float)(i + 1) / (numParalelos - 1);

            glTexCoord2f(s, t1);
            glVertex3f(x1, y1, z1);

            glTexCoord2f(s, t2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}
// Fun��o callback de redesenho da janela de visualiza��o
void Desenha(void)
{
    // Limpa a janela de visualização com a cor de fundo definida previamente
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    #if DEBUG
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
    #endif

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

    // Desenha terreno
    DesenharTerreno(1000.0f);

    // Desenha lua
    glPushMatrix();
    glTranslatef(700.0f, 500.0f, 0.0f);
    ConfigurarTexturaLua();
    DesenhaLua();
    glPopMatrix();

    // Altera a cor do desenho para rosa
    glColor3f(1.0f, 0.0f, 1.0f);

    // Desenha o objeto 3D lido do arquivo com a cor corrente
    glPushMatrix();
    glScalef(12.0f, 12.0f, 12.0f);
    glEnable(GL_NORMALIZE);
    DesenhaObjeto(objeto);
    glPopMatrix();

    // Alterando carro e desenhando
    glPushMatrix();
    glTranslatef(25.0f, 000.0f, 40.0f);
    glScalef(12.0f, 12.0f, 12.0f);
    DesenhaObjeto(coisa);
    glPopMatrix();

     // Alterando carro e desenhando
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 150.0f);
    glRotatef(90, 0, 1, 0);
    DesenharRua(1000, 100);
    glPopMatrix();

    // Desenhar cubo abaixo da rua
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 150.0f);
    glRotatef(90, 0, 1, 0);
    DesenharCuboRua(1000, 100);
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
    // Define a cor de fundo como um céu escuro
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);

    angle = 55;

    obsX = obsY = 0;
    obsZ = 100;

    objeto = CarregaObjeto("models/Bambo_House.obj", true);
    printf("Objeto carregado!");

    if (objeto->normais)
    {
        // Se já existirem normais no arquivo, apaga elas
        free(objeto->normais);
        objeto->normais_por_vertice = false;
    }

    coisa = CarregaObjeto("models/Car.obj", true);
    printf("Objeto carregado!");

    // E calcula o vetor normal em cada face
    CalculaNormaisPorFace(objeto);

    if (coisa->normais)
    {
        // Se já existirem normais no arquivo, apaga elas
        free(coisa->normais);
        coisa->normais_por_vertice = false;
    }
    CalculaNormaisPorFace(coisa);
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
	glutSetCursor(GLUT_CURSOR_NONE); 
	glutFullScreen();
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutKeyboardFunc(TeclasNormais);
	glutKeyboardUpFunc(TecladoUp);
	glutPassiveMotionFunc(GerenciaMovimPassivo);
	glutIdleFunc(AtualizaCamera);

    // Timer para animação do pulo
	glutTimerFunc(16, AtualizaPulo, 0);
	Inicializa();
	glutMainLoop();
	return 0;
}
