#define NORMAL_SPEED 120
#define MAX_SPEED 150
#define M_KANAN 7
#define M_KIRI 8
#define PWM_KANAN 6
#define PWM_KIRI 5
#define PING_KIRI A1
#define PING_DEPAN A0
#define PING_KANAN 15
#define SW 9
#define SERVO 10
#define PHOTO_KIRI 3
#define PHOTO_KANAN 2
#define LDR_KIRI  A2
#define LDR_KANAN A3
#define KANAN 1
#define KIRI 0
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
#include <Ultrasonic.h>
#include <Servo.h>

Ultrasonic pingKiri(PING_KIRI, PING_KIRI);
Ultrasonic pingKanan(PING_KANAN, PING_KANAN);
Ultrasonic pingDepan(PING_DEPAN, PING_DEPAN);

Servo perpanjangan;

float Kp = 10;
float Ki = 6;
float Kd = 12;
float error = 0;
float last_error = 0;
float output = 0;
float kecKiri = 0;
float kecKanan = 0;
int setPoint = 8; 

int jarakDepan = 0, jarak = 0;
int hitungDepan = 0;
int hitungGaris = 0;
int rentangWaktu = 400;
int kiriTerakhir = 0, kananTerakhir = 0;
unsigned long last_millis = 0;
bool mode = false;
bool kondisi = 0;



void motor(bool kiri, bool kanan, int pwmkiri, int pwmkanan) {
  analogWrite(PWM_KIRI, pwmkiri);
  analogWrite(PWM_KANAN, pwmkanan);
  digitalWrite(M_KIRI, kiri);
  digitalWrite(M_KANAN, kanan);
}


void cekGaris(bool mode) {
  bool kiri = 0, kanan = 0;
  if (mode == KIRI) {
    kiri = digitalRead(PHOTO_KIRI);
    if (kiri == LOW && kiriTerakhir == HIGH && (unsigned long) (millis() - last_millis) > 200) {
      last_millis = millis();
    }
    if ((unsigned long) (millis() - last_millis) > 50) {
      if (kiri == HIGH && kiriTerakhir == LOW) {
        hitungGaris++;
      }
    }

    kiriTerakhir = kiri;

  }
  else {
    kanan = digitalRead(PHOTO_KANAN);
    if (kanan == LOW && kananTerakhir == HIGH && (unsigned long) (millis() - last_millis) > 200) {
      last_millis = millis();
    }
    if ((unsigned long) (millis() - last_millis) > 50) {
      if (kanan == HIGH && kananTerakhir == LOW) {
        hitungGaris++;
      }
    }
    kananTerakhir = kanan;
  }
}


