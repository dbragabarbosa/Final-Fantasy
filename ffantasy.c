#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NAVEGACAO 0
#define BATALHA 1
#define TAM_HEROI 30
#define PASSO 25
#define TAM_OBJ 100
#define MAX_DIST 30
#define ATACAR 0
#define ESPECIAL 1
#define FUGIR 2
#define RECEBER_ATAQUE 3

int playing = 1;   //se acabou o jogo ou não
int modo_jogo = NAVEGACAO;     //inicializa o jogo na navegacao

const float FPS = 100;  

const int SCREEN_W = 960; //largura da tela
const int SCREEN_H = 540; //altura da tela

int CEU_H;
int X_OPT;
int Y_OPT;
ALLEGRO_FONT *FONTE;

int X_BOLINHA;
int Y_BOLINHA;

int p_mons = 0; 
int vidaMonstro = 0;

int X_ATAQUE = 720;
int X_RECEBE_ATQ = 240;
int variacaoAtq = 2.0;

int vidaPersonagem = 100;
char vida_personagem[10] = "100";

//int vida_monstro = 0;
char vida_monstro[10] = "0";

int pontuacao = 0;
char pontos[10] = "0";

int ataca_agora = 0;
int ataca_especial = 0;
int recebe_ataque= 0;

int r_monstro = 0;
int r_esp = 0;

int r_contraAtaque = 0;
int vaiTerContraAtaque = 1;

int telaGameOver = 0; //1=SIM 0=NAO
int telaVoceVenceu = 0; //1=SIM 0=NAO



typedef struct Heroi 
{
	int x, y;
	ALLEGRO_COLOR cor;
	int vida;
	
	int direcao; 
	//0 = cima, 1 = baixo, 2 = dir, 3 = esq
	
	//batalha
	int acao;
	int executar;
	int x_old, y_old;

} Heroi;

void initHeroi (Heroi *h)
{
	h->x = (TAM_HEROI/2);     //0 ;
	h->y = (SCREEN_H - TAM_HEROI/2) ;
	h->cor = al_map_rgb(225,32,65);    //(144,238,144); --> VERDE CLARO
	h->vida = 100;
	
	h->direcao = 0;
	
	h->acao = ESPECIAL;
	h->executar = 0;
	h->x_old = h->x;
	h->y_old = h->y;
}

typedef struct Monstro
{
	int x, y, r;
	ALLEGRO_COLOR cor;
	ALLEGRO_COLOR cor_sec;
	int vida;
	int vida_atual;
	char life[10];
} Monstro;


void criaMonstros (Monstro M[])
{
	int i=0;
	int aux;
	
	for (i=0; i<20; i++)
	{
			aux = rand()%10;
			
			if (aux<=3)
			{
				M[i].cor = al_map_rgb(239,255,0);
				M[i].cor_sec = al_map_rgb(167,179,0);
				M[i].vida = 30;
				//M[i].life = "30";
				M[i].r = 45;
			}
			else if (aux<=6)
			{
				M[i].cor = al_map_rgb(255,111,0);
				M[i].cor_sec = al_map_rgb(205,89,0);
				M[i].vida = 60;
				//M[i].life = "60";
				M[i].r = 70;
			}
			else if (aux<=9)
			{
				M[i].cor = al_map_rgb(255,0,0);
				M[i].cor_sec = al_map_rgb(179,0,0);
				M[i].vida = 90;
				//M[i].life = "90";
				M[i].r = 95;
			}
			
			M[i].vida_atual = M[i].vida;
	}
	
	int aux1=0;
	int aux2=0;
	
	for (i=0; i<4; i++)
	{
		do 
		{
			aux1 = rand();
		} while (aux1<0 || aux1>(SCREEN_W-TAM_OBJ));
		
		do 
		{
			aux2 = rand();
		} while (aux2<0 || aux2>TAM_OBJ);
		
		M[i].x = aux1;
		M[i].y = aux2;
		
		printf("\nMonstro criado em (%d, %d).", M[i].x, M[i].y);
	}
	
	int j=4;
	for (j=4; j<20; j++)
	{
		do 
		{
			aux1 = rand();
		} while (aux1<0 || aux1>SCREEN_W);
		
		do 
		{
			aux2 = rand();
		} while (aux2<TAM_OBJ || aux2>SCREEN_H);
		
		M[j].x = aux1;
		M[j].y = aux2;
		
		printf("\nMonstro criado em (%d, %d).", M[j].x, M[j].y);
	}
}


