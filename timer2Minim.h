class timerMinim
{
  public:
    timerMinim(uint32_t interval);				
    void setInterval(uint32_t interval);	
    boolean isReady();						
    void reset();							

  private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
};

timerMinim::timerMinim(uint32_t interval) {
  _interval = interval;
  _timer = millis();
}

void timerMinim::setInterval(uint32_t interval) {
  _interval = (interval == 0) ? 1 : interval;    
}

boolean timerMinim::isReady() {
  uint32_t thisMls = millis();
  if (thisMls - _timer >= _interval) {
    do {
      _timer += _interval;
      if (_timer < _interval) break;          
    } while (_timer < thisMls - _interval);  
    return true;
  } else {
    return false;
  }
}

void timerMinim::reset() {
  _timer = millis();
}
