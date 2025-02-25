//     TIMER 2 INTERRUPTS
//....................................
ISR(TIMER2_COMPA_vect) {

  //Creating step pulses
  if (Dir_M2 != 0) {
    Count_timer2++;
    if (Count_timer2 <= Count_TOP2)PORTD |= 0b01000000;
    else PORTD &= 0b10111111;
    if (Count_timer2 > Count_BOT2) {
      Count_timer2 = 0;
      if (Dir_M2 > 0)Step2++;
      else if (Dir_M2 < 0)Step2--;
    }
  }

  if (Dir_M3 != 0) {
    Count_timer3++;
    if (Count_timer3 <= Count_TOP3)PORTD |= 0b10000000;
    else PORTD &= 0b01111111;
    if (Count_timer3 > Count_BOT3) {
      Count_timer3 = 0;
      if (Dir_M3 > 0)Step3++;
      else if (Dir_M3 < 0)Step3--;
    }
  }
}