void cekDepan() {
  jarakDepan = pingDepan.read();
  if (jarakDepan <= 20) {
    hitungDepan++;
    motor(0, 0, 120, 120);
    delay(50);
    motor(0, 0, 0, 0);
    while (true) {
      jarakDepan = pingDepan.read();
      if (jarakDepan > 30) {
        delay(2000);
        break;
      }
      else {
        motor(0, 0, 0, 0);
      }
    }
  }
  if (hitungDepan == 1 && kondisi == 0) {
    if (mode == KIRI) {
      while (true) {
        cekGaris(KIRI);
        if (hitungGaris == 2) {
          motor(0, 0, 120, 120);
          delay(100);
          motor(0, 0, 0, 0);
          while (true) {
           perpanjangan.write(20);
            delay(500);
            perpanjangan.write(101);
            delay(2000);
            int ldr_kiri = 0;
            while(true){
             ldr_kiri = analogRead(LDR_KIRI);
             if(ldr_kiri > rentangWaktu){
              perpanjangan.write(160);
              delay(500);
              perpanjangan.write(101);
              break;
             }
            }
            delay(2000);
            break;
          }
          hitungGaris = 0;
          kondisi = 1;
          do_ping(KIRI);
          break;
        }
        jarak = pingKiri.read();
        if (jarak >= 100 || jarak <= 0) jarak = 100;
        error = setPoint - jarak;
        output = Kp * error + Ki * (error + last_error) + Kd * (error - last_error);
        last_error = error;
        kecKanan = 128 - output;
        kecKiri = 128 + output;
        kecKanan = constrain(kecKanan, 0, 130);
        kecKiri = constrain(kecKiri, 0, 130);

        motor(1, 1, kecKiri, kecKanan);
      }
    }
    else {
      while (true) {
        cekGaris(KANAN);
        if (hitungGaris == 2) {
          motor(0, 0, 120, 120);
          delay(100);
          motor(0, 0, 0, 0);
          while (true) {
            perpanjangan.write(160);
            delay(500);
            perpanjangan.write(101);
            delay(2000);
            int ldr_kanan = 0;
            while(true){
             ldr_kanan = analogRead(LDR_KANAN);
             if(ldr_kanan > rentangWaktu){
              perpanjangan.write(20);
              delay(500);
              perpanjangan.write(101);
              break;
             }
            }
            delay(2000);
            break;
          }
          hitungGaris = 0;
          kondisi = 1;
          do_ping(KANAN);
          break;
        }
        jarak = pingKanan.read();
        if (jarak >= 100 || jarak <= 0) jarak = 100;
        error = setPoint - jarak;
        output = Kp * error + Ki * (error + last_error) + Kd * (error - last_error);
        last_error = error;
        kecKanan = 128 + output;
        kecKiri = 128 - output;
        kecKanan = constrain(kecKanan, 0, 130);
        kecKiri = constrain(kecKiri, 0, 130);

        motor(1, 1, kecKiri, kecKanan);
      }
    }
  }
  if (hitungDepan == 2) {
    if (mode == KIRI) {
      while (true) {
        cekGaris(KIRI);
        if (hitungGaris == 1) {
          motor(0, 0, 120, 120);
          delay(100);
          motor(0, 0, 0, 0);
          while (true) {
            perpanjangan.write(20);
            delay(500);
            perpanjangan.write(101);
            delay(2000);
            int ldr_kiri = 0;
            while(true){
             ldr_kiri = analogRead(LDR_KIRI);
             if(ldr_kiri > rentangWaktu){
              perpanjangan.write(160);
              delay(500);
              perpanjangan.write(101);
              break;
             }
            }
            delay(2000);
            break;
          }
          hitungGaris = 0;
          kondisi = 0;
          hitungDepan = 0;
          do_ping(KIRI);
          break;
        }
        jarak = pingKiri.read();
        if (jarak >= 100 || jarak <= 0) jarak = 100;
        error = setPoint - jarak;
        output = Kp * error + Ki * (error + last_error) + Kd * (error - last_error);
        last_error = error;
        kecKanan = 128 - output;
        kecKiri = 128 + output;
        kecKanan = constrain(kecKanan, 0, 130);
        kecKiri = constrain(kecKiri, 0, 130);

        motor(1, 1, kecKiri, kecKanan);
      }
    }
    else {
      while (true) {
        cekGaris(KANAN);
        if (hitungGaris == 1) {
          motor(0, 0, 120, 120);
          delay(100);
          motor(0, 0, 0, 0);
          while (true) {
            perpanjangan.write(160);
            delay(500);
            perpanjangan.write(101);
            delay(2000);
            int ldr_kanan = 0;
            while(true){
             ldr_kanan = analogRead(LDR_KANAN);
             if(ldr_kanan > rentangWaktu){
              perpanjangan.write(20);
              delay(500);
              perpanjangan.write(101);
              break;
             }
            }
            delay(2000);
            break;
          }
          hitungGaris = 0;
          kondisi = 0;
          hitungDepan = 0;
          do_ping(KANAN);
          break;
        }
        jarak = pingKanan.read();
        if (jarak >= 100 || jarak <= 0) jarak = 100;
        error = setPoint - jarak;
        output = Kp * error + Ki * (error + last_error) + Kd * (error - last_error);
        last_error = error;
        kecKanan = 128 + output;
        kecKiri = 128 - output;
        kecKanan = constrain(kecKanan, 0, 130);
        kecKiri = constrain(kecKiri, 0, 130);

        motor(1, 1, kecKiri, kecKanan);
      }
    }
  }
}


void do_ping(bool mode) {
  (mode == KIRI) ? jarak = pingKiri.read() : jarak = pingKanan.read();
  if (jarak >= 100 || jarak <= 0) jarak = 100;
  error = setPoint - jarak;
  output = Kp * error + Ki * (error + last_error) + Kd * (error - last_error);
  last_error = error;
  if (mode == KIRI) {
    kecKanan = NORMAL_SPEED - output;
    kecKiri = NORMAL_SPEED + output;
  }
  else {
    kecKanan = NORMAL_SPEED + output;
    kecKiri = NORMAL_SPEED - output;
  }
  kecKanan = constrain(kecKanan, -MAX_SPEED, MAX_SPEED);
  kecKiri = constrain(kecKiri, -MAX_SPEED, MAX_SPEED);
}


void modeJalan(bool mode) {
  if (mode == KIRI) {
    if (kecKiri < 0) {
      while (true) {
        do_ping(KIRI);
        if (kecKiri < 0) {
          motor(0, 1, 30, 110);
        }
        else {
          motor(1, 1, kecKiri, kecKanan);
          break;
        }
      }
    }
    else {
      motor(1, 1, kecKiri, kecKanan);
    }
  }
  else {
    if (kecKanan < 0) {
      while (true) {
        do_ping(KANAN);
        if (kecKanan < 0) {
          motor(1, 0, 120, 20);
        }
        else {
          motor(1, 1, kecKiri, kecKanan);
          break;
        }
      }
    }
    else {
      motor(1, 1, kecKiri, kecKanan);
    }
  }
}


void setup() {
  pinMode(M_KANAN, OUTPUT);
  pinMode(M_KIRI, OUTPUT);
  pinMode(PWM_KANAN, OUTPUT);
  pinMode(PWM_KANAN, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(PHOTO_KIRI, INPUT_PULLUP);
  pinMode(PHOTO_KANAN, INPUT_PULLUP);
  perpanjangan.attach(SERVO);
  perpanjangan.write(20);
  delay(200);
  perpanjangan.write(101);
  motor(0, 0, 0, 0);
  mode = (bool) digitalRead(SW);
  delay(3000);
}


void loop() {
  cekDepan();
  if (mode == KANAN) {
    do_ping(KANAN);
    modeJalan(KANAN);
  }
  else {
    do_ping(KIRI);
    modeJalan(KIRI);
  }
  delay(50);
}
