#include "function.h"

int calcular_formantes(FILE * archivo){
    if(archivo == NULL){return -1;}

    float * f1;
    float * f2;
    float alfa = 0.95;
    short * muestras_ventana;
    short * x;
    short * xp;
    int m_auxiliar;
    int num_marcos = 0;
    int j = 0;
    int k = 0;
    int w = 0;
    int contador = 0;
    int suficiente = 0;
    float f1_promedio = 0, f2_promedio = 0;
    float * distancias = NULL;

    iniciar_cabecera(&archivo);
    ajustar_archivo(&archivo);
    initLista(&listaVocales);

    MUESTRAS = (MILISEGUNDOS * sampleRate) / 1000;
    DESPLAZAMIENTO = (DESPLAZAMIENTOTIEMPO * sampleRate) / 1000;

    calcular_numero_muestras();
    ajustar_exponente();

    m_auxiliar = MUESTRAS;
    x = (short*)malloc(numSamples*sizeof(short));
    xp = (short*)malloc(numSamples*sizeof(short));
    distancias = (float*)malloc(5*sizeof(float));
    muestras_ventana = (short*)malloc(MUESTRAS*sizeof(short));
    contadorVocales = (int*)malloc(5*sizeof(int));
    contadorTempVocales = (int*)malloc(5*sizeof(int));
    num_marcos = (((1000 * numSamples) / sampleRate) / 10) - 2;

    for(int i = 0; i < numSamples; ++i){
        fread(&(x[i]), 1, 2, archivo);
    }

    contadorVocales[0] = 0;
    contadorVocales[1] = 0;
    contadorVocales[2] = 0;
    contadorVocales[3] = 0;
    contadorVocales[4] = 0;
    contadorTempVocales[0] = 0;
    contadorTempVocales[1] = 0;
    contadorTempVocales[2] = 0;
    contadorTempVocales[3] = 0;
    contadorTempVocales[4] = 0;
    vocal_temporal = -1;

    f1 = (float*)malloc(MINMARCOS*sizeof(float));
    f2 = (float*)malloc(MINMARCOS*sizeof(float));

    m_auxiliar = -m_auxiliar / 2;
    for (int i = 0; i < numSamples; i++) {
        if((i-1) < 0){
            xp[i] = x[i] - alfa*x[i-1];
        }
        else{
            xp[i] = x[i];
        }
    }
    free(x);
    //num_marcos = 1;
    printf("NUMERO MUESTRAS: %d\n", numSamples);
    printf("MARCOS: %d\n", num_marcos);
    for (int i = 0; i < num_marcos; i++) {
        for (j=k, w=0; j < (MUESTRAS + k); j++) {
            muestras_ventana[w] = xp[j] * (0.54 + 0.46*cos(2*PI*(m_auxiliar + w)/MUESTRAS));
            w++;
        }

        k += DESPLAZAMIENTO;
        if(contador == 0){
            f1_promedio = 0;
            f2_promedio = 0;
            for (int m = MINMARCOS - 1; m >= 0; m--) {
                f1_promedio += f1[m];
                f2_promedio += f2[m];
                f1[m] = f1[m - 1];
                f2[m] = f2[m - 1];
            }
            f1_promedio /= MINMARCOS;
            f2_promedio /= MINMARCOS;
            reconocer(f1_promedio, f2_promedio, distancias);
            suficiente = calcular_fft(-1, muestras_ventana, &(f1[0]), &(f2[0]));
            if(suficiente == -3){
                contador = MINMARCOS-1;
            }
        }
        else{
            suficiente = calcular_fft(-1, muestras_ventana, &(f1[contador]), &(f2[contador]));
            reconocer(6000, 6000, distancias);
            contador--;
            if(suficiente == -3)
                contador = MINMARCOS-1;
        }
    }

    decidir();

    return 0;
}

