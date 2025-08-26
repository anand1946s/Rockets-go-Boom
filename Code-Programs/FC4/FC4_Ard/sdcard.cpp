#include "sdcard.h"
#include "config.h"
#include "imu.h"
#include "bmp180.h"
#include <SPI.h>
#include <SD.h>



static int flushCounter = 0;

bool initSD(){
    if(!SD.begin(SD_CS)){
        return false;
    }
     
     return true;
    
}

bool logData(const String& data){
    if(datafile){
        datafile.println(data);
        if (++flushCounter >= 20) { // flush every 20 samples
        datafile.flush();
        flushCounter = 0;
        }
        return true;
    }
    else{
        
        return false;
    }
}

