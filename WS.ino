
/* 
   
    Weature Station
    
    by : Osama ElMorady

*/

/****************************************************/
/****             Include Headers                 ***/
/****************************************************/
#include <Arduino.h>
/*****************    OLED   ************************/
#include <U8x8lib.h>
/*****************    DHT   ************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
/*****************    BMP180   **********************/
#include <Adafruit_BMP085.h>
/****************    DS3231 RTC   *******************/
#include <RTC.h>
/*******************     CP    **********************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
/****************************************************/



/****************************************************/
/*****************    OLED   ************************/
#define OLED_CLK_PIN    12
#define OLED_DATA_PIN   11
#define OLED_RESET_PIN  8
#define OLED_DC_PIN     9
#define OLED_CS_PIN     10
/*****************    DHT   ************************/
#define DHTPIN    6          // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
/*****************    BMP180   **********************/
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
/*****************    UV   ************************/
#define UV_PIN    A0          // Analog pin connected to the UV sensor 
/****************************************************/



/****************************************************/
/*****************    OLED   ************************/
U8X8_SSD1322_NHD_256X64_4W_HW_SPI u8x8(/* cs=*/ OLED_CS_PIN, /* dc=*/ OLED_DC_PIN, /* reset=*/ OLED_RESET_PIN);   /*Fixed  MOSI = 11 (NANO) , SCK = 13 (NANO) */
//U8X8_SSD1322_NHD_256X64_4W_SW_SPI u8x8(/* clock=*/ OLED_CLK_PIN, /* data=*/ OLED_DATA_PIN, /* cs=*/ OLED_CS_PIN, /* dc=*/ OLED_DC_PIN, /* reset=*/ OLED_RESET_PIN);
/*****************    DHT   ************************/
DHT_Unified dht(DHTPIN, DHTTYPE);
sensor_t dht_sensor;
DHT dht_11(DHTPIN, DHTTYPE)  ;
/*****************    BMP180   **********************/
Adafruit_BMP085 bmp;
/*****************    UV   ************************/
//float uv_sensor_value = 0.0 ;
/*****************    RTC   **********************/
static DS3231 RTC;
/****************************************************/
static void Draw_Border(void) ;
static void utils_uitoa(uint32_t n, uint8_t s[]) ;
uint8_t arr[2] = {} ; ;



void setup(void)
{  
/*****************    Serial Monitor   ************************/  
  Serial.begin(9600);


/*****************    CP   ************************/
  Wire.begin();  


/*****************    OLED   ************************/
//  u8x8.begin();
  if (!u8x8.begin()) {
    Serial.println(F("OLED SSD1322 failed"));
  }
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
/*****************    DHT   ************************/
  dht.begin();

  sensors_event_t event;
  dht.temperature().getSensor(&dht_sensor);
  dht.humidity().getSensor(&dht_sensor);
  Serial.print("Temperature = ");     Serial.print(event.temperature)         ;     Serial.println(" *C");
  Serial.print("Humidity = ")   ;     Serial.print(event.relative_humidity)   ;     Serial.println(" Pa");
/*****************    BMP180   **********************/
//  bmp.begin() ;
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
  }

  
  Serial.print("Temperature = ");     Serial.print(bmp.readTemperature()) ;     Serial.println(" *C");
  Serial.print("Pressure = ")   ;     Serial.print(bmp.readPressure())    ;     Serial.println(" Pa");
  Serial.print("Altitude = ")   ;     Serial.print(bmp.readAltitude())    ;     Serial.println(" Pa");

/****************    DS3231 RTC   *******************/
  RTC.begin();
  //  RTC.begin() ;
  if (!RTC.begin()) {
    Serial.println("Could not find a valid DS3231 sensor, check wiring!");
  }

  Serial.print("Day"); Serial.print("  ");  Serial.print("Month"); Serial.print("  ");  Serial.println("Year");
  Serial.print(RTC.getDay()); Serial.print("-");  Serial.print(RTC.getMonth()); Serial.print("-");  Serial.println(RTC.getYear());
  Serial.print("Hour"); Serial.print("  ");  Serial.print("Minute"); Serial.print("  ");  Serial.println("Sec");
  Serial.print(RTC.getHours()); Serial.print(":");  Serial.print(RTC.getMinutes()); Serial.print(":");  Serial.print(RTC.getSeconds());
