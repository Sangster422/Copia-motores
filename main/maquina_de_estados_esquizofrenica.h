/**
 * @file Maquina_estados_protesis.h
 * @brief Definición de la máquina de estados principal de la prótesis.
 * @details
 * La máquina de estados define los modos de funcionamiento de la prótesis y las fases asociadas a cada modo.
 * 
 * **Estados posibles:** (POR REVIDAR)
 * - @ref ESTADO_NORMAL: 
 *      - @ref REPOSO
 *      - @ref APERTURA
 *      - @ref CIERRE 
 * 
 * - @ref ESTADO_DESCANSO:
 *      - @ref PARADA 3 seg
 *      - @ref APERTURA 3 seg
 * 
 * - @ref ESTADO_SEGURIDAD:
 *      - @ref apertura de la mano   
 *      - @ref parada de motores
 *      - @ref cambio a @ref estado_normal
 * 
 * - @ref ESTADO_CALIBRADO_UMBRALES:
 *   - @ref FASE_1: Calibrar umbrales de activación.
 *   - @ref FASE_2: Esperar hasta que se suelte el botón (umbrales calculados).
 *   - @ref FASE_3: Calibrar umbrales de desactivación.
 * 
 * - @ref ESTADO_CALIBRADO_MOTORES:
 *   - @ref FASE_1: Motor en reposo.
 *   - @ref FASE_2: Motor abre la mano (manual).
 *   - @ref FASE_3: Motor cierra la mano (manual).
 *   - @ref FASE_4: Configurar posición 0 y cambiar a @ref ESTADO_NORMAL.
 * 
 * @note Las fases no son necesariamente secuenciales; su numeración es solo para organizar.
 */


/** Determinación nuevos criterios para las fases: (TEMPORALES)
 * 
 * FASE_INICIO << en esta se realiza el incio de los cuatro estados excepto el de seguridad
 * FASE_PASO_1 << en esta se realiza el primer paso, es decir, la primera acción de dicho estado
 * FASE_PASO_2 << en esta se realiza el segundo paso, es decir, la acción siguiente a la FASE_PASO 1
 * FASE_CAMBIO_ESTADO << en esta se realiza el cambio de estados tras cierta función/configuración/hecho.
 */

#pragma once

/**
 * @enum Estado_Protesis
 * @brief Posibles estados de la prótesis
 */
enum Estado_Protesis 
{
    ESTADO_NORMAL,             ///< Funcionamiento normal de la prótesis.
    ESTADO_DESCANSO,           ///< Modo para realizar una pausa controlada.
    ESTADO_SEGURIDAD,          ///< Modo de seguridad (secuencia: apertura y parada de motores)
    ESTADO_CALIBRADO_UMBRALES, ///< Modo de calibración de umbrales de activación/desactivación.
    ESTADO_CALIBRADO_MOTORES   ///< Modo de calibración de límites mecánicos del motor.
};

/**
 * @enum Fase_Estado
 * @brief Fases asociadas a cada estado de la prótesis.
 */
enum Fase_Estado{};

/**
 * @enum Causa_Seguridad
 * @brief Enumera las causas/condiciones por las cuales entraríamos en @ref ESTADO_SEGURIDAD.
 */
enum Causa_Seguridad 
{
    causas_ninguna = 0, 
    causa_sobrecorriente,     ///< sobrecorriente en motores.
    causa_sobrecalentameinto, ///<  niveles de temperatura demasiado elevados.
    causa_velocidad_excesiva, ///<  velocidad fuera de límites (ESTA NO SÉ AÚN SI SE QUEDARÁ O NO)
    causa_fuerza_excesiva,    ///< fuerza fuera de límites (ESTA NO SÉ AÚN SI SE QUEDARÁ O NO)
    causa_perdida_sensores,   ///< ha habido una perdida de señal de los sensores
    causa_error_encoder       ///< fallo en la lectura de la posición
};

/**
 * @struct Condiciones_Seguridad
 * @brief Estructura donde establecemos los parámetros a medir y sus valores máximos. Además, tenemos el monitoreo de dichos parámetros.
 */
struct Condiciones_Seguridad 
{
    float corriente_motores; ///< comprueba el valor de la corriente de los motores.
    float temperatura;       ///< comprueba los niveles de temperatura.
    float velocidad;         ///< comprueba el valor de la velocidad.
    float fuerza;            ///< comprueba el valor de la fuerza.
    float señal_sensores;    ///< ESTA VARIABLE TENDRÁ QUE SER UNA GLOBAL PARA OBTENER LOS VALORES DE LA SEÑAL DKT QUE SE OBTIENE EN MOTORES.
    float posicion;          ///< ESTA VARIABLE OBTIENE LAS VARIABLES DE MOTORES DE LAS FUNCIONES ESAS DE POSICIÓN (¿SÍ O NO?)

    // Estos son los valores máximos que tenemos que establecer (los umbrales): 
    const float UMBRAL_CORRIENTE = ;
    const float UMBRAL_TEMPERATURA = ;
    const float UMBRAL_VELOCIDAD = ;
    const float UMBRAL_FUERZA = ;

