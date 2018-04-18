/*
 * DECLACIÓN DE PINES Y COMPONENTES DEL RASTREADOR
 */
// LEDS y BOTONES
const int LED_1 = 12; // Led azul
const int LED_2 = 11; // Led verde
const int LED_3 = 10; // Led rojo

const int BOTON_1 = 2; // Botón 1
const int BOTON_2 = 9; // Botón 2

// CNYS
const int CNY_1 = A7; // Izquierdo
const int CNY_2 = A6;
const int CNY_3 = A5;
const int CNY_4 = A4; // Central
const int CNY_5 = A3; // Central
const int CNY_6 = A2;
const int CNY_7 = A1;
const int CNY_8 = A0; // Derecho

// MOTORES
const int PWM_MOTOR_IZQUIERDO = 5; // Motor izquierdo 1 cable azul hacia atrás
const int ENTRADA_A_MOTOR_IZQUIERDO = 3;
const int ENTRADA_B_MOTOR_IZQUIERDO = 4;

const int PWM_MOTOR_DERECHO = 6; // Motor derecho 2 cable azul hacia atrás
const int ENTRADA_A_MOTOR_DERECHO = 8;
const int ENTRADA_B_MOTOR_DERECHO = 7;

/*
 * LECTURA SENSORES
 */
bool lectura_CNYS[8]; // Lectura de los sensores para el controlador PD

/*
 *CONTROL PID
 */
int VELOCIDAD_BASE = 70; // Velocidad base
const int REFERENCIA_DIRECCION = 9; // Dirección de referencia

int KP = 15; // Constante proporcional 15
int KI = 0;  // Constante integral
int KD = 0; // Constante derivativa

int direccion; // Dirección que se va a tomar en el PD
int direccion_anterior; // Dirección anterior de la lectura de los sensores activos

int error = 0; // La variable error es la parte proporcional
int error_anterior = 0; // La variable error_anterior es la parte derivativa

int control_pwm; // Señal del PID para los motores

/*
 * MARCAS DE PARADA
 */
int contador_parada_blanco = 0; // Variable para parar el rastreador si lleva mucho tiempo sin leer nada
int contador_parada_negro = 0; // Variable para parar el rastreador si lleva mucho tiempo en una marca de parada

const int MARCA_PARADA_BLANCO = 800; // Variable para el tiempo de parada en color blanco de la pista
const int MARCA_PARADA_NEGRO = 350;  // Variable para el tiempo de parada en color negro de la pista

/*
 * COMPROBACIÓN DE LÍNEAS Y MARCAS DE GIRO
 */
byte numero_lineas; // Número de líneas detectadas en un momento dado

int marca_real; // Valor mínimo para aceptar una marca como real y no falso positivo
const int MARCA_MINIMA = 50; // Veces mínimas a detectar la marca para que sea real


enum TipoSentidoMarca
{
    DERECHO,
    IZQUIERDO,
    RECTO,
    NINGUNO
};

TipoSentidoMarca sentido_marca = NINGUNO;

/*
 * COMPROBACIÓN DE BIFURCACIONES Y ANCHOS DE LECTURA
 */
bool bifurcacion = false; // Para detectar una bifurcación
bool bifurcacion_pasada = false; // Para indicar que se ha pasado la bifurcación

byte ancho_lectura; // Variable que mide el ancho de lectura en distancia
                    // entre sensores más extremos de cada lado
                    // Sirve para evitar volver a SIGUELINEAS sin haber salido de
                    // la bifurcación


/*
 * VARIABLES PARA CONTROL DEL TIEMPO
 */
unsigned long tiempo_ciclo = 0;
unsigned long tiempo_ciclo_anterior = 0;
unsigned int TIEMPO_PID = 1500;
unsigned long TIEMPO_RESETEO_MARCAS = 5000000; // Equivale a 5 segundos

/*
 * ESTADOS DE LA MÁQUINA DE ESTADOS FINITOS (FSM)
 */
enum TipoMaquinaEstadosFinitos
{
    REPOSO,
    CALIBRACION,
    SIGUELINEAS,
    ELECCION_CAMINO
};

TipoMaquinaEstadosFinitos estado_finito = REPOSO;

/*
 * VARIABLE PARA CONTROL DE SENTIDO DE GIRO DE LOS MOTORES
 */
enum TipoSentidoMotor
{
    ADELANTE,
    ATRAS,
    STOP
};

/*
 * OTRAS VARIABLES
 */
const int RETARDO_REGLAMENTARIO = 5000; // Tiempo de retardo reglamentario
const int TIEMPO_LED_SALIDA = RETARDO_REGLAMENTARIO / 10; // Intervalo de tiempo para encender/apagar led
int REFERENCIA_COLOR = 950; // Para indicar el umbral de diferencia entre blanco y negr
bool flag_salida = false; // Variable para indicar si el rastreador está en movimiento

/*
 * FUNCIÓN SETUP
 */
