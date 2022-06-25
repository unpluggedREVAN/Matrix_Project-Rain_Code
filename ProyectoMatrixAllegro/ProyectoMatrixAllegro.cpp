// Tecnologico de Costa Rica 
// Estructuras de Datos Gr02 
// Proyecto Matrix
// Jose Pablo Aguero Mora 2021126372 
// Katerine Guzman Flores 2019390523

// Se incluyen diversas
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <random> 
#include <string>
#pragma comment(lib, "User32.lib")

// Se incluye las librerías de allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#define FPS 30.0 // indica los frames por segundo

using namespace std;


//Elementos de allegro que se utilizarán para el juego
//**********************************************************
ALLEGRO_DISPLAY* pantalla;
ALLEGRO_FONT* fuente;


// **************************************** Variables Globales *********************************************
// posiciones iniciales, cantidad de pistas totales, y cantidad de Letras de cada hilera como variables globales
int xInicial = 0;
int yInicial = 10;
int cantidadLetras = 0;
int pistasI = 35;
int cantidadAgrupaciones = 0;


// Se usa un struct de tipo hilera
typedef struct Hilera {
	int X;
	int Y;
	char caracter;
	char stack[12]; // arreglo de caracteres que van cayendo
	int pistas = 1;
	Hilera* Siguiente; // Puntero que apunta a la siguiente hilera
}*Ptr_Hilera;


// Recibe la lista de hileras como parametro por referencia que tiene la forma tipo Ptr_Hilera
void InicializarHilera(Ptr_Hilera& Lista)
{
	Lista = NULL;
}

// Funciona para generar caracteres ascii random
long g_seed = 1; // g_seed se va modificando cada vez para que realmente muestre caracteres aleatorios ya que es la semilla del random
inline int fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}

char GenerarRandom()
{
	cantidadLetras += 1;
	char c = 'A' + fastrand() % 24;
	return c;
}

// Se crea una hilera
Ptr_Hilera CrearAgrupacion(int cod)
{
	Ptr_Hilera Agrupacion = new(Hilera);

	xInicial += 20;
	Agrupacion->X = xInicial;
	Agrupacion->Y = yInicial; // siempre se inician en esta posicion porque es el inicio de la pantalla
	Agrupacion->caracter = GenerarRandom(); // se le asigna caracter aleatorio
	Agrupacion->Siguiente = NULL;
	return Agrupacion;
}

// Se van agregando las hileras a la lista
void AgregarFinal(Ptr_Hilera& Lista, Ptr_Hilera& Nuevo)
{
	Ptr_Hilera Aux;
	Aux = Lista;
	if (Aux != NULL) // Si hay al emnos un elemento
	{
		while (Aux->Siguiente != NULL) // si es el ultimo nodo
		{
			Aux = Aux->Siguiente; // Se le asigna el siguiente nodo
		}
		Aux->Siguiente = Nuevo;  // si ya llegamos a null llegamos al ultimo elemento y se engancha al final
	}
	else
	{
		Lista = Nuevo; // Se agrega nuevo al final
	}
}

// Va matando el primer elemento de la lista
void DestruirAgrupaciones(Ptr_Hilera& Lista)
{
	Ptr_Hilera Aux;
	Aux = Lista; // el auxiliar apunta al inicio de la lista
	while (Aux != NULL) // mientras el auxiliar apunte a alguien
	{
		Lista = Lista->Siguiente;  // a lista lo ponemos a apuntar al segundo elemento
		delete(Aux); // ya que lista apunta al segundo podemos matar el primero
		Aux = Lista; // se limpia la referencia que tenia aux y lo dejamos apuntando al segundo
	}
}

// Sirve para ir editando el archivo con la información de las pistas e hileras
void EditarArchivo(string cod)
{
	FILE* archivo;
	fopen_s(&archivo, "stats.txt", "a"); // crea el archivo de forma que se pueda editar

	if (NULL == archivo) { // si el archivo es NULL entonces hubo problemas al crearlo genera el siguiente mensaje:.
		fprintf(stderr, "No se pudo crear archivo %s.\n", "resultados.txt");
		exit(-1);
	}
	else {
		//Si se pude crear el archivo, entonces:
		fprintf(archivo, "%s\n", cod);
	}
	fclose(archivo); //cierra el archivo.

}