    /**
     * @brief Verifica si algún parámetro está en nivel crítico
     * @return Código de causa si hay problema, 0 si todo correcto.
     */
    Verificar_Parametros () // BUSCAR QUE TIPO DE FUNCIÓN HAY QUE PONER EN ESTE CASO
    {
        if(corriente_motores > UMBRAL_CORRIENTE) 
        {
            return causa_sobrecorriente;
        };

        if(temperatura > UMBRAL_TEMPERATURA) 
        {
            return causa_sobrecalentameinto;
        };

        if(velocidad > UMBRAL_VELOCIDAD) 
        {
            return causa_velocidad_excesiva;
        };

        if(fuerza > UMBRAL_FUERZA) 
        {
            return causa_fuerza_excesiva;
        };

        if (señal_sensores == 0) 
        {
            return causa_perdida_sensores;
        };

        if (posicion == 0)
        {
            return causa_error_encoder;
        };
        
        return causas_ninguna;

    };

    /**
    * @brief en esta vamos actualizando los valores del inicio.
    */
    void actualizacion (float corriente_actual, float temp_actual, float velocidad_actual, float fuerza_actual, float señal_actual, float posicion_actual) 
    {
        corriente_actual = corriente_motores; 
        temp_actual = temperatura;      
        velocidad_actual = velocidad;        
        fuerza_actual = fuerza;            
        señal_actual = señal_sensores;    
        posicion_actual = posicion;
    };

};


/**
  * @struct Maquina_de_estados_protesis
  * @brief Estructura que almacena y gestiona el estado y fase actual de la prótesis.
  */
struct Maquina_de_estados_protesis
{
    enum Estado_Protesis estado_actual; ///< Estado actual de la prótesis.
    enum Fase_Estado fase_actual;       ///< Fase actual dentro del estado.
    enum Causa_Seguridad error_actual;  ///< Error que provoca que haya que entrar en @ref ESTADO_SEGURIDAD

    /**
      * @brief Constructor que inicializa la prótesis en @ref ESTADO_NORMAL y @ref FASE_INICIO. Aplicamos las condiciones iniciales
      */
    Maquina_de_estados_protesis ()
    {
        estado_actual = ESTADO_NORMAL;
        fase_actual = FASE_INICIO;
        error_actual = causas_ninguna;

        /**
         * @brief iniciar condiciones utilizando los valores de los umbrales de la función anterior
         */
        Condiciones_Seguridad.actualizacion (); // ALGO ESTÁ MAL
    };

    /**
     * @brief función encargada de actualizar de manera contínua los criterios.
     * @return TRUE si debemos entrar en el estado de seguridad.
     */
    bool Comprobacion_Seguridad () 
    {
        //leer valores REVISAR CON EL CÓDIGO ORIGINAL
        float corriente_motores = ;
        float temperatura = ;
        float velocidad = ;
        float fuerza = ;
        float señal_sensores = ;
        float posicion = ;

        // actualizamos la estructura de condiciones
        Condiciones_Seguridad.actualizacion (corriente_actual, temp_actual, velocidad_actual, fuerza_actual, señal_actual, posicion_actual); ///REVISAR CÓMO LLAAMAR A ESTAS VARIABELS

        uint8_t causa = Condiciones_Seguridad.Verificar_Parámetros (); ///REVISAR COMO LLAMAR A LA FUNCIÓN

        if (causa != causas_ninguna) 
        {
            /**
             * @brief función que cambia el estado a estado de seguridad y según el error hace una fase u otra primero
             * @param fase_actual que decreta la nueva fase a la cual se acude. 
             */
            void activarSeguridad (uint8_t causa)
            {
                if (causa == causa_fuerza_excesiva) 
                {
                    fase_actual = FASE_ABRIR MOTORES // ESTA PARTE ES LO DE FASES QUE HAY QUE REVISAR
                };

                if (causa != causa_fuerza_excesiva) 
                {
                    fase_actual = FASE_DETENER MOTOR
                };
            };
            return true;
        };

        if (causa == causas_ninguna) 
        {
            return false;
        };

    };


    /**
      * @brief Cambia el estado de la prótesis y reinicia la fase a @ref FASE_1.
      * @param nuevo_estado Nuevo estado que tomará la prótesis.
      */
    void cambiarEstado(Estado_Protesis nuevo_estado)
    {
        if (Comprobacion_Seguridad == true)  ////VER COMO HAY QUE PONERLO BIEN
        {
            nuevo_estado = activarSeguridad (uint8_t causa);
        };

        if (Comprobacion_Seguridad == false) 
        {
            estado_actual = nuevo_estado;
        }; 
    }
 
     /**
      * @brief Cambia la fase actual de la prótesis.
      * @param nueva_fase Nueva fase que se establecerá.
      */
    void cambiarFase(Fase_Estado nueva_fase)
     {
        if (Comprobacion_Seguridad == false) 
        {
            fase_actual = nueva_fase;
        };
         
     }
};


