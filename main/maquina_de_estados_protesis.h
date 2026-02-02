/*
 * C-compatible definition of the prosthesis state machine.
 * The original file contained C++ constructors and methods which are
 * invalid when included from .c sources. This header exposes a plain
 * C struct and small inline helpers to operate on it from C code.
 */

#pragma once

/**
 * @enum Estado_Protesis
 * @brief Estados posibles de la prótesis.
 */
enum Estado_Protesis
{
    ESTADO_NORMAL,
    ESTADO_SEGURIDAD,
    ESTADO_CALIBRADO_UMBRALES,
    ESTADO_CALIBRADO_MOTORES
};

/**
 * @enum Fase_Estado
 * @brief Fases asociadas a cada estado de la prótesis.
 */
enum Fase_Estado
{
    FASE_1,
    FASE_2,
    FASE_3,
    FASE_4,
    FASE_5
};

/**
 * @struct Maquina_de_estados_protesis
 * @brief Estructura que almacena el estado y la fase actual.
 */
struct Maquina_de_estados_protesis
{
    enum Estado_Protesis estado_actual;
    enum Fase_Estado fase_actual;
};

/**
 * @brief Inicializa una máquina de estados en estado normal y fase 1.
 */
static inline void maquina_inicializar(struct Maquina_de_estados_protesis *m)
{
    m->estado_actual = ESTADO_NORMAL;
    m->fase_actual = FASE_1;
}

/**
 * @brief Cambia el estado y resetea la fase a FASE_1.
 */
static inline void maquina_cambiarEstado(struct Maquina_de_estados_protesis *m, enum Estado_Protesis nuevo_estado)
{
    m->estado_actual = nuevo_estado;
    m->fase_actual = FASE_1;
}

/**
 * @brief Cambia la fase actual.
 */
static inline void maquina_cambiarFase(struct Maquina_de_estados_protesis *m, enum Fase_Estado nueva_fase)
{
    m->fase_actual = nueva_fase;
}