// This one has the codes to capture an image every X seconds and store it.

// we start the image by getting the input signal from esp32
// input signal is simply a pin set to digitalread() or analog .the esp sends a HIGH signal and this one starts capturing
// also a reverse mechanism to give ff=eedback to esp32 that, this one has started work
// this module stars in INIT state and stops only after landing
// The stopping logic can be a simple time based one.
//like STOP WHEN TIME=180s etc.




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
