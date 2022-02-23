<<<<<<< HEAD
#include "mbed.h"
#include <AS5048A.h>
#include "TrigonometricTable.h"
As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX,1000000); // tx, rx
Timer timer;
int timeT;
int s,c;
int i = 0;
int main() {
      timer.start();
      sensor.frequency(8000000);
      while(1) {
            timer.reset();
            int angle = *sensor.read_angle();
            if(sensor.parity_check(angle)) {
                  int degrees = As5048Spi::degrees(angle)/100;
                  timeT = timer.read_us();
                  pc.printf("%i,%d\r\n",degrees,timeT);
            }else{
                  pc.printf("Parity check failed.\r\n");
            }
            wait_ms(1);
      }
}
=======
#include <Arduino.h>
#include <SimpleFOC.h>

// MagneticSensorSPI(int cs, float _cpr, int _angle_register)
// config           - SPI config
//  cs              - SPI chip select pin
MagneticSensorSPI sensor = MagneticSensorSPI(AS5048_SPI, PA15);

// these are valid pins (mosi, miso, sclk) for 2nd SPI bus on storm32 board (stm32f107rc)
SPIClass SPI_2(D11, D12, D13);

void setup() {
      // monitoring port
      Serial.begin(1000000);

      // initialise magnetic sensor hardware
      sensor.init(&SPI_2);

      Serial.println("Sensor ready");
      _delay(1000);
}

void loop() {
      // iterative function updating the sensor internal variables
      // it is usually called in motor.loopFOC()
      // this function reads the sensor hardware and
      // has to be called before getAngle nad getVelocity
      sensor.update();
      // display the angle and the angular velocity to the terminal
      Serial.print(sensor.getAngle());
      Serial.print("\t");
      Serial.println(sensor.getVelocity());
}
>>>>>>> parent of 296145a... FOCで回してみるやつをやってみた