void decidir(){
    int aux_vocales = 0;
    int mayor1 = 0;
    int mayor2 = 0;
    int i1 = 0;
    int i2 = 0;
    int igualdad = 0;
    /*for (int i = 0; i < 5; i++) {
        aux_vocales = contadorVocales[i];
        if(aux_vocales >= mayor1){
            i2 = i1;
            mayor2 = mayor1;
            i1 = i;
            mayor1 = aux_vocales;
        }
        else{
            if(aux_vocales > mayor2){
                i2 = i;
                mayor2 = aux_vocales;
            }
        }
    }
    printf("ESTA ES LA I1: %d\n", i1);
    printf("ESTA ES LA I2: %d\n", i2);
    if(i1 == i2){igualdad = 1; printf("%s\n", "HUBO IGUALDAD");}*/
    //ahora las variables mayor, se vuelven un contador de el numero
    //con las i sabemos las vocales y con estas nuevas variables sabemos en que lugar ocurrieron:
    int primeraVocal = -1;
    int segundaVocal = -1;
    mayor1 = 0; mayor2 = 0;
    struct nodo_lista * auxiliar = listaVocales.primero;
    while (auxiliar != NULL) {
        printf("%s %d\n", "Nodo: ", auxiliar -> vocal);
        if(primeraVocal == -1){
            primeraVocal = auxiliar -> vocal;
            mayor1 = auxiliar -> numero;
        }
        else if(auxiliar -> vocal == primeraVocal){
            if(segundaVocal == -1){
                segundaVocal = auxiliar -> vocal;
                mayor2 = auxiliar -> numero;
            }
            else{
                if(auxiliar -> numero > mayor1){
                    primeraVocal = segundaVocal;
                    mayor1 = mayor2;
                    segundaVocal = auxiliar -> vocal;
                    mayor2 = auxiliar -> numero;
                }
            }
        }
        else{
            if(segundaVocal == -1){
                segundaVocal = auxiliar -> vocal;
                mayor2 = auxiliar -> numero;
            }
            else if(segundaVocal == auxiliar -> vocal){
                if(auxiliar -> numero > mayor2){
                    segundaVocal = auxiliar -> vocal;
                    mayor2 = auxiliar -> numero;
                }
            }
            else{
                if(auxiliar -> numero > mayor2){
                    if(mayor2 > mayor1){
                        primeraVocal = segundaVocal;
                        mayor1 = mayor2;
                        segundaVocal = auxiliar -> vocal;
                        mayor2 = auxiliar -> numero;
                    }
                    else{
                        segundaVocal = auxiliar -> vocal;
                        mayor2 = auxiliar -> numero;
                    }
                }
                else if(auxiliar -> numero > mayor1){
                    primeraVocal = segundaVocal;
                    mayor1 = mayor2;
                    segundaVocal = auxiliar -> vocal;
                    mayor2 = auxiliar -> numero;
                }
            }
        }
        auxiliar  = auxiliar -> siguiente;
    }
    //printf("%d\n", i1);
    /*while (auxiliar != NULL) {
        printf("NODO: vocal %d, numero %d\n", auxiliar -> vocal, auxiliar -> numero);
        if(auxiliar -> vocal == i1 || auxiliar -> vocal == i2){
            if(primeraVocal == -1){
                primeraVocal = auxiliar -> vocal;
                mayor1 = auxiliar -> numero;
            }
            else if(primeraVocal == auxiliar -> vocal){
                if(mayor1 < auxiliar -> numero){
                    if(segundaVocal == -1){
                        if(igualdad == 1){
                            segundaVocal = auxiliar -> vocal;
                            mayor2 = auxiliar -> numero;
                        }
                        else{
                            primeraVocal = auxiliar -> vocal;
                            mayor1 = auxiliar -> numero;
                        }
                    }
                    else{
                        primeraVocal = segundaVocal;
                        mayor1 = mayor2;
                        segundaVocal = auxiliar -> vocal;
                        mayor2  = auxiliar -> numero;
                    }
                }
            }
            else{
                if(segundaVocal == -1){
                    segundaVocal = auxiliar -> vocal;
                    mayor2 = auxiliar -> numero;
                }
                else if(segundaVocal == auxiliar -> vocal){
                    if(mayor2 < auxiliar -> numero){
                        segundaVocal = auxiliar -> vocal;
                        mayor2 = auxiliar -> numero;
                    }
                }
            }
        }
        auxiliar = auxiliar -> siguiente;
    }*/
    printf("Las vocales son: %c, %c\n", retornar_vocal(primeraVocal), retornar_vocal(segundaVocal));
}

char retornar_vocal(int v){
    if(v == 0){
        return 'A';
    }
    else if(v == 1){
        return 'E';
    }
    else if(v == 2){
        return 'I';
    }
    else if(v == 3){
        return 'O';
    }
    else if(v == 4){
        return 'U';
    }
    else{
        return '-';
    }
}

