void telemetria()
{

    Serial.print(lectura_CNYS[0]); Serial.print(",");
    Serial.print(lectura_CNYS[1]); Serial.print(",");
    Serial.print(lectura_CNYS[2]); Serial.print(",");
    Serial.print(lectura_CNYS[3]); Serial.print(",");
    Serial.print(lectura_CNYS[4]); Serial.print(",");
    Serial.print(lectura_CNYS[5]); Serial.print(",");
    Serial.print(lectura_CNYS[6]); Serial.print(",");
    Serial.println(lectura_CNYS[7]);

    delay(500);
}
