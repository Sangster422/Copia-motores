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
    float corriente_motores; ///< Corriente actual.
    float temperatura;       ///< Temperatura actual.
    float velocidad;         ///< Velocidad actual.
    float fuerza;            ///< Fuerza actual.
    bool señal_sensores;     ///< Recibimos señal de los sensores o no.
    bool posicion;           ///< Comprobamos el estado del encoder.
    
    //Umbrales configurables
    float umbral_corriente; 
    float umbral_temperatura;
    float umbral_velocidad;
    float umbral_fuerza;

    /**
     * @brief Constructor donde establecemos los valores por defecto
     */
    Condiciones_Seguridad () //REVISAR VALORES
    {
        corriente_motores = 0.0f,
        temperatura = 25.0f,
        velocidad = 0.0f,
        fuerza = 0.0f,
        señal_sensores = true,
        posicion = true,
        umbral_corriente = UMBRAL_CORRIENTE_DEFAULT,
        umbral_temperatura = UMBRAL_TEMPERATURA_DEFAULT,
        umbral_velocidad = UMBRAL_VELOCIDAD_DEFAULT,
        umbral_fuerza = UMBRAL_FUERZA_DEFAULT
    };

    /**
     * @brief Verifica si algún parámetro está en nivel crítico
     * @return Código de causa si hay problema, causas_ninguna si todo correcto.
     */
    Causa_Seguridad Verificar_Parametros () 
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
    * @brief en esta vamos actualizando los valores en los que realizamos la comprobación.
    */
    void actualizacion (float corriente_actual, float temp_actual, float velocidad_actual, float fuerza_actual, bool señal_actual, bool posicion_actual) 
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
    Estado_Protesis estado_actual;     ///< Estado actual de la prótesis.
    Fase_Estado fase_actual;           ///< Fase actual dentro del estado.
    Causa_Seguridad error_actual;      ///< Error que provoca que haya que entrar en el estado de seguridad.
    Condiciones_Seguridad condiciones; ///< Condiciones para que se entre o no en el estado de seguridad.

    //variables para que nos avise cómo va la prótesis una vez dentro del estado de seguridad.
    bool en_seguridad;
    bool problema_resuelto;

    /**
      * @brief Constructor que inicializa la prótesis en @ref ESTADO_NORMAL y @ref FASE_INICIO. Aplicamos las condiciones iniciales.
      */
    Maquina_de_estados_protesis ()
    {
        estado_actual = ESTADO_NORMAL;
        fase_actual = FASE_PAUSA;
        error_actual = causas_ninguna;
        en_seguridad = false;
        problema_resuelto = false; 
    };

    /**
     * @brief función encargada de actualizar de manera contínua los criterios para ver si hay que entrar en @ref ESTADO_SEGURIDAD.
     * @return TRUE si debemos entrar en el estado de seguridad, false si está todo correcto.
     */
    bool Comprobacion () 
    {
        if (en_seguridad) 
        {
            return true;
        };

        //leer valores
        float corriente_motores = ;
        float temperatura = ;
        float velocidad = ;
        float fuerza = ;
        bool señal_sensores = ;
        bool posicion = ;

        // actualizamos la estructura de condiciones (sustituimos en la estructura del constructor los nuevos valores que hemos leído)
        condiciones.actualizacion (corriente_motores, temperatura, velocidad, fuerza, señal_sensores, posicion);
        
        // comprobamos dichos valores a que tipo de causa coinciden. 
        Causa_Seguridad causa = condiciones.Verificar_Parametros (); 

        if (causa != causas_ninguna)
        {
            return true;
        };

        if (causa == causas_ninguna) 
        {
            return false;
        };
    };

    /**
    * @brief Entramos en el ESTADO_SEGURIDAD con una causa específica
    */
    void ActivarSeguridad (Causa_Seguridad causa) 
    {
        estado_actual = ESTADO_SEGURIDAD;
        error_actual = causa;
        en_seguridad = true;
        problema_resuelto = false;

        // Según la causa queremos que haga una fase u otra primero
        if(error_actual == causa_fuerza_excesiva) 
        {
            fase_actual = FASE_PASO_1; // Primero abrir para liberar
        } 
        else 
        {
            fase_actual = FASE_PASO_2; // Detener inmediatamente
        }
    };
   
    /**
     * @brief función para verificar si ya se ha resuelto el problema y de esa forma salir del ESTADO_SEGURIDAD
     */
    bool Comprobacion_Problema () 
    {
        if (!en_seguridad) 
        {
            return true;
        };

        // Re-lectura estado actual (HACEN FALTA FUNCIONES PARA LEER LOS VALORES)
        float corriente = ;
        float temperatura = ;
        float velocidad = ;
        float fuerza = ;
        bool señal_sensores = ;
        bool posicion = ;


        //verificación según la causa mediante la comparación con los valores de los umbrales.
        switch(error_actual) 
        {
            case causa_sobrecorriente:
                problema_resuelto = (corriente <= condiciones.umbral_corriente);
                break;
                
            case causa_sobrecalentameinto:
                problema_resuelto = (temperatura <= condiciones.umbral_temperatura);
                break;
                
            case causa_velocidad_excesiva:
                problema_resuelto = (velocidad <= condiciones.umbral_velocidad);
                break;
                
            case causa_fuerza_excesiva:
                problema_resuelto = (fuerza <= condiciones.umbral_fuerza);
                break;
                
            case causa_perdida_sensores:
                problema_resuelto = señal_sensores;
                break;
                
            case causa_error_encoder:
                problema_resuelto = posicion;
                break;
                
            default:
                problema_resuelto = true;
                break;
        }
        
        return problema_resuelto;
    };

    /**
      * @brief Cambia el estado de la prótesis y reinicia la fase a @ref FASE_1.
      * @param nuevo_estado Nuevo estado que tomará la prótesis.
      */
    void cambiarEstado(Estado_Protesis nuevo_estado)
    {
        if (Comprobacion)  
        {
            nuevo_estado =  ActivarSeguridad (Causa_Seguridad causa);
        };

        if (!Comprobacion) 
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
        if (!Comprobacion) 
        {
            fase_actual = nueva_fase;
        };

        if (!Comprobacion_Problema) 
        {
            if (problema_resuelto) 
            {
                void Salir_Seguridad () 
                {
                    estado_actual = ESTADO_DESCANSO;
                    fase_seguridad = FASE_SEG_DETENER;
                    causa_emergencia = CAUSA_NINGUNA;
                    en_seguridad = false;
                    problema_resuelto = true;
                };
            };
        }
         
     };

};


/////IMPLEMENTAR FUNCIONES PARA LA LECTURA DE LAS VARIABLES


