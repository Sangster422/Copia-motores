/**
 * @file Maquina_estados_protesis.h
 * @brief Definición de la máquina de estados principal de la prótesis.
 * @details
 * La máquina de estados define los modos de funcionamiento de la prótesis y las fases asociadas a cada modo.
 * 
 * **Estados posibles:** 
 * - @ref ESTADO_NORMAL: 
 *      - @ref FASE_PAUSA: Motor en reposo.
 *      - @ref FASE_PASO_1: Motor abre la mano.
 *      - @ref FASE_PASO_2: Motor cierra la mano. 
 * 
 * - @ref ESTADO_PAUSA:
 *      - @ref FASE_PASO_1: Prótesis parada durante 3 segundos.
 *      - @ref FASE_PASO_2: Abrir mano durante 3 segundos.
 * 
 * - @ref ESTADO_CALIBRADO_UMBRALES:
 *   - @ref FASE_PASO_1: Calibrar umbrales de activación.
 *   - @ref FASE_PAUSA: Esperar hasta que se suelte el botón (umbrales calculados).
 *   - @ref FASE_PASO_2: Calibrar umbrales de desactivación.
 * 
 * - @ref ESTADO_CALIBRADO_MOTORES:
 *   - @ref FASE_PAUSA: Motor en reposo.
 *   - @ref FASE_PASO_1: Motor abre la mano (manual).
 *   - @ref FASE_PASO_2: Motor cierra la mano (manual).
 *   - @ref FASE_CAMBIO_ESTADO: Configurar @ref POSICION_0 y cambiar a @ref ESTADO_NORMAL.
 * 
 * - @ref ESTADO_SEGURIDAD:
 *   - @ref FASE_PAUSA: Espera es esta fase hasta que se resuelva el problema. No hace nada. 
 *   - @ref FASE_PASO_1: Motor abre la mano.  
 *   - @ref FASE_PASO_2: Parada de motores.
 *   - @ref FASE_CAMBIO_ESTADO: Colocarse en @ref POSICION_0 y cambiar a @ref ESTADO_NORMAL.
 * 
 * @note Las fases no son necesariamente secuenciales; su numeración es solo para organizar.
 */

#pragma once

//definición de los valores estándard de la prótesis (los umbrales) (genéricos de Internet)
#define UMBRAL_CORRIENTE_DEFAULT    2.0f    // 2.0 Amperios
#define UMBRAL_TEMPERATURA_DEFAULT  60.0f   // 60°C
#define UMBRAL_VELOCIDAD_DEFAULT    100.0f  // unidades/segundo
#define UMBRAL_FUERZA_DEFAULT       50.0f   // 50 Newtons estimados (VARIABLE) (QUERÍAMOS MAYOR FUERZA EN CUPPER)

/**
 * @enum Estado_Protesis
 * @brief Posibles estados de la prótesis.
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
enum Fase_Estado
{
    FASE_PAUSA,         ///< Los motores se encuentran en reposo, no hacen nada, hasta que se cambie de fase y/o estado.
    FASE_PASO_1,        ///< Realiza una acción según el estado actual.
    FASE_PASO_2,        ///< Realiza una acción según el estado actual.
    FASE_CAMBIO_ESTADO, ///< Configuración de posición y cambio de estado.
};

/**
 * @enum Causa_Seguridad
 * @brief Enumera las causas/condiciones por las cuales entraríamos en @ref ESTADO_SEGURIDAD.
 */
enum Causa_Seguridad 
{
    causas_ninguna = 0, 
    causa_sobrecorriente,     ///< sobrecorriente en motores.
    causa_sobrecalentameinto, ///< niveles de temperatura demasiado elevados.
    causa_velocidad_excesiva, ///< velocidad fuera de límites (ESTA NO SÉ AÚN SI SE QUEDARÁ O NO)
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
    float corriente_motores; ///< Corriente.
    float temperatura;       ///< Temperatura.
    float velocidad;         ///< Velocidad.
    float fuerza;            ///< Fuerza.
    bool señal_sensores;     ///< Recibimos señal de los sensores o no.
    bool posicion;           ///< Comprobamos el estado del encoder.
    
    //variables para umbrales configurables
    float umbral_corriente; 
    float umbral_temperatura;
    float umbral_velocidad;
    float umbral_fuerza;

