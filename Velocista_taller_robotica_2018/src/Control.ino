/*
 * Función para obtener el sensor activo más a la derecha
 */
int sensorDerecha(bool lectura_CNYS_para_PD[6])
{
    if (lectura_CNYS_para_PD[5] == 1) return(6); // Sensor más a la derecha
    if (lectura_CNYS_para_PD[4] == 1) return(5);
    if (lectura_CNYS_para_PD[3] == 1) return(4);
    if (lectura_CNYS_para_PD[2] == 1) return(3);
    if (lectura_CNYS_para_PD[1] == 1) return(2);
    if (lectura_CNYS_para_PD[0] == 1) return(1); // Sensor más a la izquierda
    return(0); // No se detecta la línea
}

/*
 * Función para obtener el sensor activo más a la izquierda
 */
int sensorIzquierda(bool lectura_CNYS_para_PD[6])
{
    if (lectura_CNYS_para_PD[0] == 1) return(1); // Sensor más a la izquierda
    if (lectura_CNYS_para_PD[1] == 1) return(2);
    if (lectura_CNYS_para_PD[2] == 1) return(3);
    if (lectura_CNYS_para_PD[3] == 1) return(4);
    if (lectura_CNYS_para_PD[4] == 1) return(5);
    if (lectura_CNYS_para_PD[5] == 1) return(6); // Sensor más a la derecha
    return(0); // No se detecta la línea
}

/*
 * Función para detectar si se está leyendo negro/blanco en todos los sensores,
 * lo que podría indicar una marca de frenada en la pista
 */
void marcasFrenada()
{
    if (lectura_CNYS[0] == 1
        && lectura_CNYS[1] == 1
        && lectura_CNYS[2] == 1
        && lectura_CNYS[3] == 1
        && lectura_CNYS[4] == 1
        && lectura_CNYS[5] == 1)
        {
            contador_parada_negro++;
        }
    if (lectura_CNYS[0] == 0
        && lectura_CNYS[1] == 0
        && lectura_CNYS[2] == 0
        && lectura_CNYS[3] == 0
        && lectura_CNYS[4] == 0
        && lectura_CNYS[5] == 0)
        {
            contador_parada_blanco++;
        }
}

/*
 * Función para el cálculo de la dirección
 */
void calculoDireccion() // Para dirección normal
{
    direccion = sensorDerecha(lectura_CNYS) + sensorIzquierda(lectura_CNYS);
}

/*
 * Función para el control PD
 */
//TipoPID controlPD (TipoPID PID)
void controlPD()
{
    // Si perdemos la línea seguimos en la última dirección detectada
    if (direccion == 0) direccion = direccion_anterior;
    else                direccion_anterior = direccion;

    error = direccion - REFERENCIA_DIRECCION; // Error respecto a la dirección que se debe tomar

    control_pwm = error * KP + (error - error_anterior) * KD; // Control PD

    error_anterior = error; // Parte derivativa
}

/*
 * Función para actuar sobre los motores con la salida del PID
 */
void actuacionMotores(int control_pwm)
{
    int motor_derecho_pwm;      // Señal PWM del motor derecho
    int motor_izquierdo_pwm;    // Señal PWM del motor izquierda

    motor_derecho_pwm = VELOCIDAD_BASE - control_pwm;   // Valor del motor derecho
    motor_izquierdo_pwm = VELOCIDAD_BASE + control_pwm; // Valor del motor izquierdo

    controlMotores(motor_izquierdo_pwm, motor_derecho_pwm); // Actuación de los motores
}
