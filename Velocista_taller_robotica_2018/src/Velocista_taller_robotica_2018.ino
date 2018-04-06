/*
 * DECLACIÓN DE PINES Y COMPONENTES
 */
// LEDS y BOTONES
const int LED_1 = 10; // Led azul
const int LED_2 = 11; // Led rojo

const int BOTON_1 = 12; // Botón 1
const int BOTON_2 = 13; // Botón 2

// CNYS
const int CNY_1 = A7; // Izquierdo
const int CNY_2 = A6;
const int CNY_3 = A3;
const int CNY_4 = A2;
const int CNY_5 = A1;
const int CNY_6 = A0; // Derecho

// I2C
const int I2C_SDA = A4; // Central
const int I2C_SLC = A5; // Central

// ENCODERS
const int ENCODER_IZQUIERDO = 2;
const int ENCONDER_DERECHO = 3;

// MOTORES
const int PWM_MOTOR_IZQUIERDO = 6; // Motor izquierdo 1
const int ENTRADA_A_MOTOR_IZQUIERDO = 5;
const int ENTRADA_B_MOTOR_IZQUIERDO = 4;

const int PWM_MOTOR_DERECHO = 9; // Motor derecho 2
const int ENTRADA_A_MOTOR_DERECHO = 8;
const int ENTRADA_B_MOTOR_DERECHO = 7;

/*
 * LECTURA SENSORES
 */
bool lectura_CNYS[6]; // Lectura de los sensores para el controlador PD

/*
 *CONTROL PID
 */
const int REFERENCIA_DIRECCION = 7; // Dirección de referencia
int VELOCIDAD_BASE = 180; // Velocidad base

int KP = 30;            // Constante proporcional
int KI = 0;             // Constante integral
int KD = 255;           // Constante derivativa

int direccion;          // Dirección que se va a tomar en el PD
int direccion_anterior; // Dirección anterior de la lectura de los sensores activos

int error;              // La variable error es la parte proporcional
int error_anterior;     // La variable error_anterior es la parte derivativa

int control_pwm;        // Señal del PID para los motores


/*
 * MARCAS DE PARADA
 */
int contador_parada_negro = 0;  // Contador para las marcas de parada negras
int contador_parada_blanco = 0; // Contador para las marcas de parada blancas

int MARCA_PARADA_BLANCO = 1000; // Variable para el tiempo de parada en color blanco de la pista
int MARCA_PARADA_NEGRO = 500; // Variable para el tiempo de parada en color negro de la pista

/*
 * VARIABLES PARA CONTROL DEL TIEMPO
 */
unsigned long tiempo_comienzo = 0;
unsigned long tiempo_fin = 0;
unsigned int TIEMPO_PID = 3;
unsigned int TIEMPO_RESETEO_MARCAS = 5000;

/*
 * ESTADOS DE LA MÁQUINA DE ESTADOS FINITOS (FSM)
 */
enum TipoMaquinaEstadosFinitos
{
    REPOSO,
    CALIBRACION,
    SIGUELINEAS
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
int REFERENCIA_COLOR = 180; // Para indicar el umbral de diferencia entre blanco y negro
bool flag_salida = false; // Variable para indicar si el velocista está en movimiento

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

    // PINES DE SALIDA
    pinMode(LED_1, OUTPUT); // LED azul
    pinMode(LED_2, OUTPUT); // LED rojo

    pinMode(PWM_MOTOR_IZQUIERDO,OUTPUT); // Motor izquierdo 1
    pinMode(ENTRADA_A_MOTOR_IZQUIERDO,OUTPUT);
    pinMode(ENTRADA_B_MOTOR_IZQUIERDO,OUTPUT);

    pinMode(PWM_MOTOR_DERECHO,OUTPUT); // Motor derecho 2
    pinMode(ENTRADA_A_MOTOR_DERECHO,OUTPUT);
    pinMode(ENTRADA_B_MOTOR_DERECHO,OUTPUT);

