/**
 * @file globales.h
 * @brief Definiciones globales, pines, constantes y variables compartidas para el control de la prótesis mioeléctrica.
 * @details
 * Contiene:
 * - Configuración general de hardware y parámetros del sistema.
 * - Definiciones de pines para sensores, motor, y LEDs.
 * - Variables globales compartidas entre módulos.
 * - Funciones de registro para depuración y análisis.
 */

 #pragma once

 // ==========================
 //       Librerías
 // ==========================
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "maquina_de_estados_protesis.h"
 
 // ==========================
 //   Parámetros de hardware
 // ==========================
 
 #define TIMER_FREQ 1000000 ///< Frecuencia base del temporizador por hardware en Hz (1 MHz = resolución de 1 µs).
 #define SAMPLING_FREQ 2000 ///< Frecuencia de muestreo de la señal EMG en Hz.
 #define CIRCULAR_ARRAY_SIZE 50 ///< Tamaño de los buffers circulares para almacenamiento de muestras EMG.
 #define POSICION_MAXIMA_MOTOR 4230 ///< Posición máxima permitida para el motor (unidad: pasos del encoder).
 #define POSICION_MINIMA_MOTOR 0    ///< Posición mínima permitida para el motor (unidad: pasos del encoder).
 #define VELOCIDAD_MOTOR 80 ///< Velocidad base del motor (% de PWM, 0 = parado, 100 = máxima velocidad).
 #define MICROSECONDS_TO_TICKS(us) ((us) / (1000000 / configTICK_RATE_HZ)) ///< Conversión de microsegundos a ticks del sistema FreeRTOS.
 #define NUMERO_CARACTERISTICAS 3 ///< Número de características EMG calculadas por cada ventana de datos.
 
 // ==========================
 //   Frecuencia de tareas
 // ==========================
 
 /**
  * @brief Frecuencia de ejecución de las tareas posteriores al muestreo.
  * @details 
  * - Determina cada cuántas muestras se activa el flag `completedBufferFlag`.
  * @warning Debe ser un divisor de @ref CIRCULAR_ARRAY_SIZE para evitar desfases.
  */
 #define FREC_EJ_TAREAS_POST_TOMA_DATOS (CIRCULAR_ARRAY_SIZE / 1)
 
 // ==========================
 //   Calibración
 // ==========================
 
 /**
  * @brief Número total de arrays de características que se almacenan durante la calibración de umbrales.
  * @details Se usa para calcular umbrales de activación y desactivación basados en la media.
  */
 #define NUMERO_TOTAL_ARRAYS_CALIBRACION_UMBRALES 300
 
 // ==========================
 //   LEDs indicadores
 // ==========================
 
 #define LED_BRIGHTNESS 20 ///< Brillo de los LEDs (% de PWM, 0 = apagado, 100 = máximo).
 #define LED_TYPE 0 ///< Tipo de activación de los LEDs: `1` = activo a nivel alto, `0` = activo a nivel bajo.
 
 // ==========================
 // Variables de velocidad
 // ==========================
 
 /**
  * @brief Velocidad del motor ajustada según @ref VELOCIDAD_MOTOR.
  * @details Conversión de porcentaje a valor PWM real (0–255).
  */
 float velocidad_motor_procesada = 2.55 * VELOCIDAD_MOTOR;
 
 // ==========================
 //  Manejadores de tareas
 // ==========================
 TaskHandle_t task_core0 = NULL; ///< Tarea principal en core 0.
 TaskHandle_t task_core1 = NULL; ///< Tarea principal en core 1.
 
 // ==========================
 //   Definición de pines
 // ==========================
 
 #define redPin_1 5    ///< Pin PWM salida LED 1 rojo.
 #define greenPin_1 18 ///< Pin PWM salida LED 1 verde.
 #define bluePin_1 23  ///< Pin PWM salida LED 1 azul.
 #define redPin_2 19   ///< Pin PWM salida LED 2 rojo.
 #define greenPin_2 22 ///< Pin PWM salida LED 2 verde.
 #define bluePin_2 21  ///< Pin PWM salida LED 2 azul.
 #define botonPin 10   ///< Pin de entrada para botón físico de seguridad.
 #define bateriaPin 4  ///< Pin ADC para medición de nivel de batería.
 #define EMGPin 15     ///< Pin ADC para lectura de señal EMG.
 #define encoderAPin 38 ///< Pin entrada canal A del encoder rotativo.
 #define encoderBPin 37 ///< Pin entrada canal B del encoder rotativo.
 #define motorEnabePWMPin 14 ///< Pin PWM para control de velocidad del motor.
 #define motorPhasePin 27    ///< Pin digital para control de dirección del motor.
 #define motorSleepPin 26    ///< Pin digital para control de estado (SLEEP/ON) del driver de motor.
 
 // ==========================
 // Variables motor
 // ==========================
 bool motorArrived; ///< Flag: `true` si el motor está en posición objetivo o límite, `false` en movimiento.
 uint16_t posicionMotor; ///< Posición actual del motor (en pasos de encoder).
 
 // ==========================
 //   Sincronización
 // ==========================
 SemaphoreHandle_t semaforoMuestreo = xSemaphoreCreateBinary(); ///< Semáforo para sincronizar ISR de muestreo y procesamiento.
 
 // ==========================
 // Buffers y datos EMG
 // ==========================
 float copiaDeteccion_buffer[CIRCULAR_ARRAY_SIZE]; ///< Copia temporal de la señal EMG para detección.
 float copiaDeteccion_diff[CIRCULAR_ARRAY_SIZE];   ///< Copia temporal de diferencias de tiempo entre muestras.
 uint16_t diff[CIRCULAR_ARRAY_SIZE];               ///< Diferencias de tiempo (µs) entre muestras.
 uint16_t buffer[CIRCULAR_ARRAY_SIZE];             ///< Buffer circular de muestras EMG crudas.
 float filteredEMG[CIRCULAR_ARRAY_SIZE];           ///< Señal EMG filtrada.
 
 float result[1]; ///< Resultado de la última capa de la ia.
 
 // ==========================
 // Variables globales de control
 // ==========================
 int i_muestreo = 0; ///< Índice global del buffer de muestreo.
 uint16_t resultDeteccion = 0; ///< Resultado binario (0/1) de la detección EMG.
 uint8_t estadoPulso = 0; ///< Estado del pulso detectado (0 = ninguno, 1 = corto, 2 = largo).
 uint16_t nivelBateria; ///< Nivel de batería medido.
 bool completedBufferFlag = false; ///< Flag que indica si el buffer está completo y listo para procesar.
 
 // ==========================
 // Características EMG
 // ==========================
 float MAVEMG = 0; ///< Media del valor absoluto (MAV) de la señal EMG actual.
 float VarianzaEMG = 0; ///< Varianza de la señal EMG actual.
 float WLEMG = 0; ///< Longitud de onda (Waveform Length) de la señal EMG actual.
 
 uint16_t MAVActivada = 0; ///< Flag activación MAV.
 bool VarActivada = 0; ///< Flag activación Varianza.
 bool WLActivada = 0; ///< Flag activación WL.
 float umbralActMAV = 5000000.0; ///< Umbral de activación MAV.
 float umbralDesMAV = 0; ///< Umbral de desactivación MAV.
 float umbralActVar = 5000000.0; ///< Umbral de activación Varianza.
 float umbralDesVar = 0; ///< Umbral de desactivación Varianza.
 float umbralActWL = 5000000.0; ///< Umbral de activación WL.
 float umbralDesWL = 0; ///< Umbral de desactivación WL.
 
 // ==========================
 // Estado de la prótesis
 // ==========================
 Maquina_de_estados_protesis estado_protesis; ///< Máquina de estados general de la prótesis.
 
 // ==========================
 // Estructuras de depuración
 // ==========================
 
 /**
  * @struct LogMessage
  * @brief Estructura para almacenar datos de depuración y registro de tareas.
  * @details Incluye información de core, tiempos, estado, fase, tarea, señal EMG y posición del motor.
  */
 typedef struct {
   bool core;                ///< Núcleo de ejecución: 0 o 1.
   unsigned long tiempo;     ///< Tiempo desde arranque (µs).
   uint8_t estado;           ///< Estado actual de la prótesis (ver @ref Estado_Protesis).
   uint8_t fase;             ///< Fase actual (ver @ref Fase_Estado).
   uint8_t tarea;            ///< ID de la tarea ejecutada.
   float emgCruda;           ///< Última muestra EMG cruda.
   uint8_t resDet;           ///< Resultado detección EMG (0/1).
   uint16_t posMotor;        ///< Posición actual del motor.
 } LogMessage;
 
 QueueHandle_t logQueue; ///< Cola para almacenar mensajes de tipo LogMessage.
 
 /**
  * @brief Envía un registro de tarea a la cola @ref logQueue.
  * @param tareaID Identificador numérico de la tarea.
  * @details Incluye en el mensaje: núcleo, tiempo, estado, fase, ID de tarea, valor EMG, resultado de detección y posición del motor.
  */
 void logTarea(uint8_t tareaID) {
   static bool inicializacion;
   static unsigned long boot_time;
 
   if (!inicializacion){
     boot_time = esp_timer_get_time();
     inicializacion = true;
   }
 
   unsigned long tiempo = esp_timer_get_time() - boot_time;
 
   LogMessage msg;
   msg.core = xPortGetCoreID();
   msg.tiempo = tiempo;
   msg.estado = estado_protesis.estado_actual;
   msg.fase = estado_protesis.fase_actual + 1;
   msg.tarea = tareaID;
   msg.emgCruda = filteredEMG[0];
   msg.resDet = resultDeteccion;
   msg.posMotor = posicionMotor;
 
   xQueueSend(logQueue, &msg, portMAX_DELAY);
 }
 