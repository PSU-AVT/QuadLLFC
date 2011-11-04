#ifndef ESC_H
#define ESC_H

typedef enum ESC {
	ESC_FRONT,
	ESC_BACK,
	ESC_LEFT,
	ESC_RIGHT
} ESC;

void esc_init_all(void);

void esc_arm_all(void);

#endif
