#include <avr/interrupt.h>
#include <avr/io.h>

int main() {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, ~CLKCTRL_PEN_bm);

    // オープンドレイン
    PORTA.OUTCLR = PIN6_bm | PIN7_bm;
    PORTA.DIRSET = PIN6_bm | PIN7_bm; // LOW
    // PORTA.DIRCLR = PIN6_bm | PIN7_bm; // Hi-Z (PWM)

    // 定期割込
    TCA0.SINGLE.PER = 1 * F_CPU / 256;
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;

    // PWM
    TCB0.CCMPL = 0xFFFF; // PWM周期
    TCB0.CCMPH = 0x3FFF; // PWMパルス幅
    TCB0.CTRLB = TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
    TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm;
}

volatile uint8_t count = 0;

ISR(TCA0_OVF_vect) {
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    count++;
    if (count % 1 == 0) {
        PORTA.DIRTGL = PIN6_bm;
    }
    if (count % 2 == 0) {
        PORTA.DIRTGL = PIN7_bm;
    }
}