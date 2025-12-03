/**
 * @file Maquina_estados_protesis.h
 * @brief Definición de la máquina de estados principal de la prótesis.
 * @details
 * La máquina de estados define los modos de funcionamiento de la prótesis y las fases asociadas a cada modo.
 * 
 * **Estados posibles:**
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
 *      - @ref parada de motores
 *      - @ref apertura de la mano 
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


/** Determinación nuevos criterios para las fases:
 * 
 * FASE_INICIO << en esta se realiza el incio de los cuatro estados excepto el de seguridad
 * FASE_FINAL << en esta se realiza el último paso de los estados
 * FASE_CAMBIO_ESTADO << en esta se realiza el cambio de estados tras cierta función/configuración/hecho.
 * FASE_
 */