    // INTERRUPCIONES DE ENCODERS
    //attachInterrupt(digitalPinToInterrupt(2),encoderIzquierdo, RISING);
    //attachInterrupt(digitalPinToInterrupt(3),encoderDerecho, RISING);

    //configurarBluethoot(); // Función para configurar el Bluethoot

    Serial.begin(9600); // Inicia comunicaciones serie a 9600 bps
}

/*
 * FUNCIÓN LOOP
 */
void loop()
{
    // Máquina de estados finitos
    switch (estado_finito)
    {
        /*
        * 1: Estado de reposo del rastreador
        */
        case TipoMaquinaEstadosFinitos::REPOSO:

            controlMotores (0, 0); // Motores parados
            flag_salida = false; // Para que al pasar al estado SIGUELÍNEAS comience la cuenta atrás

            // Se resetean los contadores de marcas de parada
            contador_parada_blanco = 0;
            contador_parada_negro = 0;

            digitalWrite(LED_1, LOW);
            digitalWrite(LED_2, HIGH); // Señal de estado REPOSO

            // Elección de estado SIGUELINEAS
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay (200);
                estado_finito = TipoMaquinaEstadosFinitos::SIGUELINEAS;
            }

            // Elección de estado CALIBRACION
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay (200);
                estado_finito = TipoMaquinaEstadosFinitos::CALIBRACION;
            }

            break;

            /*
            * 2: Estado de calibración de los sensores
            */
        case TipoMaquinaEstadosFinitos::CALIBRACION:

            digitalWrite(LED_1, HIGH); // Señal de estado CALIBRACION
            digitalWrite(LED_2, LOW);

            // Activado de la calibración
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay (200);
                REFERENCIA_COLOR = calibracionCNYS();
            }
            // Elección de estado SIGUELINEAS
            else if (digitalRead(BOTON_1) == HIGH)
            {
                delay (200);
                estado_finito = TipoMaquinaEstadosFinitos::SIGUELINEAS;
            }

            break;

        /*
         * 3: Estado normal de siguimiento de línea
         */
        case TipoMaquinaEstadosFinitos::SIGUELINEAS:

            // Si está a 0, realiza la cuenta atrás, si está a 1 la obvia
            if (flag_salida == false)
            {
                flag_salida = true;
                digitalWrite(LED_1, LOW);
                digitalWrite(LED_2, HIGH); // Señal de estado SIGUELINEAS

                for (byte i = 0; i < 5; i++)
                {
                    digitalWrite(LED_1, HIGH);
                    delay(TIEMPO_LED_SALIDA);
                    digitalWrite(LED_1, LOW);
                    delay(TIEMPO_LED_SALIDA);
                }
            }

            // Se comprueban las marcas de parada
            if (contador_parada_blanco > MARCA_PARADA_BLANCO || contador_parada_negro > MARCA_PARADA_NEGRO)
            {
                estado_finito = TipoMaquinaEstadosFinitos::REPOSO;
            }

            lecturaCnys(); // Lectura de los sensores

            marcasFrenada(); // Se comprueban si hay marcas de parada tras la lectura

            tiempo_comienzo = millis(); // Se mide el tiempo antes del PID

            // Se resetan los contadores de frenada si se sobrepasa un tiempo dado
            if (tiempo_comienzo - tiempo_fin >= TIEMPO_RESETEO_MARCAS)
            {
                contador_parada_negro = 0;
                contador_parada_blanco = 0;
            }

            // Se aplica el control PID si se sobrepasa un tiempo dado
            if (tiempo_comienzo - tiempo_fin >= TIEMPO_PID)
            {
                //Serial.println(millis());

                calculoDireccion(); // Cálculo de la dirección
                controlPD(); // Cálculo del PID
                actuacionMotores(control_pwm); // Actuación sobre los motores

                tiempo_fin = millis(); // Se mide el tiempo después del PID
            }

            // Elección de estado REPOSO
            if (digitalRead(BOTON_2) == HIGH)
            {
                delay (500);
                estado_finito = TipoMaquinaEstadosFinitos::REPOSO;
            }

            break;
    }

  //telemetria (); // Activamos la telemetría
}
