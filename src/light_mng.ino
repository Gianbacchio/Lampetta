// this file collects all functions related with light management

void setLightMode(uint8_t modeled)  {
        cmdStatus = "operating";
        switch (modeled) {
        case 0:
                changeColor(strip.Color(0, 0, 0)); // off
                cmdStatus = "off";
                break;
        case 1:
                changeColor(strip.Color(255, 255, 255)); // white
                break;
        case 5:
                changeColor(strip.Color(255, 0, 0)); // Red
                break;
        case 6:
                changeColor(strip.Color(0, 255, 0)); // Green
                break;
        case 7:
                changeColor(strip.Color(0, 0, 255)); // Blue
                break;
        case 8:
                theaterChase(strip.Color(255, 0, 0), NEOLEDNUM); // Red
                break;
        case 9:
                theaterChase(strip.Color(0, 255, 0), NEOLEDNUM); // Green
                break;
        case 10:
                theaterChase(strip.Color(255, 0, 255), NEOLEDNUM); // Green
                break;
        case 3:
                rainbowCycle(NEOLEDNUM);
                break;
        case 4:
                rainbow(NEOLEDNUM);
                break;
        case 2:
                theaterChaseRainbow(NEOLEDNUM);
                break;
        }
}

// function to set lamp in off mode
void setoff() {
        setLightMode(0);
        selectedMode = 0;
}

// function to set first led to specific colour
void setLed (uint32_t c, uint8_t n)  {
        setoff();
        for (uint16_t i = 0; i < n; i++) {
                strip.setPixelColor(i, c);
        }
        for (uint16_t i = n; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, 0);
        }
        strip.show();
}

// function to change all leds colour
void changeColor(uint32_t c) {
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, c);
                strip.show();
                delay(50);
        }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
        for (uint16_t i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, c);
                delay(wait);
                strip.show();
                delay(wait);
        }
}

void rainbow(uint8_t wait) {
        uint8_t i;
//  for (j = 0; j < 256; j++) {
        for (i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, Wheel((i + rainbowcolor) & 255));
        }
        rainbowcolor++;
        strip.show();
        delay(wait);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
        uint8_t i;
        for (i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + rainbowcolor) & 255));
        }
        rainbowcolor++;
        strip.show();
        delay(wait);
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
        for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
                for (int q = 0; q < 3; q++) {
                        for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
                                strip.setPixelColor(i + q, c); //turn every third pixel on
                        }
                        strip.show();

                        delay(wait);

                        for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
                                strip.setPixelColor(i + q, 0); //turn every third pixel off
                        }
                }
        }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
        for (int q = 0; q < 3; q++) {
                for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
                        strip.setPixelColor(i + q, Wheel( (i + rainbowcolor) % 255)); //turn every third pixel on
                }
                strip.show();
                delay(wait);

                for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
                        strip.setPixelColor(i + q, 0); //turn every third pixel off
                }
        }
        rainbowcolor++;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
        WheelPos = 255 - WheelPos;
        if (WheelPos < 85) {
                return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        if (WheelPos < 170) {
                WheelPos -= 85;
                return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        WheelPos -= 170;
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
