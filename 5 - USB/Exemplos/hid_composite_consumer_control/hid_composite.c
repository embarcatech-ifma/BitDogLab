#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

// GPIO dos botões
#define GPIO_VOLUME_UP   5
#define GPIO_VOLUME_DOWN 6

void led_blinking_task(void);
void send_hid_report(uint8_t report_id, bool key_pressed);
void hid_task(void);

// Enum para estados do LED
enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

/*------------- MAIN -------------*/
int main(void) {
  board_init();

  // Configura os botões como entrada com pull-up
  gpio_init(GPIO_VOLUME_UP);
  gpio_set_dir(GPIO_VOLUME_UP, false);
  gpio_pull_up(GPIO_VOLUME_UP);

  gpio_init(GPIO_VOLUME_DOWN);
  gpio_set_dir(GPIO_VOLUME_DOWN, false);
  gpio_pull_up(GPIO_VOLUME_DOWN);

  tusb_init();

  while (1) {
    tud_task();
    led_blinking_task();
    hid_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void) {
  blink_interval_ms = BLINK_MOUNTED;
}

void tud_umount_cb(void) {
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

void tud_suspend_cb(bool remote_wakeup_en) {
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

void tud_resume_cb(void) {
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// HID callbacks
//--------------------------------------------------------------------+
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
  uint8_t next_report_id = report[0] + 1u;
  if (next_report_id < REPORT_ID_COUNT) {
    send_hid_report(next_report_id, board_button_read());
  }
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t const* buffer, uint16_t bufsize) {
  // Nada implementado
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t* buffer, uint16_t reqlen) {
  // Nada implementado
  return 0;
}

//--------------------------------------------------------------------+
// HID Task
//--------------------------------------------------------------------+
void hid_task(void) {
  static bool last_up = true;
  static bool last_down = true;

  bool curr_up = gpio_get(GPIO_VOLUME_UP);
  bool curr_down = gpio_get(GPIO_VOLUME_DOWN);

  // Detecta transição de botão pressionado
  if (!curr_up && last_up) {
    send_hid_report(REPORT_ID_CONSUMER_CONTROL, true);
  }
  else if (!curr_down && last_down) {
    send_hid_report(REPORT_ID_CONSUMER_CONTROL, true);
  }
  // Detecta botão solto
  else if (curr_up && !last_up) {
    send_hid_report(REPORT_ID_CONSUMER_CONTROL, false);
  }
  else if (curr_down && !last_down) {
    send_hid_report(REPORT_ID_CONSUMER_CONTROL, false);
  }

  last_up = curr_up;
  last_down = curr_down;
}

void send_hid_report(uint8_t report_id, bool key_pressed) {
  if (!tud_hid_ready()) return;

  if (report_id == REPORT_ID_CONSUMER_CONTROL) {
    uint16_t volume_key = 0;

    if (!gpio_get(GPIO_VOLUME_UP)) {
      volume_key = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
    } else if (!gpio_get(GPIO_VOLUME_DOWN)) {
      volume_key = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
    }

    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_key, sizeof(volume_key));
  }
}

//--------------------------------------------------------------------+
// LED Blink
//--------------------------------------------------------------------+
void led_blinking_task(void) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  if (!blink_interval_ms) return;

  if (board_millis() - start_ms < blink_interval_ms) return;
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = !led_state;
}
