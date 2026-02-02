/**
 * @file activacion_motores.h
 * @brief Implementación de las funciones de control y gestión de motores.
 * @details
 * Contiene las funciones encargadas de la activación, control y parada de los motores,
 * así como el manejo del encoder y la interpretación de la máquina de estados.
 * Todas las variables globales y constantes utilizadas están definidas en @ref globales.h.
 */

#include "globales.h"
#include "motores.h"
#include "maquina_de_estados_protesis.h"


// Flags de control de motor
bool motorAbrir = false;   ///< Indica si el motor debe abrir.
bool motorCerrar = false;  ///< Indica si el motor debe cerrar.

/**
 * @brief Instancia del controlador del motor (C style).
 * @details Declare the struct and initialize it in `iniciaEncoder()`.
 */
struct motores motor;

/**
 * @brief Rutina de interrupción para el encoder del motor.
 * @details Llamada en cada cambio de estado del pin del encoder A para registrar pasos.
 */
void IRAM_ATTR updateMotores(void* arg)
{
  motores_step(&motor);
}

/**
 * @brief Inicializa la interrupción del encoder asociado al pin @ref encoderAPin. En cada cambio del pin ejecuta @ref updateMotores.
 */
void iniciaEncoder()
{

  gpio_config_t io_conf = {0};
  io_conf.intr_type = GPIO_INTR_ANYEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << encoderAPin);
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_config(&io_conf);

  // Registrar la ISR
  gpio_install_isr_service(0); // 0 = usar default ISR service
  gpio_isr_handler_add((gpio_num_t)encoderAPin, updateMotores, NULL);

  /* Initialize motor controller now that GPIOs are ready */
  motores_init(&motor,
               (gpio_num_t)encoderBPin,
               (gpio_num_t)encoderAPin,
               (gpio_num_t)motorEnabePWMPin,
               (gpio_num_t)motorPhasePin,
               (gpio_num_t)motorSleepPin,
               (uint16_t)POSICION_MAXIMA_MOTOR,
               (uint16_t)POSICION_MINIMA_MOTOR,
               LEDC_CHANNEL_0);

}

/**
 * @brief Comprueba si el motor mantiene la posición estable por un tiempo determinado.
 * @details
 * Evalúa si la posición actual del motor se mantiene dentro de un margen de tolerancia
 * respecto a la posición anterior durante 3 segundos, para detectar si el motor se ha bloqueado.
 *
 * @retval true  Si la posición se mantiene estable por 3 segundos.
 * @retval false En caso contrario.
 */
bool checkMotorPressure()
{
  static uint16_t posicionAnterior = 0;
  static unsigned long tiempoPresion = 0;
  static const uint8_t margenPresion = 1;

  uint16_t posicionActual = motores_read_position(&motor);
  // Asegurar que limiteInferior no tome valores negativos
  uint16_t limiteInferior = (posicionAnterior > margenPresion) ? (posicionAnterior - margenPresion) : 0;
  uint16_t limiteSuperior = posicionAnterior + margenPresion;

  if ((posicionActual >= limiteInferior) && (posicionActual <= limiteSuperior))
  { 
    // Si está dentro del margen
    if (tiempoPresion == 0)
    {
      tiempoPresion = esp_timer_get_time(); // Iniciar temporizador
    }
    else if ((esp_timer_get_time() - tiempoPresion) >= 3000000)
    {
      tiempoPresion = 0; // Reiniciar temporizador
      return true;       // Se mantiene estable por 3 segundos
    }
  }
  else
  {
    tiempoPresion = esp_timer_get_time(); // Reiniciar temporizador
  }

  posicionAnterior = posicionActual;
  return false;
}

/**
 * @brief Interpreta el estado de la prótesis y asigna las acciones de motor.
 * @details
 * Según el estado y la fase definidos en @ref estado_protesis,
 * establece los flags @ref motorAbrir y @ref motorCerrar.
 */



