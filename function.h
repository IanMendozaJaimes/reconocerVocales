#ifndef __FUNCIONES_H__
#define __FUNCIONES_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <string.h>
    #include <time.h>

    #define E 2.718281828459
    #define PI 3.1415926535
    //40ms de 8000 muestras
    #define MILISEGUNDOS 30
    #define DESPLAZAMIENTOTIEMPO 10
    #define MINMARCOS 10

    #define F1A 650
    #define F2A 1350

    #define F1E 367
    #define F2E 2150

    #define F1I 329.3563433
    #define F2I 2345

    #define F1O 450
    #define F2O 1040

    #define F1U 325
    #define F2U 1000

    struct nodo_lista{
        int vocal;
        int numero;
        struct nodo_lista * siguiente;
    };

    struct lista{
        struct nodo_lista * primero;
        struct nodo_lista * ultimo;
    };

    struct lista listaVocales;

    int MUESTRAS;
    int  DESPLAZAMIENTO;

    unsigned int chunkID;
    unsigned int chunkSize;
    unsigned int format;
    unsigned int subChunk1ID;
    unsigned int subChunk1Size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    unsigned int subChunk2ID;
    unsigned int subChunk2Size;
    unsigned int listID;
    unsigned int subChunkList;
    unsigned int numSamples;
    int bandera;
    char * informacion_extra;
    unsigned long potencias_dos_resultado;
    int exponente;
    int * contadorVocales;
    int * contadorTempVocales;
    int vocal_temporal;



    int calcular_fft(int, short *, float *, float *);
    short absoluto(short);
    void reordenar_muestras(float **, float **);
    void intercambiar(float **, int, int);
    void ajustar_exponente();
    int calcular_formantes(FILE *);
    void reconocer(float, float, float *);

    void iniciar_cabecera(FILE **);
    void imprimir_cabecera();
    void ajustar_archivo(FILE **);
    void iniciar_cabecera_nuevo_archivo(FILE **);
    void calcular_numero_muestras();
    char retornar_vocal(int);
    void decidir();
    int initLista(struct lista *);
    int agregarElemento(struct lista *, int);

#endif