/*****************    UV   ************************/


  float uv_sensor_value = analogRead(UV_PIN);
  float sensorVoltage = uv_sensor_value / 1024 * 5.0;
  int UV_index = sensorVoltage / 0.1;
  
  Serial.print("    UV INDEX  ");  Serial.println(UV_index);

  
  delay(100);
}



/* Section the Oled Screen 256 * 64    ----->  32 col * 8 lines */

uint8_t * Border_line[32] = {
  "--------------------------------" ,
  "- D  M  YEAR | Hr Min S   | UV -" ,
  "-            |            |    -" ,
  "--------------------------------" ,
  "- TEMP |  PRESSURE |  HUMIDITY -" ,
  "-      |           |           -" ,
  "-  C   |     Pa    |     %     -" ,
  "--------------------------------"  
} ;

/****************    DS3231 RTC   *******************/
#define RTC_DATA_LINE     2
#define RTC_DAY_INDEX     2
#define RTC_MONTH_INDEX   5
#define RTC_YEAR_INDEX    8
#define RTC_HOUR_INDEX    15
#define RTC_MINUTE_INDEX  18
#define RTC_SEC_INDEX     22
#define RTC_PM_INDEX      25


/****************    UV   *******************/
#define UV_DATA_LINE   2
#define UV_DATA_INDEX    28

/****************    TEMP   *******************/
#define TEMP_DATA_LINE   5
#define TEMP_DATA_INDEX    3

/****************    PRESSURE   *******************/
#define PRESSURE_DATA_LINE   5
#define PRESSURE_DATA_INDEX    12

/****************    HUMITDY   *******************/
#define HUMIDTY_DATA_LINE   5
#define HUMIDTY_DATA_INDEX    25



#define sEOS_UPDATE  500UL
#define CLEAR_UPDATE  2000UL
#define DHT_UPDATE  2000UL

uint8_t firstrun_flag = 1 ;
volatile uint32_t clear_counter = 4 ;
volatile uint32_t dht_counter = 4 ;