void interpretarMaquinaEstados()   
{
  switch (estado_protesis.estado_actual)
  {
  case ESTADO_NORMAL:
  case ESTADO_CALIBRADO_MOTORES:
    switch (estado_protesis.fase_actual)
    {
    case FASE_1:
      motorAbrir = motorCerrar = 0;
      break;
    case FASE_2:
      motorAbrir = 1;
      motorCerrar = 0;
      break;
    case FASE_3:
      motorAbrir = 0;
      motorCerrar = 1;
      break;
    default:
      motorAbrir = motorCerrar = 0;
      break;
    }
    break;

  case ESTADO_SEGURIDAD:
    switch (estado_protesis.fase_actual)
    {
    case FASE_1:
      motorAbrir = motorCerrar = 0;
      break;
    case FASE_2:
      motorAbrir = 1;
      motorCerrar = 0;
      break;
    default:
      motorAbrir = motorCerrar = 0;
      break;
    }
    break;

  default:
    motorAbrir = motorCerrar = 0;
    break;
  }
}





// ESPACIO EN BLANCO SIMPLEMENTE PARA QUE COINCIDAN LÍNEAS CON activacion_motores_copy



// ESPACIO EN BLANCO SIMPLEMENTE PARA QUE COINCIDAN LÍNEAS CON activacion_motores_copy




// ESPACIO EN BLANCO SIMPLEMENTE PARA QUE COINCIDAN LÍNEAS CON activacion_motores_copy



// ESPACIO EN BLANCO SIMPLEMENTE PARA QUE COINCIDAN LÍNEAS CON activacion_motores_copy



// ESPACIO EN BLANCO SIMPLEMENTE PARA QUE COINCIDAN LÍNEAS CON activacion_motores_copy




/**
 * @brief Ejecuta la acción de abrir el motor.
 * @details
 * Hace girar el motor en sentido de apertura hasta la posición mínima.
 *
 * @retval true  Si llega al límite.
 * @retval false En caso contrario.
 */
bool abrirMotor()
{
  bool direccionMotor = ABRIR;
  bool motorLlegado = motores_start_until(&motor, direccionMotor, POSICION_MINIMA_MOTOR, velocidad_motor_procesada);
  return motorLlegado;
}

/**
 * @brief Ejecuta la acción de cerrar el motor.
 * @details
 * Hace girar el motor en sentido de cierre hasta la posición máxima o hasta detectar presión.
 *
 * @retval true  Si llega al límite o detecta presión.
 * @retval false En caso contrario.
 */
bool cerrarMotor()
{
  bool direccionMotor = CERRAR;
  bool motorLlegado = motores_start_until(&motor, direccionMotor, POSICION_MAXIMA_MOTOR, velocidad_motor_procesada);
  bool motorPresionando = (estado_protesis.estado_actual == ESTADO_NORMAL) ? checkMotorPressure() : false;
  return motorLlegado || motorPresionando;
}

/**
 * @brief Detiene la rotación del motor inmediatamente.
 */
void pararMotor()
{
  motores_stop_rotation(&motor);
}

/**
 * @brief Control principal de activación de motores.
 * @details
 * Ajusta la velocidad del motor según el estado de la prótesis,
 * interpreta la máquina de estados y ejecuta las acciones correspondientes.
 * También gestiona la calibración de motores y actualiza la posición del motor.
 * @warning Siempre que el sistema salga de la calibración de motores, la posición será 0 (prótesis abierta).
 */
void activacionMotores()
{
  // Ajustar la velocidad del motor según el estado
  if (estado_protesis.estado_actual == ESTADO_CALIBRADO_MOTORES || estado_protesis.estado_actual == ESTADO_CALIBRADO_UMBRALES)
  {
    
    velocidad_motor_procesada = 2.55 * 50;
  }
  else
  {
    velocidad_motor_procesada = 2.55 * VELOCIDAD_MOTOR;
  }

  interpretarMaquinaEstados();

  if (motorAbrir)
  {
    motorArrived = abrirMotor();
  }
  else if (motorCerrar)
  {
    motorArrived = cerrarMotor();
  }
  else
  {
    pararMotor();
    motorArrived = false;
  }

  // Calibración de motores: establecer posición inicial
  if (estado_protesis.estado_actual == ESTADO_CALIBRADO_MOTORES)
  {
    if (estado_protesis.fase_actual != FASE_4)
    {
      uint16_t posicionIntermedia = (POSICION_MAXIMA_MOTOR + POSICION_MINIMA_MOTOR) / 2;
      motores_set_position(&motor, posicionIntermedia);
    }
    else
    {
      motores_set_position(&motor, 0);
      maquina_cambiarEstado(&estado_protesis, ESTADO_NORMAL);
    }
  }

  posicionMotor = motores_read_position(&motor);
}