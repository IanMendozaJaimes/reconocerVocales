#include "function.h"

int main(int num_args, char ** args){
    clock_t inicio, final;
    double total;

    inicio = clock();

    FILE * archivo = fopen(args[1], "rb");
    if(archivo == NULL){
        printf("%s\n", "Hubo un problema con la lectura del archivo.");
        exit(2);
    }
    
    calcular_formantes(archivo);

    final = clock();
    total = (double)(final - inicio) / CLOCKS_PER_SEC;
    printf("Tiempo empleado: %f\n", total);

    return 0;
}
