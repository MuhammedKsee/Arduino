int rpin = 11;
int gpin = 10;
int bpin = 9;

float h = 0;

int r = 0, g = 0, b = 0;

void setup() {
  // Put your setup code here, to run once:
  pinMode(rpin, OUTPUT);
  pinMode(gpin, OUTPUT);
  pinMode(bpin, OUTPUT);
}

void loop() {
  h += 0.01;
  if (h >= 1.0) {
    h = 0;
  }

  h2rgb(h, r, g, b);

  analogWrite(rpin, r);
  analogWrite(gpin, g);
  analogWrite(bpin, b);

  delay(5);
}

void h2rgb(float h, int& r, int& g, int& b) {
  float v = 1; // Value
  float s = 1; // Saturation
  float var1, var2, var3;
  float varr, varg, varb;

  if (s == 0) {
    r = g = b = v * 255;
  } else {
    float varh = h * 6;
    if (varh == 6) varh = 0;
    int vari = int(varh);
    var1 = v * (1 - s);
    var2 = v * (1 - s * (varh - vari));
    var3 = v * (1 - s * (1 - (varh - vari)));

    if (vari == 0) {
      varr = v;
      varg = var3;
      varb = var1;
    } else if (vari == 1) {
      varr = var2;
      varg = v;
      varb = var1;
    } else if (vari == 2) {
      varr = var1;
      varg = v;
      varb = var2;
    } else if (vari == 3) {
      varr = var1;
      varg = var2;
      varb = v;
    } else if (vari == 4) {
      varr = var2;
      varg = var1;
      varb = v;
    } else {
      varr = v;
      varg = var1;
      varb = var2;
    }

    r = varr * 255;
    g = varg * 255;
    b = varb * 255;
  }
}
