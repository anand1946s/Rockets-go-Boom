#include "sdcard.h"
#include "config.h"
#include<SPI.h>
#include <SD.h>

const int SD_CS =10;

File dataFile;

bool initSD(){
    if(!SD.begin(SD_CS)){
        return false;
    }
     
     return true;
    
}

bool logData(const String& data){
    dataFile = SD.open("data.csv",FILE_WRITE);
    if(dataFile){
        dataFile.println(data);
        dataFile.close();
        return true;
    }
    else{
        
        return false;
    }
}