void loop(void)
{
  if (firstrun_flag)
  {
    firstrun_flag = !firstrun_flag ;  

    u8x8.clearDisplay();
    u8x8.drawString(0,0,Border_line[0]);
    u8x8.drawString(0,7,Border_line[7]);
    u8x8.drawString(8,2,"LOCAL  WEATHER");
    u8x8.drawString(11,4,"STATION");
    delay(2000);
  }

  if (clear_counter)
  {
    clear_counter-=1 ;
  }
  else
  {
    clear_counter = CLEAR_UPDATE/sEOS_UPDATE ;
    u8x8.clearDisplay();
      Draw_Border() ;

  }  
/****************    DS3231 RTC   *******************/ 
//  u8x8.clearDisplay();
//  Draw_Border() ;

  utils_uitoa(RTC.getDay(),arr) ;
  u8x8.drawString(RTC_DAY_INDEX,RTC_DATA_LINE,arr);
  utils_uitoa(RTC.getMonth(),arr) ;
  u8x8.drawString(RTC_MONTH_INDEX,RTC_DATA_LINE,arr);
  utils_uitoa(RTC.getYear(),arr) ;
  u8x8.drawString(RTC_YEAR_INDEX,RTC_DATA_LINE,arr);

  utils_uitoa(RTC.getHours(),arr) ;
  u8x8.drawString(RTC_HOUR_INDEX,RTC_DATA_LINE,arr);
  utils_uitoa(RTC.getMinutes(),arr) ;
  u8x8.drawString(RTC_MINUTE_INDEX,RTC_DATA_LINE,arr);
  utils_uitoa(RTC.getSeconds(),arr) ;
  u8x8.drawString(RTC_SEC_INDEX,RTC_DATA_LINE,arr);

    if (RTC.getHourMode() == CLOCK_H12)
  {
      switch (RTC.getMeridiem()) {
      case HOUR_AM:
        u8x8.drawString(RTC_PM_INDEX,RTC_DATA_LINE,"AM");
        break;
      case HOUR_PM:
        u8x8.drawString(RTC_PM_INDEX,RTC_DATA_LINE,"PM");
        break;
      }
  }


/*****************    UV   ************************/
//  u8x8.clearDisplay();
//  Draw_Border() ;

  float uv_sensor_value = analogRead(UV_PIN);
  float sensorVoltage = uv_sensor_value / 1024 * 5.0;
  int UV_index = sensorVoltage / 0.1;

  utils_uitoa(UV_index,arr) ;
  u8x8.drawString(UV_DATA_INDEX,UV_DATA_LINE,arr);  

/*****************    BMP180   **********************/
//  u8x8.clearDisplay();
//  Draw_Border() ;

  utils_uitoa(bmp.readPressure()/100,arr) ;
  u8x8.drawString(PRESSURE_DATA_INDEX,PRESSURE_DATA_LINE,arr);

  utils_uitoa((uint8_t)bmp.readTemperature(),arr) ;
  u8x8.drawString(TEMP_DATA_INDEX,TEMP_DATA_LINE,arr);


/*****************    DHT   ************************/
//  u8x8.clearDisplay();
//  Draw_Border() ;

  sensors_event_t event;

  if (dht_counter)
  {
    dht_counter-=1 ;
  }
  else
  {
    dht_counter = DHT_UPDATE/sEOS_UPDATE ;


  /* Random Values for humadity */
  utils_uitoa((uint8_t)(rand()%10 + 40),arr) ; 

  u8x8.drawString(HUMIDTY_DATA_INDEX,HUMIDTY_DATA_LINE,arr);

  
/* Get temperature event and print its value.  */
//  dht.temperature().getEvent(&event);
//  if (isnan(event.temperature)) {
////    u8x8.drawString(TEMP_DATA_INDEX,TEMP_DATA_LINE,"ER");
//  }
//  else
//  {
//    utils_uitoa((uint8_t)event.temperature,arr) ;
//    u8x8.drawString(TEMP_DATA_INDEX,TEMP_DATA_LINE,arr);
//  }


/* Get humidity event and print its value.  */
//  utils_uitoa((uint8_t)dht_11.readHumidity(),arr) ;
//  u8x8.drawString(HUMIDTY_DATA_INDEX,HUMIDTY_DATA_LINE,arr);
  
//  dht.humidity().getEvent(&event);
//  if (isnan(event.relative_humidity)) {
//    u8x8.drawString(HUMIDTY_DATA_INDEX,HUMIDTY_DATA_LINE,"ER");
//  }
//  else {
//    utils_uitoa((uint8_t)event.relative_humidity,arr) ;
//    u8x8.drawString(HUMIDTY_DATA_INDEX,HUMIDTY_DATA_LINE,arr);
//  }
} 


  

  delay(sEOS_UPDATE);
}




/****************************************************/
void Draw_Border(void) 
{
  uint8_t line_counter = 0 ;
  
  for (line_counter = 0 ; line_counter < 8 ; line_counter++)
  {
      u8x8.drawString(0,line_counter,Border_line[line_counter]);
  } 
}
/****************************************************/



/****************************************************/
/****************************************************/
static void utils_reverse(uint8_t *str) ;
static int utils_strlen(uint8_t *s) ;
/****************************************************/
static void utils_uitoa(uint32_t n, uint8_t s[])
{
    uint8_t i = 0;

    i = 0;
    do
    {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i] = '\0';
    utils_reverse(s);
}
/****************************************************/
static void utils_reverse(uint8_t *str)
{
    int i, j;
    uint8_t temp;
    for (i = 0, j = utils_strlen(str) - 1; i < j; i++, j--)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}
