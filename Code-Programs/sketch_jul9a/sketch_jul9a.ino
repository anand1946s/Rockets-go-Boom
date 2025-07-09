#include <Wire.h> //  I2C 
//for MPU6050 if Ad0 is high I2C address is 0x69 if low 0x68
const int MPU_ADDR=0x68;
int16_t acce_x,acce_y,acce_z;//raw
int16_t gyro_x,gyro_y,gyro_z;//raw
int16_t temperature;

int buzzer=10;
int samples=1000;

 float ax_MS2=0; 
  float ay_MS2=0; 
   float az_MS2=0; 

   float gx_dps=0;
    float gy_dps=0; 
     float gz_dps=0;

unsigned long prevmillis;
unsigned long interval_1=500;
long accsum_calx=0;
long accsum_caly=0;
long accsum_calz=0;
long gyrsum_calx=0;
long gyrsum_caly=0;
long gyrsum_calz=0;
float accoffset_calx=0;
float accoffset_caly=0;
float accoffset_calz=0;
float gyroffset_calx=0;
float gyroffset_caly=0;
float gyroffset_calz=0;

char tmp_str[7];


char* convert_int16_to_str(int16_t i)
{
  sprintf(tmp_str,"%6d",i);
  return tmp_str;
}

void setup() {
 Serial.begin(9600);
 Wire.begin();
 Wire.beginTransmission(MPU_ADDR);//Begin a transmission to the I2C slave 
 Wire.write(0x6B); //PWR_MGMT_1 registor
 Wire.write(0); //set to zero (wakes up the MPU6050)
 Wire.endTransmission(true);
 pinMode(buzzer,OUTPUT);

for(int i=0;i<samples;i++)
{
   Wire.beginTransmission(MPU_ADDR);
 Wire.write(0x3B); //starting with register 0x3B(ACCL_XOUT_H)
 Wire.endTransmission(false);/*The parameter indicates that the arduino will send a restart.
  As a result , the connection is kept active*/
  Wire.requestFrom(MPU_ADDR,7*2,true); // request a total of 7*2=14 register

    accsum_calx+=Wire.read()<<8 | Wire.read(); 
    accsum_caly+=Wire.read()<<8 | Wire.read(); 
    accsum_calz+=Wire.read()<<8 | Wire.read(); 
    Wire.read(); Wire.read(); // read and dont mind about the temperature bytes
    gyrsum_calx += Wire.read()<<8 | Wire.read(); 
    gyrsum_caly += Wire.read()<<8 | Wire.read(); 
    gyrsum_calz += Wire.read()<<8 | Wire.read(); 

}

accoffset_calx=(accsum_calx/samples) - 16384.0;
accoffset_caly=accsum_caly/samples;
accoffset_calz=accsum_calz/samples;
gyroffset_calx=gyrsum_calx/samples;
gyroffset_caly=gyrsum_caly/samples;
gyroffset_calz=gyrsum_calz/samples;

  Serial.println("Calibration complete!");
  Serial.print("Accel offsets (raw): ");
  Serial.print(accoffset_calx); Serial.print(", ");
  Serial.print(accoffset_caly); Serial.print(", ");
  Serial.println(accoffset_calz);
  Serial.print("Gyro offsets (raw): ");
  Serial.print(gyroffset_calx); Serial.print(", ");
  Serial.print(gyroffset_caly); Serial.print(", ");
  Serial.println(gyroffset_calz);

}

void loop() {

 Wire.beginTransmission(MPU_ADDR);
 Wire.write(0x3B); //starting with register 0x3B(ACCL_XOUT_H)
 Wire.endTransmission(false);/*The parameter indicates that the arduino will send a restart.
  As a result , the connection is kept active*/
  Wire.requestFrom(MPU_ADDR,7*2,true); // request a total of 7*2=14 register

  //wire.read()<< 8 | wire.read(); means tworegisters are read and stored in the same variable
  acce_x=Wire.read()<<8 | Wire.read(); //reading registers :0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    acce_y=Wire.read()<<8 | Wire.read(); //reading registers :0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
      acce_z=Wire.read()<<8 | Wire.read(); //reading registers :0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)

  temperature=Wire.read()<<8 | Wire.read(); //reading registers :0x41(Temp_out_H) and 0x42(temp_out_L)

  gyro_x = Wire.read()<<8 | Wire.read(); //reading registers:0x43(GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    gyro_y = Wire.read()<<8 | Wire.read(); //reading registers:0x45(GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
      gyro_z = Wire.read()<<8 | Wire.read(); //reading registers:0x47(GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
   



   
if( millis() - prevmillis >= interval_1 )
{
  prevmillis=millis();
 

   /*Serial.print("Acc_X :");Serial.print(convert_int16_to_str(acce_x));Serial.print("\t");
    Serial.print("Acc_Y :");Serial.print(convert_int16_to_str(acce_y));Serial.print("\t");
     Serial.print("Acc_Z :");Serial.print(convert_int16_to_str(acce_z));Serial.print("\t");
    */
    float acce_x_cal = acce_x - accoffset_calx;
       float acce_y_cal = acce_y - accoffset_caly;
          float acce_z_cal = acce_z - accoffset_calz;
          
             float gyro_x_cal = gyro_x - gyroffset_calx;
                float gyro_y_cal = gyro_y - gyroffset_caly;
                   float gyro_z_cal = gyro_z - gyroffset_calz;


                   ax_MS2 = acce_x_cal / 16384.0 * 9.80665;
                   ay_MS2 = acce_y_cal / 16384.0 * 9.80665;   
                   az_MS2 = acce_z_cal / 16384.0 * 9.80665;

                   
                    gx_dps = gyro_x_cal / 131.0;
                    gy_dps = gyro_y_cal / 131.0; 
                    gz_dps = gyro_z_cal / 131.0;   


                   if(ax_MS2>14) { digitalWrite(10,HIGH); }    
                   else { digitalWrite(10,LOW); }               
                      
Serial.print("ACC_X_MS2 :"); Serial.print(ax_MS2); Serial.print("\t");
Serial.print("ACC_Y_MS2 :"); Serial.print(ay_MS2); Serial.print("\t");
Serial.print("ACC_Z_MS2 :"); Serial.print(az_MS2); Serial.print("\t");
Serial.print("\n");

Serial.print("GYRO_X_DEG :"); Serial.print(gx_dps); Serial.print("\t");
Serial.print("GYRO_Y_DEG :"); Serial.print(gy_dps); Serial.print("\t");
Serial.print("GYRO_Z_DEG :"); Serial.print(gz_dps); Serial.print("\t");
Serial.print("\n");

Serial.print("Temp(C): "); Serial.print((temperature / 340.0) + 36.53); Serial.print("\n");

/*
        Serial.print("Gyro_X :");Serial.print(convert_int16_to_str(gyro_x));Serial.print("\t");
         Serial.print("Gyro_Y :");Serial.print(convert_int16_to_str(gyro_y));Serial.print("\t");
          Serial.print("Gyro_Z :");Serial.print(convert_int16_to_str(gyro_z));Serial.print("\t");

          Serial.println();
  */        
}      
}