void initGlobais ()
{
	CEU_H = (SCREEN_H/4)-30;
	X_OPT = 3*SCREEN_W/4;
	Y_OPT = 3*SCREEN_H/4;
	
	X_BOLINHA = X_OPT;
	Y_BOLINHA = Y_OPT;
	
	int tam_fonte = 32;
	if (SCREEN_H < 300)
	{
		tam_fonte = 16;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("arial.ttf", tam_fonte, 1);   
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
}


//FUNCAO PARA GERAR UM ALEATORIO ENTRE 0 E n-1
int random (int n)
{
	return rand()%n;
}

//FUNCAO PARA GERAR ALEATORIOS ENTRE MIN E MAX (INCLUSIVE)
int randInt (int min, int max)    //min=5, max=10
{
	return min + random(max - min + 1);
}


// ==================== NAVEGACAO ==============
void desenhaHeroiNaveg (Heroi h)
{
	if (telaVoceVenceu == 1)
	{
		
	}
	else 
	{
		if (h.direcao == 0)
			al_draw_filled_triangle( (h.x), (h.y-TAM_HEROI/2), (h.x-TAM_HEROI/2), (h.y+TAM_HEROI/2), (h.x+TAM_HEROI/2), (h.y+TAM_HEROI/2), h.cor);
		else if (h.direcao == 1)
			al_draw_filled_triangle( (h.x), (h.y+TAM_HEROI/2), (h.x+TAM_HEROI/2), (h.y-TAM_HEROI/2), (h.x-TAM_HEROI/2), (h.y-TAM_HEROI/2), h.cor);
		else if (h.direcao == 2)
			al_draw_filled_triangle( (h.x+TAM_HEROI/2), (h.y), (h.x-TAM_HEROI/2), (h.y-TAM_HEROI/2), (h.x-TAM_HEROI/2), (h.y+TAM_HEROI/2), h.cor);
		else if (h.direcao == 3)
			al_draw_filled_triangle( (h.x-TAM_HEROI/2), (h.y), (h.x+TAM_HEROI/2), (h.y+TAM_HEROI/2), (h.x+TAM_HEROI/2), (h.y-TAM_HEROI/2), h.cor);
	}
}


void desenhaCenarioNaveg ()
{
	int auxfinal=0;
	
	if (telaVoceVenceu == 1)
	{
		al_clear_to_color(al_map_rgb(0,0,0));
		al_draw_text(FONTE, al_map_rgb(0,68,255), ((SCREEN_W/2)-150), (SCREEN_H/2), 0, "VOCE VENCEU");
		
		sprintf(pontos, "%d", pontuacao);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)-150), ((SCREEN_H/2)+50), 0, "Score: ");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)), ((SCREEN_H/2)+50), 0, pontos);
		
		FILE *arq, *arqw;
		arq = fopen("recorde.txt", "r");
		arqw = fopen("recordenovo.txt", "w");
		int m=0;
		fscanf(arq, "%d", &m);
		//printf("\n %d", m);
		
		char recorde[10] = "0";
		//sprintf(pontos, "%d", pontuacao);
		
		if (m < pontuacao)
		{
			auxfinal=pontuacao;
			
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)-150), ((SCREEN_H/2)+150), 0, "Novo recorde");
			
			sprintf(recorde, "%d", pontuacao);
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)-150), ((SCREEN_H/2)+100), 0, "Recorde: ");
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)), ((SCREEN_H/2)+100), 0, recorde);
			
			fprintf(arqw, "%d", pontuacao);
		}
		else if (m >= pontuacao)
		{
			auxfinal=m;
			
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)-150), ((SCREEN_H/2)+100), 0, "Recorde: ");
			sprintf(recorde, "%d", m);
			al_draw_text(FONTE, al_map_rgb(255, 255, 255), ((SCREEN_W/2)), ((SCREEN_H/2)+100), 0, recorde);
		
			fprintf(arqw, "%d", m);
		}
		
		fclose(arq);
		fclose(arqw);
		/*
		FILE *arqx;
		arqx = fopen("recorde.txt", "w");
		fprintf(arqx, "%d", auxfinal);
		fclose(arqx);  */
	}
	else
	{
		al_clear_to_color(al_map_rgb(144,238,144));        //(58,114,56));
		
		al_draw_filled_circle (SCREEN_W-(TAM_OBJ/2), (TAM_OBJ/2), (TAM_OBJ/2), al_map_rgb(102, 88, 91));
		al_draw_filled_circle (SCREEN_W-(2*TAM_OBJ/3), (TAM_OBJ/2), (TAM_OBJ/4), al_map_rgb(0, 0, 0));
		al_draw_filled_rectangle (SCREEN_W - TAM_OBJ, (TAM_OBJ/2), SCREEN_W, TAM_OBJ, al_map_rgb(144,238,144));
		
		sprintf(pontos, "%d", pontuacao);
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), (SCREEN_W/20), (SCREEN_H/20), 0, pontos);
	}
}