    /**
     * @brief Constructor donde establecemos los valores por defecto.
     * (**VALORES QUE ME HAN PARECIDO COHERENTES**)
     */
    Condiciones_Seguridad ()
    {
        corriente_motores = 0.0f,
        temperatura = 20.0f,
        velocidad = 0.0f,
        fuerza = 0.0f,
        señal_sensores = true,
        posicion = true,
        umbral_corriente = UMBRAL_CORRIENTE_DEFAULT,
        umbral_temperatura = UMBRAL_TEMPERATURA_DEFAULT,
        umbral_velocidad = UMBRAL_VELOCIDAD_DEFAULT,
        umbral_fuerza = UMBRAL_FUERZA_DEFAULT;
    };

    /**
     * @brief Verifica si algún parámetro está en nivel crítico
     * @return Devuelve una causa si hay problema, causas_ninguna si todo correcto.
     * Esta función la llamamos más adelante, tras leer los valores de corriente, fuerza, temp, etc.
     */
    Causa_Seguridad Verificar_Causas () 
    {   

        if (!señal_sensores) 
        {
            return causa_perdida_sensores;
        };

        if (!posicion)
        {
            return causa_error_encoder;
        };

        if(corriente_motores > umbral_corriente) 
        {
            return causa_sobrecorriente;
        };

        if(temperatura > umbral_temperatura) 
        {
            return causa_sobrecalentameinto;
        };

        if(velocidad > umbral_velocidad) 
        {
            return causa_velocidad_excesiva;
        };

        if(fuerza > umbral_fuerza) 
        {
            return causa_fuerza_excesiva;
        };
        
        return causas_ninguna;

    };

    /**
    * @brief en esta vamos actualizando los valores en los que realizamos la comprobación y leyendo los valores actuales.
    * Es una función que llamamos más adelante definiento las variables.
    */
    void actualizacion () 
    {
        //leer valores (**HARÍA FALTA COLOCAR AQUÍ UNA FUNCIÓN QUE LEA LOS VALORES**)
        float corriente_actual = ;
        float temp_actual = ;
        float velocidad_actual = ;
        float fuerza_actual = ;
        bool señal_actual = ;
        bool posicion_actual = ;

        corriente_motores = corriente_actual; 
        temperatura =  temp_actual ;      
        velocidad = velocidad_actual;        
        fuerza = fuerza_actual;            
        señal_sensores = señal_actual;    
        posicion = posicion_actual;
    };

};

/**
  * @struct Maquina_de_estados_protesis
  * @brief Estructura que almacena y gestiona el estado y fase actual de la prótesis.
  */
struct Maquina_de_estados_protesis
{
    Estado_Protesis estado_actual;     ///< Estado actual de la prótesis.
    Fase_Estado fase_actual;           ///< Fase actual dentro del estado.
    Causa_Seguridad error_actual;      ///< Error que provoca que haya que entrar en el estado de seguridad.
    Condiciones_Seguridad condiciones; ///< Condiciones para que se entre o no en el estado de seguridad.

    //variables para que nos avise cómo va la prótesis una vez dentro del estado de seguridad.
    bool en_seguridad;
    bool problema_resuelto;

    /**
      * @brief Constructor que inicializa la prótesis en @ref ESTADO_NORMAL y @ref FASE_INICIO. 
      * Aplicamos las condiciones iniciales en este.
      */
    Maquina_de_estados_protesis ()
    {
        estado_actual = ESTADO_NORMAL;
        fase_actual = FASE_PAUSA;
        error_actual = causas_ninguna;
        en_seguridad = false;
        problema_resuelto = true; 
    };

    /**
     * @brief función encargada de actualizar de manera contínua los criterios para ver si hay que entrar en @ref ESTADO_SEGURIDAD.
     * @return TRUE si debemos entrar en el estado de seguridad, false si está todo correcto.
     */
    bool ComprobacionSeguridad () 
    {
        if (en_seguridad) 
        {
            return true;
        };

        // actualizamos la estructura de condiciones
        condiciones.actualizacion ();
        
        // comprobamos dichos valores a que tipo de causa coinciden. 
        Causa_Seguridad causa = condiciones.Verificar_Causas (); 

        if (causa != causas_ninguna)
        {
            en_seguridad = true;
            problema_resuelto = false;
            return true;
        };

        if (causa == causas_ninguna) 
        {
            en_seguridad = false;
            problema_resuelto = true;
            return false;
        };
    };