/****************************************************/
static int utils_strlen(uint8_t *s)
{
    int i = 0;
    while (s[i] != 0 && s[i] != '.')
        i++;
    return i;
}
/****************************************************/
/****************************************************/

#if 0
///* Section the Oled Screen 256 * 64    ----->  32 col * 8 lines */
//
//void loop(void)
//{
//  
///****************    DS3231 RTC   *******************/ 
//  u8x8.clearDisplay();
//  u8x8.setCursor(1, 10);
//  u8x8.print(RTC.getHours());
//  u8x8.print(":");
//  u8x8.setCursor(1, 14);
//  u8x8.print(RTC.getMinutes());
//  if (RTC.getHourMode() == CLOCK_H12)
//  {
//      switch (RTC.getMeridiem()) {
//      case HOUR_AM:
//        u8x8.print(" AM");
//        break;
//      case HOUR_PM:
//        u8x8.print(" PM");
//        break;
//      }
//  }
//
//    u8x8.print(RTC.getDay());
//    u8x8.print("-");
//    u8x8.print(RTC.getMonth());
//    u8x8.print("-");
//    u8x8.print(RTC.getYear());
//    u8x8.print(" ");
//    switch (RTC.getDoW()) {
//    case 1:
//      u8x8.print("Saturday");
//      break;
//    case 2:
//      u8x8.print("Sunday");
//      break;
//    case 3:
//      u8x8.print("Monday");
//      break;
//    case 4:
//      u8x8.print("Tuesday");
//      break;
//    case 5:
//      u8x8.print("Wednesday");
//      break;
//    case 6:
//      u8x8.print("Thursday");
//      break;
//    case 7:
//      u8x8.print("Friday");
//      break;
//  }
//
//  delay(5000);
//
//
///*****************    BMP180   **********************/
//  u8x8.clearDisplay();
//  u8x8.drawBitmap(0, 0, frame0, 40, 40, 1);
//  u8x8.setCursor(41, 28);
//  u8x8.print(bmp.readPressure() / 100);
//  u8x8.setCursor(110, 28);
//  u8x8.print(" hPa");
//  u8x8.setCursor(0, 55);
//  u8x8.print("Altitude:");
//  u8x8.setCursor(65, 62);
//  u8x8.print(bmp.readAltitude(1005.47));
//  u8x8.setCursor(113, 62);
//  u8x8.print("m");
//  u8x8.display();
//  
//  delay(5000);
///*****************    DHT   ************************/
//  u8x8.clearDisplay();
//  u8x8.drawBitmap(0, 5, frame1, 40, 51, 1);
//
//  // Get temperature event and print its value.
//  sensors_event_t event;
//  dht.temperature().getEvent(&event);
//  if (isnan(event.temperature)) {
//    display.print("ET");
//  }
//  else {
//    u8x8.setCursor(35, 30);
//    u8x8.print(event.temperature);
//    u8x8.setCursor(102, 28);
//    u8x8.print(" *");
//    u8x8.setCursor(110, 31);
//    u8x8.print(" C");
//  }
//
//
//  // Get humidity event and print its value.
//  dht.humidity().getEvent(&event);
//  if (isnan(event.relative_humidity)) {
//    display.print("EH");
//  }
//  else {
//    u8x8.setCursor(66, 45);
//    u8x8.println("RH :");
//    u8x8.setCursor(95, 45);;
//    u8x8.print(event.relative_humidity);
//    u8x8.setCursor(110, 45);
//    u8x8.print("%");
//  }
///*****************    UV   ************************/
//  u8x8.clearDisplay();
//  u8x8.drawBitmap(0, 56, frame2, 135, 15, 1);
//
//  float uv_sensor_value = analogRead(UV_PIN);
//  float sensorVoltage = uv_sensor_value / 1024 * 5.0;
//  int UV_index = sensorVoltage / 0.1;
//
//  u8x8.setCursor(0, 15);
//  u8x8.print("    UV INDEX  ");
//  u8x8.setCursor(58, 45);
//  u8x8.println(UV_index);
//
//  u8x8.display();
//  delay(5000);
//}

#endif