void processaTeclaNaveg (Heroi *h, int tecla)
{
	h->x_old = h->x;
	h->y_old = h->y;
	
	switch (tecla)
	{
		case ALLEGRO_KEY_UP: 
		{
			if (h->y - (2*TAM_HEROI/3) - PASSO >= 0)
			{
				h->y -= PASSO;
				h->direcao = 0;
			}
			break;
		}
		case ALLEGRO_KEY_DOWN:
		{
			if (h->y + /*TAM_HEROI +*/ PASSO <= SCREEN_H)
			{
				h->y += PASSO;
				h->direcao = 1;
			}
			break;
		}
		case ALLEGRO_KEY_LEFT: 
		{
			if (h->x - PASSO >= 0)
			{
				h->x -= PASSO;
				h->direcao = 3;
			}
			break;
		}
		case ALLEGRO_KEY_RIGHT:
		{
			if (h->x + PASSO <= SCREEN_W)
			{
				h->x += PASSO;
				h->direcao = 2;
			}
			break;
		}
	}
}


int chegouObjetivo (Heroi h)
{
	if (h.x >= SCREEN_W-TAM_OBJ && h.y+TAM_HEROI <= TAM_OBJ)
	{
		//TELA DE WIN
		telaVoceVenceu = 1;
		
		//return 1;
	}
	else 
	{
		return 0;
	}
}

// ==================== BATALHA ==============
void desenhaMonstro (Monstro M[])
{	
	if (telaGameOver == 1)
	{
		al_clear_to_color(al_map_rgb(0,0,0));
		al_draw_text(FONTE, al_map_rgb(235,0,0), ((SCREEN_W/2)-70), (SCREEN_H/2), 0, "GAME OVER");
		//al_rest(5.0);
		//playing= 0;
	}
	else 
	{
		r_monstro = (M[p_mons].r/6);
		
		al_draw_filled_circle((SCREEN_W/4), (SCREEN_H/2), M[p_mons].r, M[p_mons].cor);
		
		//olhos
		al_draw_filled_circle((SCREEN_W/4-(M[p_mons].r/3)), (SCREEN_H/2-(M[p_mons].r/3)), (M[p_mons].r/4), al_map_rgb(70, 56, 56));
		al_draw_filled_circle((SCREEN_W/4+(M[p_mons].r/3)), (SCREEN_H/2-(M[p_mons].r/3)), (M[p_mons].r/4), al_map_rgb(70, 56, 56));
		
		//boca
		al_draw_filled_rectangle(((SCREEN_W/4)-(M[p_mons].r/2)), ((SCREEN_H/2)+(M[p_mons].r/2)), 
								((SCREEN_W/4)+(M[p_mons].r/2)), ((SCREEN_H/2)+(M[p_mons].r/2)+10),
								al_map_rgb(70, 56, 56));
		
		
		sprintf(vida_monstro, "%d", M[p_mons].vida);
		al_draw_text((FONTE), al_map_rgb(225,0,0), ((SCREEN_W/4)-20), ((SCREEN_H/2)-140), 0, vida_monstro);
	}
}


