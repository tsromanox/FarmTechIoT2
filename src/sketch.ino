#include "DHT.h"

#define DHTPIN 4       // Pino de dados do DHT22
#define DHTTYPE DHT22  // Tipo do sensor DHT

#define PHOSPHORUS_PIN 16
#define POTASSIUM_PIN 17
#define LDR_PIN 34       // Pino ADC para o LDR (pH)
#define RELAY_PIN 5
#define LED_PIN 2        // LED de status da bomba

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(PHOSPHORUS_PIN, INPUT_PULLUP); // Ou INPUT_PULLDOWN
  pinMode(POTASSIUM_PIN, INPUT_PULLUP);  // Ou INPUT_PULLDOWN
  // LDR_PIN é ADC, não precisa de pinMode para input analógico
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW); // Bomba desligada inicialmente
  digitalWrite(LED_PIN, LOW);   // LED desligado inicialmente
  Serial.println("Sistema de Irrigacao Inteligente FarmTech Solutions Inicializado");
}



void loop() {
  // Leitura do Sensor de Umidade (DHT22)
  float humidity = dht.readHumidity();
  // float temperature = dht.readTemperature(); // O DHT22 também lê temperatura

  // Simulação da leitura de Fósforo (P)
  bool phosphorus_present = digitalRead(PHOSPHORUS_PIN) == HIGH;

  // Simulação da leitura de Potássio (K)
  bool potassium_present = digitalRead(POTASSIUM_PIN) == HIGH;

  // Leitura do LDR e mapeamento para pH (Exemplo de Mapeamento)
  int ldr_value = analogRead(LDR_PIN);
  // O LDR no Wokwi geralmente varia de 0 (escuro) a 4095 (muito claro) no ESP32 ADC
  // Este mapeamento é apenas um EXEMPLO e precisa ser ajustado/calibrado
  // Suponha que LDR baixo (escuro) = pH ácido (e.g. 4) e LDR alto (claro) = pH alcalino (e.g. 9)
  // Para uma escala de 0-14, você precisaria de uma faixa mais ampla ou uma lógica diferente.
  // Por simplicidade, vamos mapear para uma faixa menor, ex: 4.0 a 9.0
  float ph_value = map(ldr_value, 0, 4095, 40, 90) / 10.0; // Mapeia para 4.0 - 9.0

  // Validação das leituras (DHT22 pode retornar NaN)
  if (isnan(humidity)) {
    Serial.println("Falha ao ler do sensor DHT!");
  } else {
    // Exibir dados no Monitor Serial (para cópia manual)
    Serial.print("Umidade: "); Serial.print(humidity); Serial.print(" %");
    Serial.print(" | pH: "); Serial.print(ph_value, 1); // 1 casa decimal
    Serial.print(" | Fosforo (P): "); Serial.print(phosphorus_present ? "Presente" : "Ausente");
    Serial.print(" | Potassio (K): "); Serial.print(potassium_present ? "Presente" : "Ausente");

    // Lógica de Controle da Bomba
    // Exemplo simples: Ligar a bomba se a umidade for menor que 60%
    bool activate_pump = false;
    if (humidity < 60.0 && humidity > 0) { // Adicionado humidity > 0 para evitar ligar com NaN
        activate_pump = true;
    }

    // Outras condições de ativação da bomba
    // if (temperature > 30.0) activate_pump = true; // Exemplo: temperatura alta
    // if (humidity < 50.0) activate_pump = true; // Exemplo: umidade baixa
    // if (potassium_present) activate_pump = true; // Exemplo: Potássio presente
    // if (ph_value < 6.0) activate_pump = true; // Exemplo: pH ácido
    // if (ph_value > 7.5) activate_pump = true; // Exemplo: pH alcalino
    // if (ph_value < 5.0 || ph_value > 8.0) activate_pump = false; // Não irrigar se pH extremo
    // if (!phosphorus_present) activate_pump = false; // Não irrigar se faltar fósforo (exemplo)

    if (activate_pump) {
      digitalWrite(RELAY_PIN, HIGH); // Ligar bomba
      digitalWrite(LED_PIN, HIGH);   // Ligar LED
      Serial.println(" | Bomba: LIGADA");
    } else {
      digitalWrite(RELAY_PIN, LOW);  // Desligar bomba
      digitalWrite(LED_PIN, LOW);    // Desligar LED
      Serial.println(" | Bomba: DESLIGADA");
    }
  }
  delay(5000); // Intervalo entre leituras
}