void setup()
{
    // PINES DE ENTRADA
    pinMode(BOTON_1, INPUT);
    pinMode(BOTON_2, INPUT);

    pinMode(CNY_1, INPUT);
    pinMode(CNY_2, INPUT);
    pinMode(CNY_3, INPUT);
    pinMode(CNY_4, INPUT);
    pinMode(CNY_5, INPUT);
    pinMode(CNY_6, INPUT);
    pinMode(CNY_7, INPUT);
    pinMode(CNY_8, INPUT);

      // PINES DE SALIDA
    pinMode(LED_1, OUTPUT); // LED azul
    pinMode(LED_2, OUTPUT); // LED verde
    pinMode(LED_3, OUTPUT); // LED rojo

    pinMode(PWM_MOTOR_IZQUIERDO,OUTPUT); // Motor izquierdo 1
    pinMode(ENTRADA_A_MOTOR_IZQUIERDO,OUTPUT);
    pinMode(ENTRADA_B_MOTOR_IZQUIERDO,OUTPUT);

    pinMode(PWM_MOTOR_DERECHO,OUTPUT); // Motor derecho 2
    pinMode(ENTRADA_A_MOTOR_DERECHO,OUTPUT);
    pinMode(ENTRADA_B_MOTOR_DERECHO,OUTPUT);

    //configurarBluethoot(); // Función para configurar el Bluethoot

    Serial.begin(9600); // Inicia comunicaciones serie a 9600 bps
    //Serial.begin(115200); // Inicia comunicaciones serie a 115200 bps;

    controlMotores(0, 0); // Motores parados en el setup
}

/*
 * FUNCIÓN LOOP //
 */
