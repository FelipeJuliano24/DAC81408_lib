#include <Arduino.h>,
#include <DAC81408.h>

#define DAC_CS 10
#define DAC_RST 28
#define DAC_LDAC -1

DAC81408 dac(DAC_CS, DAC_RST, DAC_LDAC, &SPI, 30000000);

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("init");

  int res = dac.init();
  if (res == 0) Serial.println("DAC81404 init success");

  dac.set_int_reference(true);
  Serial.print("internal ref stat = ");
  Serial.println(dac.get_int_reference());

  dac.set_ch_enabled(1, true);
  dac.set_ch_enabled(2, true);
  dac.set_ch_enabled(3, true);
  dac.set_ch_enabled(4, true);
  dac.set_ch_enabled(5, true);


  for (int i = 1; i < 5; i++) {
    Serial.print("ch ");
    Serial.print(i);
    Serial.print(" power -> ");
    Serial.println(dac.get_ch_enabled(i));
  }
  Serial.println();

  dac.set_range(1, DAC81408::U_5); // 0 -> +10V
  dac.set_range(2, DAC81408::U_5); // 0 -> +10V
  dac.set_range(3, DAC81408::U_5); // 0 -> +10V
  dac.set_range(4, DAC81408::U_5); // 0 -> +10V
  dac.set_range(5, DAC81408::U_5); // 0 -> +10V


  dac.set_out(1, 10);
}

uint16_t i = 0;
bool done = true;

void loop() {
  
  dac.set_out(0, (i)%65535 );
  dac.set_out(1, (2*i)%65535 );
  dac.set_out(2, (3*i)%65535 );
  dac.set_out(3, (4*i)%65535);

  i = (i+64)%65535;  
  delayMicroseconds(1);

}
