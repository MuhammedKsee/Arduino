#include <LiquidCrystal_I2C_AvrI2C.h>
 
//lcd isminde bir 16x2 ekran nesnesi oluşturduk
LiquidCrystal_I2C_AvrI2C lcd(0x27,16,2);  
 
void setup()
{
  lcd.begin(); //lcd'yi başlatıyoruz
  lcd.backlight(); //lcd arka ışığını açıyoruz.
  lcd.setCursor(0,0); //imleci 1.satır ilk karaktere getiriyoruz.
  lcd.print("Hello"); //İlk satıra yazalım
  lcd.setCursor(0,1); //imleci 2.satır ilk karaktere getiriyoruz.
  lcd.print("World"); //İlk satıra yazalım  
}
 
void loop()
{
}