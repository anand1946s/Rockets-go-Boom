void setup() {
  Serial.begin(115200);
}

void loop() {
  static unsigned long t = 0;
  t += 100;  // Time in milliseconds

  // Convert time to seconds
  float time = t / 1000.0;

  // Simulated dynamic angles (in degrees)
  float roll  = 15 * sin(2 * time);                           // side wobble
  float pitch = 10 * sin(1.5 * time) + 5 * cos(0.5 * time);   // forward/back tilt
  float yaw   = 30 * sin(0.8 * time);                         // spin

  // Add small random jitter occasionally
  if (random(0, 100) < 5) {
    roll  += random(-10, 11);  // between -10 and +10
    pitch += random(-5, 6);
    yaw   += random(-15, 16);
  }

  // Print in comma-separated format
  Serial.print(roll); Serial.print(",");
  Serial.print(pitch); Serial.print(",");
  Serial.println(yaw);

  delay(100); // ~10Hz update rate
}