void TiraVidaMonstro (Monstro M[], int n)
{
	M[p_mons].vida -= n;
	
	if (M[p_mons].vida <=0)
	{
		//volta pra navegacao
		modo_jogo = NAVEGACAO;
		M[p_mons].x = -100;
		M[p_mons].y = -100;
		
		//incrementa a pontuacao 
		pontuacao += M[p_mons].vida_atual;
		
		vaiTerContraAtaque = 0;
		
	}
}


void TiraVidaPersonagem (Monstro M[], int n)
{
	vidaPersonagem -= n;
	
	if (vidaPersonagem <=0)
	{
		//TERMINA O JOGO COM GAME OVER
		telaGameOver = 1;
		//playing = 0;
	}
}


void desenhaCenarioBatalha ()
{
	if (telaGameOver == 1)
	{
		al_clear_to_color(al_map_rgb(0,0,0));
		al_draw_text(FONTE, al_map_rgb(235,0,0), ((SCREEN_W/2)-70), (SCREEN_H/2), 0, "GAME OVER");
		//al_rest(5.0);
		//playing= 0;
	}
	else
	{
		al_clear_to_color(al_map_rgb(58,114,56));
		al_draw_filled_rectangle(0, 0, SCREEN_W, CEU_H, al_map_rgb(84, 158, 255));
		al_draw_filled_rectangle(X_OPT, Y_OPT, SCREEN_W, SCREEN_H, al_map_rgb(0, 0, 0));

		al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT+50, Y_OPT+5, 0, "Atacar");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT+50, Y_OPT+45, 0, "Especial");
		al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT+50, Y_OPT+85, 0, "Fugir");
		
		//jogador
		al_draw_filled_triangle (X_OPT, (SCREEN_H/2), (X_OPT+110), (SCREEN_H/2 + 40), (X_OPT+110), (SCREEN_H/2 - 40), al_map_rgb(225,32,65));
		
		sprintf(vida_personagem, "%d", vidaPersonagem);
		al_draw_text((FONTE), al_map_rgb(225,32,65), (X_OPT), ((SCREEN_H/2)-140), 0, vida_personagem);
	}
}