void reconocer(float formante1, float formante2, float * distancias){
    distancias[0] = sqrt(pow(F1A - formante1,2) + pow(F2A - formante2,2));
    distancias[1] = sqrt(pow(F1E - formante1,2) + pow(F2E - formante2,2));
    distancias[2] = sqrt(pow(F1I - formante1,2) + pow(F2I - formante2,2));
    distancias[3] = sqrt(pow(F1O - formante1,2) + pow(F2O - formante2,2));
    distancias[4] = sqrt(pow(F1U - formante1,2) + pow(F2U - formante2,2));
    float aux = distancias[0];
    int contador_aux = 0;

    for (int i = 1; i < 5; i++) {
        if(aux > distancias[i]){
            aux = distancias[i];
            contador_aux = i;
        }
    }
    if(distancias[contador_aux] < 800){
        /*
        if(vocal_temporal != contador_aux){
            agregarElemento(&listaVocales, contador_aux);
            vocal_temporal = contador_aux;
        }
        else{
            listaVocales.ultimo -> numero += 1;
        }*/
        if(contador_aux == 0){
            contadorVocales[0] += 1;
            contadorTempVocales[0] += 1;
            printf("%s", "ES UNA:  A");
            //printf("%f\n", formante1);
        }
        else if(contador_aux == 1){
            contadorVocales[1] += 1;
            contadorTempVocales[1] += 1;
            printf("%s", "ES UNA:  E");
            //printf("%f\n", formante1);
        }
        else if(contador_aux == 2){
            contadorVocales[2] += 1;
            contadorTempVocales[2] += 1;
            printf("%s", "ES UNA:  I");
            //printf("%f\n", formante1);
        }
        else if(contador_aux == 3){
            contadorVocales[3] += 1;
            contadorTempVocales[3] += 1;
            printf("%s", "ES UNA:  O");
            //printf("%f\n", formante1);
        }
        else if(contador_aux == 4){
            contadorVocales[4] += 1;
            contadorTempVocales[4] += 1;
            printf("%s", "ES UNA:  U");
            //printf("%f\n", formante1);
        }
    }
    else{
        printf("%s\n", "--------");
        if(contadorTempVocales[0] != 0||contadorTempVocales[1] != 0||contadorTempVocales[2] != 0||contadorTempVocales[3] != 0||contadorTempVocales[4] != 0){
            int mas_grande = 0;
            int i1 = 0;
            for (int i = 0; i < 5; i++) {
                if(mas_grande < contadorTempVocales[i]){
                    mas_grande = contadorTempVocales[i];
                    i1 = i;
                }
            }
            agregarElemento(&listaVocales, i1);
            listaVocales.ultimo -> numero = contadorTempVocales[i1];
            contadorTempVocales[0] = 0;
            contadorTempVocales[1] = 0;
            contadorTempVocales[2] = 0;
            contadorTempVocales[3] = 0;
            contadorTempVocales[4] = 0;
        }
    }
    printf("    %f, %f,  distancia: %f\n", formante1, formante2, distancias[contador_aux]);
    //printf("%f\n", formante1);
}

