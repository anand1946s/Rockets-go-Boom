#include "sdcard.h"
#include "config.h"
#include <SPI.h>
#include <SD.h>




bool initSD(){
    if(!SD.begin(SD_CS)){
        return false;
    }
     
     return true;
    
}

bool logData(const String& data){
    if(datafile){
        datafile.println(data);
        datafile.close();
        return true;
    }
    else{
        
        return false;
    }
}

