const int dataPins[4] = {8, 9, 10, 11};  
const int tePin = 12;                    
const int DESPLAZAMIENTO = 3;            

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], OUTPUT);
  }
  pinMode(tePin, OUTPUT);
  digitalWrite(tePin, HIGH);
  Serial.println("Introduce un mensaje para enviar:");
}

void loop() {
  if (Serial.available()) {
    String mensaje = Serial.readStringUntil('\n');
    enviarMensaje(cifrarMensaje(mensaje + "@"));  
  }
}

String cifrarMensaje(String mensaje) {
  String mensajeCifrado = "";
  for (int i = 0; i < mensaje.length(); i++) {
    char caracterCifrado = mensaje[i] + DESPLAZAMIENTO;
    mensajeCifrado += caracterCifrado;
  }
  return mensajeCifrado;
}

void enviarMensaje(String mensaje) {
  for (int i = 0; i < mensaje.length(); i++) {
    char caracter = mensaje[i];
    String binario = convertirABinario(caracter);
    enviarMitad(binario.substring(0, 4));
    delay(300);
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
  digitalWrite(tePin, LOW);
  for (int i = 0; i < 4; i++) {
    digitalWrite(dataPins[i], bits[i] == '1' ? HIGH : LOW);
  }
  delay(150);
  digitalWrite(tePin, HIGH);
}