void ListarPistas(Ptr_Hilera& Lista)
{
	int Contador = 1;
	Ptr_Hilera Aux;
	Aux = Lista;
	string oracion; // Se guarda en esta variable el texto que se va guardando en el artículo
	while (Aux != NULL)
	{
		oracion = "Pista " + to_string(Contador); // Indica el numero de la pista
		oracion += " = Cantidad de agrupaciones: " + to_string(Aux->pistas); // Indica la cantidad de hileras por pista
		oracion += "\n";
		EditarArchivo(oracion); // Se incluye la información anterior al archivo

		// Se imprime la misma informacoón en pantalla
		printf("Pista %d ", Contador);
		printf("= Cantidad de agrupaciones: %d ", Aux->pistas);
		cout << endl;
		Aux = Aux->Siguiente;
		Contador++;
	}
}

/* Se usa un stack que es un arreglo en forma de pila con longitud definida. Por este motivo es necesario una función que
sobre escriba las posiciones para que logre un efecto de movimiento a la derecha. */
void desplazar(char stack[12]) {
	stack[11] = stack[10];
	stack[10] = stack[9];
	stack[9] = stack[8];
	stack[8] = stack[7];
	stack[7] = stack[6];
	stack[6] = stack[5];
	stack[5] = stack[4];
	stack[4] = stack[3];
	stack[3] = stack[2];
	stack[2] = stack[1];
	stack[1] = stack[0];
}

/* Esta función en cada ciclo imprime el contenido de cada posición del stack con diferentes propíedades de colores, pero
cuidando las coordenadas de los caracteres anteriores para lograr el efecto de huella. */
char actual[1];
void dibujar(Ptr_Hilera& Agrupaciones) {
	actual[0] = Agrupaciones->stack[0];
	al_draw_text(fuente, al_map_rgb(255, 255, 255), Agrupaciones->X, Agrupaciones->Y - 20, ALLEGRO_ALIGN_CENTRE, actual); // Se dibuja en color blanco

	actual[0] = Agrupaciones->stack[1];
	al_draw_text(fuente, al_map_rgb(0, 200, 0), Agrupaciones->X, Agrupaciones->Y - 40, ALLEGRO_ALIGN_CENTRE, actual);
	actual[0] = Agrupaciones->stack[2];
	al_draw_text(fuente, al_map_rgb(0, 200, 0), Agrupaciones->X, Agrupaciones->Y - 60, ALLEGRO_ALIGN_CENTRE, actual);

	actual[0] = Agrupaciones->stack[3];
	al_draw_text(fuente, al_map_rgb(0, 150, 0), Agrupaciones->X, Agrupaciones->Y - 80, ALLEGRO_ALIGN_CENTRE, actual);
	actual[0] = Agrupaciones->stack[4];
	al_draw_text(fuente, al_map_rgb(0, 150, 0), Agrupaciones->X, Agrupaciones->Y - 100, ALLEGRO_ALIGN_CENTRE, actual);

	actual[0] = Agrupaciones->stack[5];
	al_draw_text(fuente, al_map_rgb(0, 100, 0), Agrupaciones->X, Agrupaciones->Y - 120, ALLEGRO_ALIGN_CENTRE, actual);
	actual[0] = Agrupaciones->stack[6];
	al_draw_text(fuente, al_map_rgb(0, 100, 0), Agrupaciones->X, Agrupaciones->Y - 140, ALLEGRO_ALIGN_CENTRE, actual);

	actual[0] = Agrupaciones->stack[7];
	al_draw_text(fuente, al_map_rgb(0, 50, 0), Agrupaciones->X, Agrupaciones->Y - 160, ALLEGRO_ALIGN_CENTRE, actual);
	actual[0] = Agrupaciones->stack[8];
	al_draw_text(fuente, al_map_rgb(0, 50, 0), Agrupaciones->X, Agrupaciones->Y - 180, ALLEGRO_ALIGN_CENTRE, actual);

	actual[0] = Agrupaciones->stack[9];
	al_draw_text(fuente, al_map_rgb(0, 25, 0), Agrupaciones->X, Agrupaciones->Y - 200, ALLEGRO_ALIGN_CENTRE, actual);
	actual[0] = Agrupaciones->stack[10];
	al_draw_text(fuente, al_map_rgb(0, 25, 0), Agrupaciones->X, Agrupaciones->Y - 220, ALLEGRO_ALIGN_CENTRE, actual);

	actual[0] = Agrupaciones->stack[11];
	al_draw_text(fuente, al_map_rgba(0, 0, 0, 255), Agrupaciones->X, Agrupaciones->Y - 240, ALLEGRO_ALIGN_CENTRE, actual); // Se pinta de color negro
}