void loop()
{
    // Máquina de estados finitos
    switch (estado_finito)
    {
        /*
         * 1: Estado de reposo del rastreador
         */
        case REPOSO:

            controlMotores(0, 0); // Motores parados
            flag_salida = false; // Para que al pasar al estado SIGUELÍNEAS comience la cuenta atrás

            digitalWrite(LED_1, LOW);
            digitalWrite(LED_2, LOW);
            digitalWrite(LED_3, HIGH); // Señal de estado REPOSO

            // Elección de estado SIGUELINEAS
            if (digitalRead(BOTON_1) == HIGH)
            {
                delay(200);
                estado_finito = SIGUELINEAS;
            }
            // Elección de estado CALIBRACION
            else if (digitalRead(BOTON_2) == HIGH)
            {
                delay(200);
                estado_finito = CALIBRACION;
            }
            break;

        /*
         * 2: Estado de calibración de los sensores
         */
        case CALIBRACION:

            digitalWrite(LED_1, HIGH); // Señal de estado CALIBRACION
            digitalWrite(LED_2, LOW);
            digitalWrite(LED_3, LOW);

            // Activado de la calibración
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay(200);
                REFERENCIA_COLOR = calibracionCNYS();
            }
            // Elección de estado SIGUELINEAS
            else if (digitalRead(BOTON_1) == HIGH)
            {
                delay(200);
                estado_finito = SIGUELINEAS;
            }

            break;

        /*
         * 3: Estado normal de siguimiento de línea
         */
        case SIGUELINEAS:

            // Si está a 0, realiza la cuenta atrás, si está a 1 la obvia
            if (flag_salida == false)
            {
                flag_salida = true;
                digitalWrite(LED_1, LOW);
                digitalWrite(LED_2, HIGH);
                digitalWrite(LED_3, LOW); // Señal de estado SIGUELINEAS

                for (byte i = 0; i < 5; i++)
                {
                    digitalWrite(LED_1, HIGH);
                    delay(TIEMPO_LED_SALIDA);
                    digitalWrite(LED_1, LOW);
                    delay(TIEMPO_LED_SALIDA);
                }
            }

            tiempo_ciclo = micros(); // Se mide el tiempo antes del PID

            // Cada ciclo se ejecuta todo el proceso PID
            if (tiempo_ciclo - tiempo_ciclo_anterior >= TIEMPO_PID)
            {
                tiempo_ciclo_anterior = tiempo_ciclo; // Se sustituye el nuevo tiempo de ciclo

                lecturaCnys(); // Lectura de los sensores
                ancho_lectura = anchoLectura(sensorIzquierda(), sensorDerecha());

                marcasFrenadaNegro(); // Se comprueban si hay marcas de parada tras la lectura
                marcasFrenadaBlanco();

                // Se resetan los contadores de frenada si se sobrepasa un tiempo dado
                if (tiempo_ciclo - tiempo_ciclo_anterior >= TIEMPO_RESETEO_MARCAS)
                {
                    contador_parada_negro = 0;
                    contador_parada_blanco = 0;
                }

                // En caso de estar mucho tiempo sobre blanco total o negro total se detiene el vehículo
                if (contador_parada_blanco > MARCA_PARADA_BLANCO || contador_parada_negro > MARCA_PARADA_NEGRO)
                {
                    estado_finito = REPOSO;
                    limpiezaVariables();
                }

                numero_lineas = comprobarNumeroLineas(); // Comprobamos el número de líneas detectadas

                // Cambiamos de estado finito si terminamos de leer la marca
                // y se vuelve a tener sólo una línea
                if ((marca_real > MARCA_MINIMA) && (numero_lineas == 1 && ancho_lectura <= 1))
                {
                    estado_finito = ELECCION_CAMINO;
                    break;
                }

                // Se va comprobando si hay marcas y/o falsos positivos
                if (numero_lineas <= 1 && ancho_lectura <= 1) // Parece que no hay marcas
                {
                    marca_real = 0;
                    digitalWrite(LED_1, LOW);
                    digitalWrite(LED_3, LOW);

                    calculoDireccionNormal(); // Cálculo de la dirección actual
                    controlPD(); // Cálculo del PID
                    actuacionMotores(); // Actuación sobre los motores
                }

                if (numero_lineas > 1 || ancho_lectura >= 3)  // Parece que hay una marca
                {
                    // Se espera para asegurar una lectura correcta del lado de la marca
                    if (marca_real > 49 && marca_real < 51)
                    {
                        sentido_marca = comprobarLadoMarca(); // Comprobación del lado de la marca

                        // Se marca con leds el lado de la marca
                        if (sentido_marca == DERECHO)
                        {
                            digitalWrite(LED_1, HIGH);
                        }
                        if (sentido_marca == IZQUIERDO)
                        {
                            digitalWrite(LED_3, HIGH);
                        }
                        if (sentido_marca == RECTO)
                        {
                            digitalWrite(LED_1, HIGH);
                            digitalWrite(LED_3, HIGH);
                        }
                    }

                    marca_real++; // Contador para asegurar que es una marca real

                    calculoDireccionRecto();
                    controlPD(); // Cálculo del PID
                    actuacionMotores(); // Actuación sobre los motores
                }
            }

            // Elección de estado REPOSO
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay(500);
                estado_finito = REPOSO;
                limpiezaVariables();
            }

            break;

        /*
         * 4: Estado de elección de camino en bifurcación
         */
        case ELECCION_CAMINO:

            tiempo_ciclo = micros(); // Se mide el tiempo antes del PID

            // Cada ciclo se ejecuta todo el proceso PID
            if (tiempo_ciclo - tiempo_ciclo_anterior >= TIEMPO_PID)
            {
                tiempo_ciclo_anterior = tiempo_ciclo; // Se sustituye el nuevo tiempo de ciclo

                // Se resetan los contadores de frenada si se sobrepasa un tiempo dado
                if (tiempo_ciclo - tiempo_ciclo_anterior >= TIEMPO_RESETEO_MARCAS)
                {
                    contador_parada_blanco = 0;
                }

                // En caso de estar mucho tiempo sobre blanco total se detiene el vehículo
                if (contador_parada_blanco > MARCA_PARADA_BLANCO)
                {
                    estado_finito = REPOSO;
                    limpiezaVariables();
                }

                lecturaCnys(); // Lectura de los sensores
                marcasFrenadaBlanco(); // Se comprueban si hay marcas de parada tras la lectura
                ancho_lectura = anchoLectura(sensorIzquierda(), sensorDerecha());

                bifurcacion = deteccionBifurcacion();

                if (bifurcacion == false && bifurcacion_pasada == false && ancho_lectura <= 1)
                {
                    calculoDireccionNormal(); // Cálculo de la dirección actual
                    controlPD(); // Cálculo del PID
                    actuacionMotores(); // Actuación sobre los motores
                }

                if (bifurcacion == true || ancho_lectura >= 3)
                {
                    // Para indicar la detección de la bifurcación de forma visual
                    digitalWrite(LED_1, LOW);
                    digitalWrite(LED_2, LOW);
                    digitalWrite(LED_3, LOW);

                    if (sentido_marca == DERECHO) // Marca a la derecha
                    {
                        calculoDireccionDerecha(); // Sólo se lee por la derecha
                    }

                    if (sentido_marca == IZQUIERDO) // Marca a la izquierda
                    {
                        calculoDireccionIzquierda(); // Sólo se lee por la izquierda
                    }

                    if (sentido_marca == RECTO) // Marca a ambos lados
                    {
                        calculoDireccionRecto(); // Sólo se lee por el centro
                    }

                    controlPD(); // Cálculo del PID
                    actuacionMotores(); // Actuación sobre los motores
                    bifurcacion_pasada = true;
                }

                // Volver a SIGUELINEAS
                if (bifurcacion == false && bifurcacion_pasada == true && ancho_lectura <= 1)
                {
                    digitalWrite(LED_1, LOW);
                    digitalWrite(LED_2, HIGH);
                    digitalWrite(LED_3, LOW);
                    estado_finito = SIGUELINEAS;
                    limpiezaVariables();
                }
            }

            // Elección de estado REPOSO
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay(500);
                estado_finito = REPOSO;
                limpiezaVariables();
            }

            break;
    }

    //telemetria(); // Se activa la telemetría
}
