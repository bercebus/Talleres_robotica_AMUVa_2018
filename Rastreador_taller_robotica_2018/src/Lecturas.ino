/*
 * Función para convertir una entrada analógica en digital
 */
bool analogicoDigital(int entrada_analogica)
{
    bool salida_digital;

    if (entrada_analogica < REFERENCIA_COLOR)
    {
        salida_digital = 1;
    }
    else if (entrada_analogica > REFERENCIA_COLOR)
    {
        salida_digital = 0;
    }

    return (salida_digital);
}

/*
 * Función de calibración de los sensores de suelo
 */
int calibracionCNYS()
{
    int media_alta = 0; // Color blanco en rastreador, color negro en velocista
    int media_baja = 0; // Color negro en rastreador, color blanco en velocista
    int media_final;
    int lectura_analogica_CNYS[8];

    lectura_analogica_CNYS[0] = analogRead(CNY_1);
    lectura_analogica_CNYS[1] = analogRead(CNY_2);
    lectura_analogica_CNYS[2] = analogRead(CNY_3);
    lectura_analogica_CNYS[3] = analogRead(CNY_4);
    lectura_analogica_CNYS[4] = analogRead(CNY_5);
    lectura_analogica_CNYS[5] = analogRead(CNY_6);
    lectura_analogica_CNYS[6] = analogRead(CNY_7);
    lectura_analogica_CNYS[7] = analogRead(CNY_8);

    for (byte i = 0; i < 8; i++)
    {
        media_alta = media_alta + lectura_analogica_CNYS[i];
    }

    media_alta = media_alta / 8;
    digitalWrite(LED_2, HIGH);

    // Se bloquea el rastreador hasta ponerlo en posición para medir negro
    while (digitalRead(BOTON_2) != HIGH)
    {}

    digitalWrite(LED_2, LOW);
    delay (500);

    lectura_analogica_CNYS[0] = analogRead(CNY_1);
    lectura_analogica_CNYS[1] = analogRead(CNY_2);
    lectura_analogica_CNYS[2] = analogRead(CNY_3);
    lectura_analogica_CNYS[3] = analogRead(CNY_4);
    lectura_analogica_CNYS[4] = analogRead(CNY_5);
    lectura_analogica_CNYS[5] = analogRead(CNY_6);
    lectura_analogica_CNYS[6] = analogRead(CNY_7);
    lectura_analogica_CNYS[7] = analogRead(CNY_8);

    for (byte i = 0; i < 8; i++) // 6 en velocista
    {
        media_baja = media_baja + lectura_analogica_CNYS[i];
    }

    media_baja = media_baja / 8;

    media_final = (media_alta + media_baja) / 2;

    return (media_final);

    digitalWrite(LED_2, HIGH);
    delay (500);
}

/*
 * Función de lectura de los sensores de suelo en un instante dado
 */
void lecturaCnys()
{
    lectura_CNYS[0] = analogicoDigital(analogRead(CNY_1));
    lectura_CNYS[1] = analogicoDigital(analogRead(CNY_2));
    lectura_CNYS[2] = analogicoDigital(analogRead(CNY_3));
    lectura_CNYS[3] = analogicoDigital(analogRead(CNY_4));
    lectura_CNYS[4] = analogicoDigital(analogRead(CNY_5));
    lectura_CNYS[5] = analogicoDigital(analogRead(CNY_6));
    lectura_CNYS[6] = analogicoDigital(analogRead(CNY_7));
    lectura_CNYS[7] = analogicoDigital(analogRead(CNY_8));
}
