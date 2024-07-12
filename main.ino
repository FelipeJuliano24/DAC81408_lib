#include <Arduino.h>
#include <DAC81408.h>

#define DAC_CS 10
#define DAC_RST 28
#define DAC_LDAC -1

DAC81408 dac(DAC_CS, DAC_RST, DAC_LDAC, &SPI, 30000000);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("init");

  int res = dac.init();
  if (res == 0) Serial.println("DAC81408 init success");

  dac.set_int_reference(true);
  Serial.print("internal ref stat = ");
  Serial.println(dac.get_int_reference());

  for (int ch = 1; ch <= 8; ch++) {
    dac.set_ch_enabled(ch, true);
  }

  for (int ch = 0; ch <= 3; ch++) {
    dac.set_range(ch, DAC81408::U_10);
  }
}

uint16_t i = 0;

void loop() {
  dac.set_out(0, i % 65535);
  dac.set_out(1, (2 * i) % 65535);
  dac.set_out(2, (3 * i) % 65535);
  dac.set_out(3, (4 * i) % 65535);

  i = (i + 64) % 65535;
  delayMicroseconds(1);

  if (Serial.available() > 0) {  // Verifica se há dados disponíveis para leitura
    String dadosRecebidos = Serial.readStringUntil('\n');  // Lê os dados recebidos até encontrar um caractere de nova linha (\n)
    Serial.print("Dados recebidos: ");
    Serial.println(dadosRecebidos);  // Imprime os dados recebidos no monitor serial
  }
}