// función para sacar una hilera random / un número aleatorio
int crearRandom(int n)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, n);
	return dis(gen);
}

// Me devuelve una hilera random
Ptr_Hilera AgruAleatoria(Ptr_Hilera& Lista)
{
	// se recorre la lista para saber cuantas hileras en total hay
	int contador = 0;
	Ptr_Hilera Aux;
	Aux = Lista;
	while (Aux != NULL)
	{
		contador += 1;
		Aux = Aux->Siguiente;
	}

	// Me devuelve la hilera con el numero ramdon
	int ra = crearRandom(contador);
	Ptr_Hilera Aux2;
	Aux2 = Lista;
	int cont2 = 1;
	while (cont2 < ra)
	{
		cont2 += 1;
		Aux2 = Aux2->Siguiente;
	}
	return Aux2;
}

void CrearArchivo()
{
	FILE* archivo;
	fopen_s(&archivo, "Stats.txt", "w"); //crea el archivo

	if (NULL == archivo) { // si el archivo es NULL entonces hubo problemas al crearlo genera el siguiente mensaje:.
		fprintf(stderr, "No se pudo crear archivo %s.\n", "resultados.txt");
		exit(-1);
	}
	fclose(archivo); //cierra el archivo.

}

// programa principal
int main() {
	if (!al_init()) {
		fprintf(stderr, "No se puede iniciar allegro!\n");
		return -1;
	}

	// Esta línea de código permite que la ventana tenga la capacidad de cambiar de tamaño
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	// Se establecen las dimensiones d la pantalla y un título
	pantalla = al_create_display(720, 480);
	al_set_window_position(pantalla, 200, 200);
	al_set_window_title(pantalla, "MATRIX ALLEGRO");
	if (!pantalla) {
		fprintf(stderr, "No se puede crear la pantalla!\n");
		return -1;
	}
	//Líneas para obtener las funcionalidades del uso de las fuentes
	al_init_font_addon();
	al_init_ttf_addon();

	//Líneas para obtener las funcionalidades de los audios
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);

	fuente = al_load_font("squid.ttf", 16, NULL); // Se especifica la fuente de los caracteres en la simulación

	//Timers que se necesitarán para el juego
	//**********************************************************
	ALLEGRO_TIMER* primerTimer = al_create_timer(0.1 / FPS); //
	ALLEGRO_TIMER* segundoTimer = al_create_timer(0.4 / FPS); //
	//**********************************************************

	//Se crea una cola de eventos
	ALLEGRO_EVENT_QUEUE* colaEventos = al_create_event_queue();

	//Registro de los eventos
	//**********************************************************
	al_register_event_source(colaEventos, al_get_timer_event_source(primerTimer));
	al_register_event_source(colaEventos, al_get_timer_event_source(segundoTimer));
	//**********************************************************

	al_reserve_samples(2); // Sonido de allegro / Matrix
	ALLEGRO_SAMPLE* song = al_load_sample("matrix_sound.wav");
	al_play_sample(song, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

	//Inicialización de los timer
	//**********************************************************
	al_start_timer(primerTimer);
	al_start_timer(segundoTimer);
	//**********************************************************
	bool hecho = true; // variable para el ciclo infinito / cuando esta es false se acaba la simulación
	ALLEGRO_KEYBOARD_STATE estadoTeclado;

	time_t inicio, fin; // para llevar estadisticas de ejecucion
	inicio = time(NULL);

	Ptr_Hilera Agrupaciones; // Lista enlazada
	Ptr_Hilera Nuevo; // Para agregar nuevos nodos a la lista
	InicializarHilera(Agrupaciones);

	for (int i = 0; i <= 35; i++) // Se agregan 36 nodos / estas serán las pistas de la simulación
	{
		Nuevo = CrearAgrupacion(i);
		AgregarFinal(Agrupaciones, Nuevo);
	}

	Ptr_Hilera Aux;
	Ptr_Hilera elemA;
	Aux = Agrupaciones; // Agrupaciones == Lista
	while (hecho) { // inicia el ciclo infinito
		Aux = Agrupaciones;
		ALLEGRO_EVENT eventos;

		al_wait_for_event(colaEventos, &eventos);

		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == primerTimer) { // Condicional entra en el primer timer

				if (GetAsyncKeyState(VK_ESCAPE)) { // Código para parar la simulación cuando se presiona la tecla escape
					hecho = false;
				}

				elemA = AgruAleatoria(Agrupaciones); // Esta sección actualiza los datos de un nodo random de la lista en cada ciclo
				elemA->caracter = GenerarRandom();
				desplazar(elemA->stack);
				elemA->stack[0] = elemA->caracter;
				elemA->Y = elemA->Y + 20;
			}

			if (eventos.timer.source == segundoTimer) { // Condicional entra en el segundo timer
				al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpia los residuos de la pantalla en cada ciclo
				while (Aux != NULL)
				{

					dibujar(Aux); // Se dibujan todas las agrupaciones en cada ciclo
					if (Aux->Y >= 680) { // Si la hilera actual supera la coordenada y 680 entonces la devuelve al inicio de la pantalla
						Aux->Y = 10;
						Aux->pistas = Aux->pistas + 1;
						pistasI += 1;
					}
					Aux = Aux->Siguiente;
				}
				al_flip_display();
			}

		}
	}
	al_destroy_event_queue(colaEventos); // Se finalizan los servicios de allegro
	al_destroy_font(fuente);
	al_destroy_timer(primerTimer);
	al_destroy_display(pantalla);
	al_destroy_sample(song);

	fin = time(NULL); // Aquí termina de contabilizar el tiempo de ejecución

	cout << "\n-------------- Fin de la simulacion - Presione cualquier tecla para mostrar las estadisticas --------------\n" << endl;
	system("pause");
	system("CLS");

	CrearArchivo(); // Se genera el archivo Stats para proceder a llenarlo

	// Edición de archivo
	int tiempo = difftime(fin, inicio);
	string str1;
	str1 = "La ejecucion ha tardado : " + to_string(tiempo);
	str1 += " segundos\n";
	EditarArchivo(str1); // Se ingresan los datos de las estadísticas al archivo Stats

	str1 = "Cantidad de pistas: 36";
	str1 += "\n";
	EditarArchivo(str1);

	str1 = "Cantidad total de letras generadas: " + to_string(cantidadLetras);
	str1 += "\n";
	EditarArchivo(str1);

	str1 = "Cantidad total de agrupaciones generadas: " + to_string(pistasI + 1);
	str1 += "\n";
	EditarArchivo(str1);

	// Pintar estadisticas
	cout << "---------------------------- Estadisticas de ejecucion --------------------------------" << endl;
	printf("\nLa ejecucion ha tardado : %f segundos.\n", difftime(fin, inicio));
	cout << "Cantidad de pistas: " << 36 << "\n";
	cout << "Cantidad total de letras generadas: " << cantidadLetras << "\n";
	cout << "Cantidad total de agrupaciones generadas: " << pistasI + 1 << "\n";
	ListarPistas(Agrupaciones); // Muestra cantidad de agrupaciones por pista
	cout << "\n---------------------------- Fin - Estadisticas de ejecucion --------------------------------" << endl;

	DestruirAgrupaciones(Agrupaciones); // Libera la lista enlazada
	system("pause");

	cout << "\n------------- Aviso: Se ha generado un archivo (Stats) con las estadisticas de ejecucion -------------" << endl;
}
