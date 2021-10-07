/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>
#include <drivers/gpio.h>

#define PWM_LABEL DT_LABEL(DT_ALIAS(pwm_buzzer))
#define PWM_CHANNEL PWM_PIN
#define PWM_PIN       20

#define NOTE_DURATION_MS 50
#define REPEAT_DELAY_MS  100

static struct k_work_delayable pwm_work;

const struct device *pwm_dev;

enum note_frequency {
	NOTE_B5 = 988,
	NOTE_A5 = 880,
	NOTE_G5 = 784,
	NOTE_F5 = 699,
	NOTE_E5 = 659,
	NOTE_D5 = 587,
	NOTE_C5 = 523,
	NOTE_B4 = 494,
	NOTE_A4 = 440,
	NOTE_G4 = 392,
};

enum note_duration {
	NOTE_FULL = 1,
	NOTE_HALF = 2,
	NOTE_QUARTER = 4,
	NOTE_EIGTH = 8,
};

typedef struct {
	uint16_t note_frequency;
	uint8_t duration;
} note_t;

const note_t sound_identify[] = {
	{NOTE_B5, NOTE_HALF},
	{NOTE_A5, NOTE_HALF},
	{NOTE_G5, NOTE_HALF},
	{NOTE_F5, NOTE_HALF},
	{NOTE_E5, NOTE_HALF},
	{NOTE_D5, NOTE_HALF},
	{NOTE_C5, NOTE_HALF},
};

static void pwm_handler(struct k_work *item)
{
	uint32_t note_period;
	uint32_t note_duration;
	uint32_t note_rest_duration;

	for (int i = 0; i < 7; i++) {
		note_period = USEC_PER_SEC / sound_identify[i].note_frequency;
		note_duration = NOTE_DURATION_MS / sound_identify[i].duration;
		note_rest_duration = note_duration * 1.3;
		
		/* Play note */
		pwm_pin_set_usec(pwm_dev, PWM_CHANNEL, note_period,
			note_period / 2U, 0);
		k_msleep(note_duration);
		
		/* Rest */
		pwm_pin_set_usec(pwm_dev, PWM_CHANNEL, 0, 0, 0);
		k_msleep(note_rest_duration);
	}

	k_work_reschedule(&pwm_work, K_MSEC(REPEAT_DELAY_MS));
}

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	pwm_dev = device_get_binding(PWM_LABEL);

	k_work_init_delayable(&pwm_work, pwm_handler);

	k_work_reschedule(&pwm_work, K_MSEC(0));

	while (1) {
		k_msleep(1000);
		printk("❤️\n");
	}
}
