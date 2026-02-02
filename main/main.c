#include <stdio.h>
#include "activacion_motores.h"

void app_main(void)
{

    semaforoMuestreo = xSemaphoreCreateBinary();
    printf("\n\nSEMAFORO CREADO: %p\n", semaforoMuestreo);
}