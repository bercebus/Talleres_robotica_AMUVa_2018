/*
 * Función para convertir una entrada analógica en digital
 */
byte analogicoDigital(int entrada_analogica)
{
    byte salida_digital;

    if (entrada_analogica < REFERENCIA_COLOR)
    {
        salida_digital = 0;
    }
    else if (entrada_analogica > REFERENCIA_COLOR)
    {
        salida_digital = 1;
    }

    return (salida_digital);
}

/*
 * Función de calibración de los sensores de suelo
 */
int calibracionCNYS()
{
    int media_alta = 0;
    int media_baja = 0;
    int media_final;
    int lectura_analogica_CNYS[6];

    lectura_analogica_CNYS[0] = analogRead(CNY_1);
    lectura_analogica_CNYS[1] = analogRead(CNY_2);
    lectura_analogica_CNYS[2] = analogRead(CNY_3);
    lectura_analogica_CNYS[3] = analogRead(CNY_4);
    lectura_analogica_CNYS[4] = analogRead(CNY_5);
    lectura_analogica_CNYS[5] = analogRead(CNY_6);

    for (byte i = 0; i < 6; i++)
    {
        media_alta = media_alta + lectura_analogica_CNYS[i];
    }

    media_alta = media_alta / 6;
    digitalWrite(LED_2, HIGH);

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

        for (byte i = 0; i < 6; i++)
        {
            media_baja = media_baja + lectura_analogica_CNYS[i];
        }

        media_baja = media_baja / 6;

        media_final = (media_alta + media_baja) / 2;

        digitalWrite(LED_2, HIGH);
        delay (500);

        return(media_final);
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
}
