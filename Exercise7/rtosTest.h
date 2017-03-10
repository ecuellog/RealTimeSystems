void init_LED(void);
void init_PINS(void);

void enable_LED(void);
void disable_LED(void);
void toggle_LED(void);

void interrupt_disable_ON(void);

/* Up to 5 tasks (0-5) */
void task_ON(uint8_t);
void task_OFF(uint8_t);
