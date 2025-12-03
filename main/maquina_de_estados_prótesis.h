/**
 * @file Maquina_estados_protesis.h
 * @brief Definición de la máquina de estados principal de la prótesis.
 * @details
 * La máquina de estados define los modos de funcionamiento de la prótesis y las fases asociadas a cada modo.
 * 
 * **Estados posibles:**
 * - @ref ESTADO_NORMAL:
 *   - @ref FASE_1: Motor en reposo. -------- reposo
 *   - @ref FASE_2: Motor abre la mano. ------- apertura
 *   - @ref FASE_3: Motor cierra la mano. ------- cierre
 * 
 * - @ref ESTADO_SEGURIDAD:
 *   - @ref FASE_1: Prótesis parada durante 3 segundos. ----- parada total
 *   - @ref FASE_2: Abrir mano durante 3 segundos. ---- posición base
 * 
 * - @ref ESTADO_CALIBRADO_UMBRALES:
 *   - @ref FASE_1: Calibrar umbrales de activación. ---- obtención datos básicos señales usuario (contracción voluntaria musculos)
 *   - @ref FASE_2: Esperar hasta que se suelte el botón (umbrales calculados). -- se presiona botón durante la calibración y se suelta para continuar
 *   - @ref FASE_3: Calibrar umbrales de desactivación. --- medir señal que será reposo
 * 
 * - @ref ESTADO_CALIBRADO_MOTORES:
 *   - @ref FASE_1: Motor en reposo. --- motores en modo baja potencia
 *   - @ref FASE_2: Motor abre la mano (manual). --- determina límite físico
 *   - @ref FASE_3: Motor cierra la mano (manual). -- determina límite físico de cierre
 *   - @ref FASE_4: Configurar posición 0 y cambiar a @ref ESTADO_NORMAL. ---- establece cual es la posición 0 y guarda todos los parámetros calibrados
 * 
 * 
 *
 * @note Las fases no son necesariamente secuenciales; su numeración es solo para organizar.
 */

 #pragma once

 /**
  * @enum Estado_Protesis
  * @brief Estados posibles de la prótesis.
  */
 enum Estado_Protesis
 {
     ESTADO_NORMAL,             ///< Funcionamiento normal de la prótesis.
     ESTADO_SEGURIDAD,          ///< Modo de seguridad (secuencia De seguridad: parada y apertura).
     ESTADO_CALIBRADO_UMBRALES, ///< Modo de calibración de umbrales de activación/desactivación.
     ESTADO_CALIBRADO_MOTORES   ///< Modo de calibración de límites mecánicos del motor.
 };
 
 /**
  * @enum Fase_Estado
  * @brief Fases asociadas a cada estado de la prótesis.
  */
 enum Fase_Estado
 {
     FASE_1, ///< Función según el estado actual (reposo, calibración, etc.).
     FASE_2, ///< Función según el estado actual (abrir mano, esperar, etc.).
     FASE_3, ///< Función según el estado actual (cerrar mano, calibrar desactivación, etc.).
     FASE_4, ///< Función según el estado actual (configuración de posición, cambio de estado).
     FASE_5  ///< Fase adicional para extensiones futuras.
 };
 
 /**
  * @struct Maquina_de_estados_protesis
  * @brief Estructura que almacena y gestiona el estado y fase actual de la prótesis.
  */
 struct Maquina_de_estados_protesis
 {
     enum Estado_Protesis estado_actual; ///< Estado actual de la prótesis.
     enum Fase_Estado fase_actual;       ///< Fase actual dentro del estado.
 
     /**
      * @brief Constructor que inicializa la prótesis en @ref ESTADO_NORMAL y @ref FASE_1.
      */
     Maquina_de_estados_protesis()
     {
         estado_actual = ESTADO_NORMAL;
         fase_actual = FASE_1;
     }
 
     /**
      * @brief Cambia el estado de la prótesis y reinicia la fase a @ref FASE_1.
      * @param nuevo_estado Nuevo estado que tomará la prótesis.
      */
     void cambiarEstado(Estado_Protesis nuevo_estado)
     {
         estado_actual = nuevo_estado;
         fase_actual = FASE_1;
     }
 
     /**
      * @brief Cambia la fase actual de la prótesis.
      * @param nueva_fase Nueva fase que se establecerá.
      */
     void cambiarFase(Fase_Estado nueva_fase)
     {
         fase_actual = nueva_fase;
     }
 };
 