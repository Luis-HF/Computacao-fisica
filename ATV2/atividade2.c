uint8_t leds = 0x00;
bool clk_last;
unsigned long last_debounce_time = 0;
const int debounce_delay = 50;

void setup() {
DDRD = 0xFF;
DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2));
PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2);

clk_last = PINC & (1 << PINC2);
}

void loop() {
bool sw_now = PINC & (1 << PINC0); // A0
bool dt_now = PINC & (1 << PINC1); // A1
bool clk_now = PINC & (1 << PINC2); // A2

if (clk_now != clk_last) {
if ((millis() - last_debounce_time) > debounce_delay) {

if (clk_now != dt_now) {
leds = (leds << 1) | 0x01;
} else {
leds = (leds >> 1);
}

last_debounce_time = millis();
}
}
clk_last = clk_now;

static bool sw_last = HIGH;
if (sw_now == LOW && sw_last == HIGH) {
leds = ~leds;
delay(200);
}
sw_last = sw_now;
PORTD = leds;
}