int calcular_fft(int signo, short * muestras, float * f1, float * f2){
    ajustar_exponente();

    float * real = (float *)malloc(potencias_dos_resultado * sizeof(float));
    float * imaginaria = (float *)malloc(potencias_dos_resultado * sizeof(float));
    float maxValor = 32768;

    int f1_temporal = 0; // son los picos
    int f2_temporal = 0; // son los picos
    int i1_temporal = 0;
    int i2_temporal = 0;
    float aux_formante = 0;
    float aux_formante2 = 0;
    int auxiliar_k = 0;
    float aux_total = 0;
    int divisor = 2;
    int contador = 0;
    int factor_k = 0;
    int contador_exponente = exponente;
    float temp_real = 0;
    float temp_img = 0;

    for (int i = 0; i < potencias_dos_resultado; i++) {
        if(i < MUESTRAS){
            real[i] = (muestras[i] / maxValor);
        }
        else{
            real[i] = 0;
        }
        imaginaria[i] = 0;
    }

    reordenar_muestras(&real, &imaginaria);

    for (int i = 0; i < exponente; i++) {
        for(int j = 0; j < (int)pow(2,contador_exponente-1); j++){
            auxiliar_k = (int)pow(2,i);
            for(int k = 0; k < auxiliar_k; k++){
                factor_k = k * (int)pow(2, contador_exponente - 1);

                temp_real  = real[contador + auxiliar_k];
                real[contador + auxiliar_k] =( (real[contador + auxiliar_k] * cos(signo*2*PI*factor_k / potencias_dos_resultado)) - (imaginaria[contador + auxiliar_k] * sin(signo*2*PI*factor_k / potencias_dos_resultado)) );
                imaginaria[contador + auxiliar_k] = ( (imaginaria[contador + auxiliar_k] * cos(signo*2*PI*factor_k / potencias_dos_resultado)) + (temp_real* sin(signo*2*PI*factor_k / potencias_dos_resultado)) );

                temp_real = (real[contador] + real[contador + auxiliar_k]) / divisor;
                temp_img = (imaginaria[contador] + imaginaria[contador + auxiliar_k]) / divisor;

                real[contador + auxiliar_k] = (real[contador] -real[contador + auxiliar_k]) / divisor;
                imaginaria[contador + auxiliar_k] = (imaginaria[contador] - imaginaria[contador + auxiliar_k]) / divisor;

                real[contador] = temp_real;
                imaginaria[contador] = temp_img;

                contador++;
            }
            contador += auxiliar_k;
        }
        contador = 0;
        contador_exponente -= 1;
    }

    //FILE * nuevo_archivo = fopen("furiersazo.wav", "wb");
    //iniciar_cabecera_nuevo_archivo(&nuevo_archivo);
    //printf("%s\n", "==============================================");
    /*for (int i = 0; i < potencias_dos_resultado; i++) {
        aux_total = sqrt(pow(real[i] * maxValor, 2) + pow(imaginaria[i] * maxValor, 2));
        aux_total *= 2;
        printf("%f\n", real[2]);
        fwrite(&aux_total, 2, 1, nuevo_archivo);
    }*/
    for (int i = potencias_dos_resultado-1; i > (900 * potencias_dos_resultado) / sampleRate; i--) {
        aux_total = sqrt(pow(real[i] * maxValor, 2) + pow(imaginaria[i] * maxValor, 2));
        if(aux_formante < aux_total){
            aux_formante = aux_total;
        }
        else{
            if(aux_formante > f1_temporal){
                f1_temporal = aux_formante;
                i1_temporal = potencias_dos_resultado - 1 - i;
                aux_formante = 0;
            }
        }
    }

    aux_formante = 0;
    for (int i = potencias_dos_resultado - ((900 * potencias_dos_resultado) / sampleRate); i > potencias_dos_resultado / 2 ; i--) {
        aux_total = sqrt(pow(real[i] * maxValor, 2) + pow(imaginaria[i] * maxValor, 2));
        if(aux_formante < aux_total){
            aux_formante = aux_total;
        }
        else{
            if(aux_formante > f2_temporal){
                f2_temporal = aux_formante;
                i2_temporal = potencias_dos_resultado - 1 - i;
                aux_formante = 0;
            }
        }
    }


    //printf("Las I son: %d, %d\n", i1_temporal, i2_temporal);
    *f1 = (float)(i1_temporal * sampleRate) / potencias_dos_resultado;
    *f2 = (float)(i2_temporal * sampleRate) / potencias_dos_resultado;

    free(real);
    free(imaginaria);

    if((f1_temporal < 20) || (f2_temporal < 20)){
        return -3;
    }

    return 0;
}

short absoluto(short x){
    if(x < 0){
        x = x * (-1);
    }
    return x;
}

void ajustar_exponente(){
    exponente = 0;
    potencias_dos_resultado = 0;
    exponente = ceil(log(MUESTRAS)/log(2));
    potencias_dos_resultado = pow(2, exponente);
}

void reordenar_muestras(float ** x, float ** y){
    if(x == NULL){return;}
    int j = 0;
    int aux = 0;
    for(int i = 0; i < potencias_dos_resultado; i++){
        if(j > i){
            intercambiar(x, i, j);
            intercambiar(y, i, j);
        }
        aux = potencias_dos_resultado / 2;
        while(aux >= 2 && j >= aux){
            j -= aux;
            aux >>= 1;
        }
        j += aux;
    }
}

void intercambiar(float ** x, int a, int b){
    if(x == NULL){return;}
    float temp = 0;
    temp = *(x[0] + a);
    *(x[0] + a) = *(x[0] + b);
    *(x[0] + b) = temp;
}

void calcular_numero_muestras(){
    numSamples = (8 * subChunk2Size) / (numChannels * bitsPerSample);
}

