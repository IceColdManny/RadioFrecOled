#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta, cambie en la librería de Adafruit_SSD1306.h!")
#endif
const int dataPins[4] = {8, 9, 10, 11};
const int pinVT = 12;
String primeros4Bits = "";
String segundos4Bits = "";
String mensaje = "";
bool mitadSuperior = true;
bool mensajeCompleto = false;
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], INPUT);
  }
  pinMode(pinVT, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  Serial.println("Esperando mensaje...");
}
void loop() {
  if (digitalRead(pinVT) == HIGH) {
    if (!mensajeCompleto) {
      String bitsLeidos = leerBits();
      if (mitadSuperior) {
        primeros4Bits = bitsLeidos;
        mitadSuperior = false;
      } else {
        segundos4Bits = bitsLeidos;
        String caracterBinario = primeros4Bits + segundos4Bits;
        if (caracterBinario.length() == 8) {
          if (esCaracterASCIIValido(caracterBinario)) {
            char caracter = decodificarASCII(caracterBinario);
            Serial.print("Bits recibidos: ");
            Serial.println(caracterBinario);
            if (caracter == '@') {
              mensajeCompleto = true;
              Serial.println("\n--- Fin del mensaje detectado ('@') ---");
              Serial.println("Mensaje completo:");
              Serial.println(mensaje);
              desplazarEnOLED(mensaje);
              reiniciarReceptor();
            } else {
              Serial.print("Caracter decodificado: ");
              Serial.println(caracter);
              mensaje += caracter;
            }
          }
        }
        mitadSuperior = true;
      }
      delay(300);
    }
  } else {
    if (mensajeCompleto) {
      Serial.println("\nEsperando nueva transmisión...");
      mensajeCompleto = false;
    }
  }
}
String leerBits() {
  String bits = "";
  for (int i = 0; i < 4; i++) {
    bits += digitalRead(dataPins[i]) ? "1" : "0";
  }
  return bits;
}
char decodificarASCII(String bits) {
  int valor = strtol(bits.c_str(), nullptr, 2);
  return (char)valor;
}
bool esCaracterASCIIValido(String bits) {
  return !(bits.startsWith("0000") || bits.startsWith("1111"));
}
void desplazarEnOLED(String mensaje) {
  int anchoTexto = mensaje.length() * 8;
  int x = 128;

  while (true) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(x, 15);
    display.print(mensaje);
    display.display();

    x--;
    if (x < -anchoTexto) {
      x = 128;
    }

    if (digitalRead(pinVT) == HIGH) {
      break;
    }
  }
}
void reiniciarReceptor() {
  mensaje = "";
  primeros4Bits = "";
  segundos4Bits = "";
  mitadSuperior = true;
  mensajeCompleto = false;
}
