#include <Adafruit_GFX.h>          // Librería para gráficos
#include <Adafruit_SSD1306.h>      // Librería para OLEDs basados en SSD1306

#define OLED_RESET 13              // Pin de reset para el OLED
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta, cambie en la librería de Adafruit_SSD1306.h!");
#endif

const int dataPins[4] = {8, 9, 10, 11}; // Pines de entrada para HT12D (D8 a D11)
const int pinVT = 12;                   // Pin para Valid Transmission (VT)
String primeros4Bits = "";
String segundos4Bits = "";
String mensaje = "";
bool mitadSuperior = true;              // Controla si estamos leyendo los primeros o segundos 4 bits
bool mensajeCompleto = false;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(dataPins[i], INPUT);
  }
  pinMode(pinVT, INPUT);                // Configurar VT como entrada
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicializar OLED en la dirección I2C 0x3C
  display.clearDisplay();
  display.display();
  Serial.println("Esperando mensaje...");
}

void loop() {
  // Verificar si hay transmisión válida mediante el pin VT
  if (digitalRead(pinVT) == HIGH) {
    if (!mensajeCompleto) {
      String bitsLeidos = leerBits();  // Leer 4 bits desde los pines

      if (mitadSuperior) {
        primeros4Bits = bitsLeidos;  // Guardar primeros 4 bits
        mitadSuperior = false;
      } else {
        segundos4Bits = bitsLeidos;  // Guardar segundos 4 bits
        String caracterBinario = primeros4Bits + segundos4Bits;

        if (caracterBinario.length() == 8) {
          // Validar y decodificar solo si es un carácter ASCII válido
          if (esCaracterASCIIValido(caracterBinario)) {
            char caracter = decodificarASCII(caracterBinario);
            Serial.print("Bits recibidos: ");
            Serial.println(caracterBinario);  // Mostrar los bits recibidos

            // Verificar si es el carácter de fin de mensaje
            if (caracter == '@') {
              mensajeCompleto = true;
              Serial.println("\n--- Fin del mensaje detectado ('@') ---");
              Serial.println("Mensaje completo:");
              Serial.println(mensaje);  // Mostrar el mensaje completo

              // Mostrar el mensaje completo en la pantalla OLED
              mostrarEnOLED(mensaje);

              // Preparar para recibir un nuevo mensaje
              reiniciarReceptor();
            } else {
              Serial.print("Caracter decodificado: ");
              Serial.println(caracter);  // Mostrar el carácter decodificado
              mensaje += caracter;       // Agregar al mensaje
            }
          }
        }
        mitadSuperior = true;  // Reiniciar para leer los siguientes bits
      }

      delay(300);  // Espera para sincronización
    }
  } else {
    // Si VT está en BAJO, esperar hasta que haya transmisión válida
    if (mensajeCompleto) {
      Serial.println("\nEsperando nueva transmisión...");
      mensajeCompleto = false;
    }
  }
}

// Lee 4 bits desde los pines D8-D11
String leerBits() {
  String bits = "";
  for (int i = 0; i < 4; i++) {
    bits += digitalRead(dataPins[i]) ? "1" : "0";
  }
  return bits;
}

// Convierte una cadena binaria de 8 bits a un carácter ASCII
char decodificarASCII(String bits) {
  int valor = strtol(bits.c_str(), nullptr, 2);  // Convertir binario a decimal
  return (char)valor;  // Retornar el carácter ASCII correspondiente
}

// Verifica si el binario recibido representa un carácter ASCII imprimible válido
bool esCaracterASCIIValido(String bits) {
  // Caracteres válidos:
  // - No deben comenzar con "0000" (caracteres de control no imprimibles)
  // - No deben comenzar con "1111" (fuera del rango ASCII estándar)
  return !(bits.startsWith("0000") || bits.startsWith("1111"));
}

// Muestra el mensaje completo en la pantalla OLED
void mostrarEnOLED(String mensaje) {
  display.clearDisplay();      // Borra el contenido anterior
  display.setTextSize(2);      // Establece el tamaño de texto
  display.setTextColor(WHITE); // Establece el color del texto
  display.setCursor(0, 10);    // Establece la posición inicial
  display.println(mensaje);    // Muestra el mensaje en el OLED
  display.display();           // Actualiza el OLED
}

// Reinicia el receptor para recibir un nuevo mensaje después de completarlo
void reiniciarReceptor() {
  mensaje = "";             // Vaciar el mensaje
  primeros4Bits = "";       // Reiniciar almacenamiento de bits
  segundos4Bits = "";
  mitadSuperior = true;     // Reiniciar estado de lectura
  mensajeCompleto = false;  // Listo para recibir un nuevo mensaje
}