float dist (int x1, int y1, int x2, int y2)
{
	return sqrt ( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}


int detectouMonstro (Heroi h, Monstro M[])
{
	int i=0;	
	for (i=0; i<20; i++)
	{
		if (dist (h.x, h.y, M[i].x, M[i].y) <= MAX_DIST)
		{
			printf("\n\nInimigo encontrado em: (%d, %d).", M[i].x, M[i].y);
			p_mons = i;
			vidaMonstro = M[i].vida;
			return 1;
		}
	}
	return 0;
} 


void desenhaBolinha (Heroi h)
{
	if (telaGameOver == 1)
	{
		al_clear_to_color(al_map_rgb(0,0,0));
		al_draw_text(FONTE, al_map_rgb(235,0,0), ((SCREEN_W/2)-70), (SCREEN_H/2), 0, "GAME OVER");
		//al_rest(5.0);
		//playing= 0;
	}
	else
	{
		if (h.acao == FUGIR)
			al_draw_filled_circle(X_BOLINHA+25, Y_BOLINHA+105, 7, al_map_rgb(255, 255, 255));
		else if (h.acao == ESPECIAL)
			al_draw_filled_circle(X_BOLINHA+25, Y_BOLINHA+60, 7, al_map_rgb(255, 255, 255));
		else if (h.acao == ATACAR)
			al_draw_filled_circle(X_BOLINHA+25, Y_BOLINHA+20, 7, al_map_rgb(255, 255, 255));
	}
}
 
 
void processaTeclaBatalha (Heroi *h, int tecla)
{
	switch (tecla)
	{
		case ALLEGRO_KEY_UP: 
		{
			if (h->acao == FUGIR)
				h->acao = ESPECIAL;
			else if (h->acao == ESPECIAL)
				h->acao = ATACAR;
			else if (h->acao == ATACAR)
				h->acao = ATACAR;
			
			break;
		}
		case ALLEGRO_KEY_DOWN:
		{
			if (h->acao == FUGIR)
				h->acao = FUGIR;
			else if (h->acao == ESPECIAL)
				h->acao = FUGIR;
			else if (h->acao == ATACAR)
				h->acao = ESPECIAL;
			
			break;
		}
		case ALLEGRO_KEY_ENTER: 
		{
			h->executar = 1;
			break;
		}
	}
}

 
int processaAcaoHeroi(Heroi *h) 
{
	int aux;
	
	if (h->executar)
	{
		h->executar = 0;
		
		if (h->acao == FUGIR)
		{
			aux = rand()%10;
			
			if (aux <= 7)
			{
				h->x = h->x_old;
				h->y = h->y_old;
				
				return NAVEGACAO;
			}
			else 
			{
				recebe_ataque = 1;
			}
		}
		
		else if (h->acao == ATACAR)
		{
			ataca_agora = 1;
		}
		
		else if (h->acao == ESPECIAL)
		{
			r_esp = randInt(8,20);
			ataca_especial = 1;
		}
	}
	else 
	{
		return BATALHA;
	}
}
 
 
 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	//inicia o temporizador
	al_start_timer(timer);
	
	//int playing = 1;
	//int modo_jogo = NAVEGACAO;
	
	initGlobais();
	
	Heroi heroi;
	initHeroi(&heroi);
	
	Monstro monstros[20];
	criaMonstros (monstros);
	
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			if (modo_jogo == NAVEGACAO)
			{
				desenhaCenarioNaveg();
				desenhaHeroiNaveg(heroi);
				
				if (detectouMonstro(heroi, monstros))
				{
					modo_jogo = BATALHA;
				}
				
				if (chegouObjetivo(heroi))
				{
					//playing = 0;
				}
			}
			else 
			{
				desenhaCenarioBatalha();
				desenhaMonstro(monstros);
				desenhaBolinha(heroi);
				modo_jogo = processaAcaoHeroi(&heroi);
				
				
				if (ataca_agora == 1)
				{
					if (X_ATAQUE > (SCREEN_W/4))
					{
						X_ATAQUE -= variacaoAtq;
						al_draw_filled_circle (X_ATAQUE, (SCREEN_H/2), 10, al_map_rgb(225,32,65));
					}
					else 
					{
						X_ATAQUE = 720;
						ataca_agora = 0;
						
						TiraVidaMonstro(monstros, 10);
						
						if (vaiTerContraAtaque == 1)
						{
							recebe_ataque = 1;
						}
						else if (vaiTerContraAtaque == 0)
						{
							recebe_ataque = 0;
							vaiTerContraAtaque = 1;
						}
					}
				}
				
				if (ataca_especial == 1)
				{
					if (X_ATAQUE > (SCREEN_W/4))
					{
						X_ATAQUE -= variacaoAtq;
						al_draw_filled_circle (X_ATAQUE, (SCREEN_H/2), r_esp, al_map_rgb(225,32,65));
					}
					else 
					{
						X_ATAQUE = 720;
						ataca_especial= 0;
						
						TiraVidaMonstro(monstros, r_esp);
						
						if (vaiTerContraAtaque == 1)
						{
							recebe_ataque = 1;
						}
						else if (vaiTerContraAtaque == 0)
						{
							recebe_ataque = 0;
							vaiTerContraAtaque = 1;
						}
					}
				}
				
				if (recebe_ataque == 1)
				{	
						r_contraAtaque = randInt ((r_monstro-2), (r_monstro+2));
						
						
						if (X_RECEBE_ATQ < X_ATAQUE)
						{
							X_RECEBE_ATQ += variacaoAtq;
							al_draw_filled_circle (X_RECEBE_ATQ, (SCREEN_H/2), r_monstro, al_map_rgb(225,32,65));
						}
						else 
						{							
							X_ATAQUE = 720;
							X_RECEBE_ATQ = 240;
							recebe_ataque = 0;
								
							TiraVidaPersonagem(monstros, r_contraAtaque);
						}
				}	
			}

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
			if (modo_jogo == NAVEGACAO)
			{
				processaTeclaNaveg (&heroi, ev.keyboard.keycode);
			}
			else 
			{
				processaTeclaBatalha (&heroi, ev.keyboard.keycode);
			}
			
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
		}

	} //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
 
	return 0;
}
