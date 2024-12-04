// Pines de salida para HT12E (D8 a D11)
const int dataPins[4] = {8, 9, 10, 11};
const int tePin = 12; // Pin TE (Enable Transmission)

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], OUTPUT);
  }
  pinMode(tePin, OUTPUT);
  digitalWrite(tePin, HIGH); // TE desactivado inicialmente
  Serial.println("Introduce un mensaje para enviar:");
}

void loop() {
  if (Serial.available()) {
    String mensaje = Serial.readStringUntil('\n'); // Leer mensaje completo
    enviarMensaje(mensaje + "@"); // Agregar el marcador '@' al final del mensaje
  }
}

void enviarMensaje(String mensaje) {
  for (int i = 0; i < mensaje.length(); i++) {
    char caracter = mensaje[i];
    String binario = convertirABinario(caracter);

    // Enviar los primeros 4 bits

    enviarMitad(binario.substring(0, 4));
    delay(300);

    // Enviar los segundos 4 bits
    enviarMitad(binario.substring(4, 8));
    delay(300);
  }
}

String convertirABinario(char caracter) {
  String binario = "";
  for (int i = 7; i >= 0; i--) {
    binario += ((caracter >> i) & 1) ? "1" : "0";
  }
  return binario;
}

void enviarMitad(String bits) {
  digitalWrite(tePin, LOW); // Activar transmisión
  for (int i = 0; i < 4; i++) {
    digitalWrite(dataPins[i], bits[i] == '1' ? HIGH : LOW);
  }
  delay(150); // Pequeño retraso para asegurar la transmisión
  digitalWrite(tePin, HIGH); // Desactivar transmisión
}