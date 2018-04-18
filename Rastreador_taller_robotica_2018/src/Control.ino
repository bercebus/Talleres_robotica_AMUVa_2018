/*
 * Función para obtener el sensor activo más a la derecha
 */
int sensorDerecha()
{
    if (lectura_CNYS[7] == 1) return(8); // Sensor más a la derecha
    if (lectura_CNYS[6] == 1) return(7);
    if (lectura_CNYS[5] == 1) return(6);
    if (lectura_CNYS[4] == 1) return(5);
    if (lectura_CNYS[3] == 1) return(4);
    if (lectura_CNYS[2] == 1) return(3);
    if (lectura_CNYS[1] == 1) return(2);
    if (lectura_CNYS[0] == 1) return(1); // Sensor más a la izquierda
    return(0); // No se detecta la línea
}

/*
 * Función para obtener el sensor activo más a la izquierda
 */
int sensorIzquierda()
{
    if (lectura_CNYS[0] == 1) return(1); // Sensor más a la izquierda
    if (lectura_CNYS[1] == 1) return(2);
    if (lectura_CNYS[2] == 1) return(3);
    if (lectura_CNYS[3] == 1) return(4);
    if (lectura_CNYS[4] == 1) return(5);
    if (lectura_CNYS[5] == 1) return(6);
    if (lectura_CNYS[6] == 1) return(7);
    if (lectura_CNYS[7] == 1) return(8); // Sensor más a la derecha
    return(0); // No se detecta la línea
}

/*
 * Función para obtener el sensor activo más a la derecha de la parte central
 */
int sensorDerechaReducido()
{
    if (lectura_CNYS[4] == 1) return(5);
    if (lectura_CNYS[3] == 1) return(4);
    return(0); // No se detecta la línea
}

/*
 * Función para obtener el sensor activo más a la izquierda de la parte central
 */
int sensorIzquierdaReducido()
{
    if (lectura_CNYS[3] == 1) return(4);
    if (lectura_CNYS[4] == 1) return(5);
    return(0); // No se detecta la línea
}

/*
 * Función para detectar si se está leyendo negro en todos los sensores,
 * lo que podría indicar una marca de frenada en la pista
 */
void marcasFrenadaNegro()
{
    if (lectura_CNYS[0] == 1
        && lectura_CNYS[1] == 1
        && lectura_CNYS[2] == 1
        && lectura_CNYS[3] == 1
        && lectura_CNYS[4] == 1
        && lectura_CNYS[5] == 1
        && lectura_CNYS[6] == 1
        && lectura_CNYS[7] == 1)
        {
            contador_parada_negro++;
        }

}

/*
 * Función para detectar si se está leyendo blanco en todos los sensores,
 * lo que podría indicar una marca de frenada en la pista
 */
void marcasFrenadaBlanco()
{
    if (lectura_CNYS[0] == 0
        && lectura_CNYS[1] == 0
        && lectura_CNYS[2] == 0
        && lectura_CNYS[3] == 0
        && lectura_CNYS[4] == 0
        && lectura_CNYS[5] == 0
        && lectura_CNYS[6] == 0
        && lectura_CNYS[7] == 0)
        {
            contador_parada_blanco++;
        }
}

/*
 * Función para medir la diferencia en distancia entre las lecturas extremas
 */
byte anchoLectura(int sensor_mas_a_la_izquierda, int sensor_mas_a_la_derecha)
{
    byte ancho_lectura = sensor_mas_a_la_derecha - sensor_mas_a_la_izquierda;
    return (ancho_lectura);
}

/*
 * Función para comprobar el número de lineas detectadas a un momento dado
 * Lo que hace es restar la lectura de un sensor a la anterior buscando valores de -1
 */
byte comprobarNumeroLineas()
{
    byte numero_lineas = 0;

    if (lectura_CNYS[0] == 0) // Comparamos de izquierda a derecha
    {
        for (int i = 0; i < 7; i++)
        {
            if ((lectura_CNYS[i] - lectura_CNYS[i + 1]) == -1)
            {
                numero_lineas++;
            }
        }
    }

    if (lectura_CNYS[0] == 1) // Comparamos de derecha a izquierda
    {
        for (int i = 7; i > 0; i--)
        {
            if ((lectura_CNYS[i] - lectura_CNYS[i - 1]) == -1)
            {
                numero_lineas++;
            }
        }
    }

    return (numero_lineas); // Cero implica ninguna línea
                            // Uno implica una línea,
                            // Dos implica una línea y una marca
                            // Tres implica una línea y dos marcas
}

/*
 * Función para comprobar de qué lado está la marca
 */
TipoSentidoMarca comprobarLadoMarca()
{
    if ((lectura_CNYS[6] == 1 || lectura_CNYS[7] == 1)
        && (lectura_CNYS[0] == 1 || lectura_CNYS[1] == 1))
    {
        return(RECTO);
    }
    if (lectura_CNYS[6] == 1 || lectura_CNYS[7] == 1)
    {
        return(DERECHO); // Marca a la derecha
    }
    if (lectura_CNYS[0] == 1 || lectura_CNYS[1] == 1)
    {
        return(IZQUIERDO); // Marca a la izquierda
    }
    return NINGUNO;
}

/*
 * Comprobamos si estamos sobre una bifurcación
 */
byte deteccionBifurcacion()
{
    byte numero_lineas = comprobarNumeroLineas();

    if (numero_lineas == 1) return (false); // No se detecta bifurcación
    if (numero_lineas > 1)  return (true); // Se detecta bifurcación
    return EXIT_SUCCESS;
}

/*
 * Funciones para el cálculo de la dirección
 */
void calculoDireccionNormal() // Para dirección normal
{
    direccion = sensorDerecha() + sensorIzquierda();
}

void calculoDireccionDerecha()// Para bifurcación derecha
{
    direccion = 2 * sensorDerecha();
}

void calculoDireccionIzquierda()// Para bifurcación izquierda
{
    direccion = 2 * sensorIzquierda();
}

void calculoDireccionRecto()
{
    direccion = sensorDerechaReducido() + sensorIzquierdaReducido();
}

/*
 * Función para el control PD
 */
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
void actuacionMotores()
{
    int motor_derecho_pwm; // Señal PWM del motor derecho
    int motor_izquierdo_pwm; // Señal PWM del motor izquierda

    motor_derecho_pwm = VELOCIDAD_BASE - control_pwm; // Valor del motor derecho
    motor_izquierdo_pwm = VELOCIDAD_BASE + control_pwm; // Valor del motor izquierdo

    controlMotores (motor_izquierdo_pwm, motor_derecho_pwm); // Actuación de los motores
}

/*
 * Función para limpiar los valores de ciertas variables de control
 */
void limpiezaVariables()
{
    marca_real = 0; // Se borra la marca real
    bifurcacion = false; // Se borra la bifurcación
    sentido_marca = NINGUNO; // Se borra el lado de marca
    bifurcacion_pasada = false; // Se borra que se ha pasado la bifurcación
    contador_parada_negro = 0; // Se borran las marcas de parada del rastreador
    contador_parada_blanco = 0;
}