    /**
    * @brief Si entramos en el ESTADO_SEGURIDAD, entramos con una causa específica
    */
    void ActivarSeguridad (Causa_Seguridad causa) 
    {
        if (ComprobacionSeguridad) //esta comprobacion era la función que nos decía si hacía falta entrar en seguridad o no. (TRUE si hacía falta)
        {
            estado_actual = ESTADO_SEGURIDAD;
            error_actual = causa;
            en_seguridad = true;
            problema_resuelto = false;

            // Si hay un problema de exceso de fuerza, queremos que por seguridad abra la mano primero sino, todas harían primero el paso 
            // de parar motores primero.
            if(error_actual == causa_fuerza_excesiva) 
            {
                fase_actual = FASE_PASO_1; // Primero abrir para liberar
            } 
            else 
            {
                fase_actual = FASE_PASO_2; // Primero que se detenga inmediatamente
            }
        };
        
    };
   
    /**
     * @brief función para verificar si ya se ha resuelto el problema y de esa forma salir del ESTADO_SEGURIDAD
     * @return devuelve diferentes valores en la función de problem_resuelto (TRUE/FALSE)
     */
    bool Comprobacion_Problema () 
    {
        if (!en_seguridad) 
        {
            return true;
        };

        // Re-lectura estado actual tras entrar en el paso 1 (abrir mano) en el caso de problemas de fuerza o
        // entrar en el paso 2 (parar motores) en el resto. Es decir, una vez hacemos la pausa cómo nos encontramos.
        condiciones.actualizacion ();

        //verificación según la causa mediante la comparación con los valores de los umbrales.
        switch(error_actual) 
        {
            case causa_sobrecorriente:
                
            if ( condiciones.corriente_motores <= condiciones.umbral_corriente) 
                {
                    problema_resuelto = true;
                }

                else
                {
                    problema_resuelto = false;
                };   
            break;
                
            case causa_sobrecalentameinto:

                if (condiciones.temperatura <= condiciones.umbral_temperatura) 
                {
                    problema_resuelto = true;
                }
                else
                {
                    problema_resuelto = false;
                };
            break;
                
            case causa_velocidad_excesiva:

                if (condiciones.velocidad <= condiciones.umbral_velocidad) 
                {
                    problema_resuelto = true;
                }

                else
                {
                    problema_resuelto = false;
                };
            break;
                
            case causa_fuerza_excesiva:
                
                if (condiciones.fuerza <= condiciones.umbral_fuerza) 
                {
                    problema_resuelto = true;
                }

                else
                {
                    problema_resuelto = false;
                };
            break;
                
            case causa_perdida_sensores:
                
                if (condiciones.señal_sensores) 
                {
                    problema_resuelto = true;
                }

                else
                {
                    problema_resuelto = false;
                };
            break;
                
            case causa_error_encoder:
                 
                if (condiciones.posicion) 
                {
                    problema_resuelto = true;
                }

                else
                {
                    problema_resuelto = false;
                };
            break;
                
            default:
                problema_resuelto = true;
                break;
        }
    };

    /**
      * @brief Cambia el estado de la prótesis y reinicia la fase a @ref FASE_PAUSA solo si la protesis no se encuentra en seguridad.
      * @param nuevo_estado Nuevo estado que tomará la prótesis.
      */
    void cambiarEstado(Estado_Protesis nuevo_estado)
    {
        if (!en_seguridad) 
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
        if (!en_seguridad) 
        {
            fase_actual = nueva_fase;
        };

        if (en_seguridad) 
        {
            switch (fase_actual)
            {
                case FASE_PASO_1:
                    fase_actual = FASE_PASO_2;
                break;

                case FASE_PASO_2:
                    fase_actual = FASE_CAMBIO_ESTADO;
                break;

                case FASE_CAMBIO_ESTADO:
                    if (problema_resuelto) 
                    {
                        en_seguridad = false;
                        cambiarEstado (ESTADO_NORMAL);
                    };
                break;
            }
        }
         
     };
};


/////IMPLEMENTAR FUNCIONES PARA LA LECTURA DE LAS VARIABLES/////////