void iniciar_cabecera(FILE ** archivo){
    if(archivo == NULL){return;}
    fread(&chunkID, 1, 4, *archivo);
    fread(&chunkSize, 1, 4, *archivo);
    fread(&format, 1, 4, *archivo);
    fread(&subChunk1ID, 1, 4, *archivo);
    fread(&subChunk1Size, 1, 4, *archivo);
    fread(&audioFormat, 1, 2, *archivo);
    fread(&numChannels, 1, 2, *archivo);
    fread(&sampleRate, 1, 4, *archivo);
    fread(&byteRate, 1, 4, *archivo);
    fread(&blockAlign, 1, 2, *archivo);
    fread(&bitsPerSample, 1, 2, *archivo);
    fread(&subChunk2ID, 1, 4, *archivo);
    fread(&subChunk2Size, 1, 4, *archivo);
}

void imprimir_cabecera(){
    printf("chunkID %x\n", chunkID);
    printf("chunkSize %x\n", chunkSize);
    printf("format %x\n", format);
    printf("subChunk1ID %x\n", subChunk1ID);
    printf("subChunk1Size %x\n", subChunk1Size);
    printf("audioFormat %x\n", audioFormat);
    printf("numChannels %x\n", numChannels);
    printf("sampleRate %x\n", sampleRate);
    printf("byteRate %x\n", byteRate);
    printf("blockAlign %x\n", blockAlign);
    printf("bitsPerSample %x\n", bitsPerSample);
    printf("subChunk2ID %x\n", subChunk2ID);
    printf("subChunk2Size %x\n", subChunk2Size);
}

void ajustar_archivo(FILE ** archivo){
    if(archivo == NULL){return;}

    if(subChunk2ID == 1414744396){
        listID = subChunk2ID;
        subChunkList = subChunk2Size;
        subChunk2ID = 0;
        subChunk2Size = 0;
        bandera = 1;

        informacion_extra = (char *)malloc(sizeof(char)*subChunkList*2);
        fread(informacion_extra, 1, subChunkList, *archivo);

        fread(&subChunk2ID, 1, 4, *archivo);
        fread(&subChunk2Size, 1, 4, *archivo);
    }
    else{
        bandera = 0;
    }
}

void iniciar_cabecera_nuevo_archivo(FILE ** archivo){
    if(archivo == NULL){return;}

    numChannels = 1;
    byteRate = (sampleRate * numChannels * bitsPerSample)/8;
    blockAlign = (numChannels * bitsPerSample)/8;
    subChunk2Size = (numSamples * numChannels * bitsPerSample)/8;
    chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);

    fwrite(&chunkID, 4, 1, *archivo);
    fwrite(&chunkSize, 4, 1, *archivo);
    fwrite(&format, 4, 1, *archivo);
    fwrite(&subChunk1ID, 4, 1, *archivo);
    fwrite(&subChunk1Size, 4, 1, *archivo);
    fwrite(&audioFormat, 2, 1, *archivo);
    fwrite(&numChannels, 2, 1, *archivo);
    fwrite(&sampleRate, 4, 1, *archivo);
    fwrite(&byteRate, 4, 1, *archivo);
    fwrite(&blockAlign, 2, 1, *archivo);
    fwrite(&bitsPerSample, 2, 1, *archivo);

    if(bandera == 1){
        fwrite(&listID, 4, 1, *archivo);
        fwrite(&subChunkList, 4, 1, *archivo);
        fwrite(informacion_extra, 1, subChunkList, *archivo);
    }

    fwrite(&subChunk2ID, 4, 1, *archivo);
    fwrite(&subChunk2Size, 4, 1, *archivo);
}

int initLista(struct lista * lista){
    if(lista == NULL){return -1;}

    lista -> primero = NULL;
    lista -> ultimo = NULL;

    return 0;
}

int agregarElemento(struct lista * lista, int vocal){
    if(lista == NULL){return -1;}

    struct nodo_lista * auxiliar = (struct nodo_lista *)malloc(sizeof(struct nodo_lista));
    if(auxiliar == NULL){return -2;}

    auxiliar -> vocal = vocal;
    auxiliar -> numero = 1;
    auxiliar -> siguiente = NULL;

    if(lista -> primero == NULL){
        lista -> primero = auxiliar;
    }
    else{
        lista -> ultimo -> siguiente = auxiliar;
    }
    lista -> ultimo = auxiliar;

    return 0;
}
