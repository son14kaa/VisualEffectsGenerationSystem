#define TEXT_DIRECTION 1  
#define MIRR_V 0          
#define MIRR_H 0          

#define TEXT_HEIGHT 3     
#define LET_WIDTH 5       
#define LET_HEIGHT 8      
#define SPACE 1           

int offset = WIDTH;
uint32_t scrollTimer;

void resetString() {
  offset = WIDTH;
}

boolean fillString(String text, CRGB textColor, boolean clear) {
  if (loadingFlag) {
    offset = WIDTH;   
    loadingFlag = false;
  }

  if (millis() - scrollTimer >= 100) {
    scrollTimer = millis();
    if (clear) FastLED.clear();
    byte i = 0, j = 0;
    while (text[i] != '\0') {
      if ((byte)text[i] > 191) {    
        i++;
      } else {
        drawLetter(j, text[i], offset + j * (LET_WIDTH + SPACE), textColor);
        i++;
        j++;
      }
    }

    offset--;
    if (offset < -j * (LET_WIDTH + SPACE)) {    
      offset = WIDTH + 3;
      return true;
    }
    FastLED.show();
  }
  return false;
}

void drawLetter(uint8_t index, uint8_t letter, int16_t offset, CRGB textColor) {
  int8_t start_pos = 0, finish_pos = LET_WIDTH;

  if (offset < -LET_WIDTH || offset > WIDTH) return;
  if (offset < 0) start_pos = -offset;
  if (offset > (WIDTH - LET_WIDTH)) finish_pos = WIDTH - offset;

  for (byte i = start_pos; i < finish_pos; i++) {
    int thisByte;
    if (MIRR_V) thisByte = getFont((byte)letter, LET_WIDTH - 1 - i);
    else thisByte = getFont((byte)letter, i);

    for (byte j = 0; j < LET_HEIGHT; j++) {
      boolean thisBit;

      if (MIRR_H) thisBit = thisByte & (1 << j);
      else thisBit = thisByte & (1 << (LET_HEIGHT - 1 - j));

      if (TEXT_DIRECTION) {
        if (thisBit) leds[getPixelNumber(offset + i, TEXT_HEIGHT + j)] = textColor;
      } else {
        if (thisBit) leds[getPixelNumber(i, offset + TEXT_HEIGHT + j)] = textColor;
      }
    }
  }
}

uint8_t getFont(uint8_t font, uint8_t row) {
  font = font - '0' + 16;   
  if (font <= 90) return pgm_read_byte(&(fontHEX[font][row]));     
  else if (font >= 112 && font <= 159) {    
    return pgm_read_byte(&(fontHEX[font - 17][row]));
  } else if (font >= 96 && font <= 111) {
    return pgm_read_byte(&(fontHEX[font + 47][row]));
  }
}
