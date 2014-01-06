#include <pebble.h>

static Window *window;

static TextLayer *text_layer;
static char text_layer_buff[16];

static TextLayer *stop_name;
static TextLayer *stop_direction;
static char stop[16];
static char direction[13];

static TextLayer *row1;
static TextLayer *row1_details;
static char row1_buff[10];
static char row1_details_buff[12];

static TextLayer *row2;
static TextLayer *row2_details;
static char row2_buff[10];
static char row2_details_buff[12];

static TextLayer *row3;
static TextLayer *row3_details;
static char row3_buff[10];
static char row3_details_buff[12];

static TextLayer *row4;
static TextLayer *row4_details;
static char row4_buff[10];
static char row4_details_buff[12];

static AppSync sync;
static uint8_t sync_buffer[250];

enum BusKey {
  BUS_NAME = 0x0,
  BUS_DIRECTION = 0x1,
  T1 = 0x2,
  T1_D = 0x3,
  T2 = 0x4,
  T2_D = 0x5,
  T3 = 0x6,
  T3_D = 0x7,
  T4 = 0x8,
  T4_D = 0x9,
};

static char *dir1 = "fra midtbyen";
static char *dir2 = "til midtbyen";
static char *dir3 = "mot Klæbu";
static char *dir4 = "mot Klæbu";
static char *dir5 = "mot Stjørdal";
static char *dir6 = "mot Trondheim";
static char *dir7 = "vestover";
static char *dir8 = "østover";
static char *dir9 = "-retning-";
static char *dir_text[9];

static void switch_visibility(bool g1, bool g2){
  layer_set_hidden((Layer *)text_layer, g1);

  layer_set_hidden((Layer *)row1, g2);
  layer_set_hidden((Layer *)row1_details, g2);
  layer_set_hidden((Layer *)row2, g2);
  layer_set_hidden((Layer *)row2_details, g2);
  layer_set_hidden((Layer *)row3, g2);
  layer_set_hidden((Layer *)row3_details, g2);
  layer_set_hidden((Layer *)row4, g2);
  layer_set_hidden((Layer *)row4_details, g2);
}

static void set_field_text(TextLayer *field, char * buff, const char *text) {
  if (*text != '\0') {
    strcpy(buff, "Linje ");
    text_layer_set_text(field, strcat(buff, text));
  }
  else
  {
    text_layer_set_text(field, "");
  }
}

static void update_data() {
  switch_visibility(false, true);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletCString(10, "refresh");
  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case BUS_NAME:
      switch_visibility(true , false);
      text_layer_set_text(stop_name, new_tuple->value->cstring);
      break;

    case BUS_DIRECTION:
      text_layer_set_text(stop_direction, dir_text[new_tuple->value->uint8]);
      break;

    case T1:
      set_field_text(row1, row1_details_buff, new_tuple->value->cstring);
      break;

    case T1_D:
      text_layer_set_text(row1_details, new_tuple->value->cstring);
      break;

    case T2:
      set_field_text(row2, row2_details_buff, new_tuple->value->cstring);
      break;

    case T2_D:
      text_layer_set_text(row2_details, new_tuple->value->cstring);
      break;

    case T3:
      set_field_text(row3, row3_details_buff, new_tuple->value->cstring);
      break;

    case T3_D:
      text_layer_set_text(row3_details, new_tuple->value->cstring);
      break;

    case T4:
      set_field_text(row4, row4_details_buff, new_tuple->value->cstring);
      break;

    case T4_D:
      text_layer_set_text(row4_details, new_tuple->value->cstring);
      break;
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Refreshing...");
  update_data();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Refreshing...");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  stop_name = text_layer_create(GRect(5, 0, 144, 30));
  text_layer_set_font(stop_name, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(stop_name, GTextAlignmentLeft);
  text_layer_set_text(stop_name, stop);

  stop_direction = text_layer_create(GRect(5, 28, 144, 30));
  text_layer_set_text_alignment(stop_direction, GTextAlignmentLeft);
  text_layer_set_text(stop_direction, direction);

  row1 = text_layer_create((GRect) { .origin = { 5, 60 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(row1, GTextAlignmentLeft);
  text_layer_set_text(row1, row1_buff);

  row1_details = text_layer_create((GRect) { .origin = { bounds.size.w-70, 60 }, .size = { 60, 20 } });
  text_layer_set_text_alignment(row1_details, GTextAlignmentRight);
  text_layer_set_text(row1_details, row1_details_buff);


  row2 = text_layer_create((GRect) { .origin = { 5, 80 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(row2, GTextAlignmentLeft);
  text_layer_set_text(row2, row2_buff);

  row2_details = text_layer_create((GRect) { .origin = { bounds.size.w-70, 80 }, .size = { 60, 20 } });
  text_layer_set_text_alignment(row2_details, GTextAlignmentRight);
  text_layer_set_text(row2_details, row2_details_buff);


  row3 = text_layer_create((GRect) { .origin = { 5, 100 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(row3, GTextAlignmentLeft);
  text_layer_set_text(row3, row3_buff);

  row3_details = text_layer_create((GRect) { .origin = { bounds.size.w-70, 100 }, .size = { 60, 20 } });
  text_layer_set_text_alignment(row3_details, GTextAlignmentRight);
  text_layer_set_text(row3_details, row3_details_buff);


  row4 = text_layer_create((GRect) { .origin = { 5, 120 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(row4, GTextAlignmentLeft);
  text_layer_set_text(row4, row4_buff);

  row4_details = text_layer_create((GRect) { .origin = { bounds.size.w-70, 120 }, .size = { 60, 20 } });
  text_layer_set_text_alignment(row4_details, GTextAlignmentRight);
  text_layer_set_text(row4_details, row4_details_buff);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text(text_layer, text_layer_buff);

  Tuplet initial_values[] = {
    TupletCString(BUS_NAME, "holdeplass"),
    TupletInteger(BUS_DIRECTION, (uint8_t) 8),
    TupletCString(T1, ""),
    TupletCString(T1_D, ""),
    TupletCString(T2, ""),
    TupletCString(T2_D, ""),
    TupletCString(T3, ""),
    TupletCString(T3_D, ""),
    TupletCString(T4, ""),
    TupletCString(T4_D, ""),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer),
               initial_values, ARRAY_LENGTH(initial_values),
               sync_tuple_changed_callback, sync_error_callback, NULL);

  layer_add_child(window_layer, text_layer_get_layer(stop_name));
  layer_add_child(window_layer, text_layer_get_layer(stop_direction));
  layer_add_child(window_layer, text_layer_get_layer(row1));
  layer_add_child(window_layer, text_layer_get_layer(row1_details));
  layer_add_child(window_layer, text_layer_get_layer(row2));
  layer_add_child(window_layer, text_layer_get_layer(row2_details));
  layer_add_child(window_layer, text_layer_get_layer(row3));
  layer_add_child(window_layer, text_layer_get_layer(row3_details));
  layer_add_child(window_layer, text_layer_get_layer(row4));
  layer_add_child(window_layer, text_layer_get_layer(row4_details));

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_set_hidden((Layer *)text_layer, true);
}

static void window_unload(Window *window) {
  app_sync_deinit(&sync);
  
  text_layer_destroy(text_layer);
  text_layer_destroy(stop_name);
  text_layer_destroy(stop_direction);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Refresh message sent sent");
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed message sent");
}

static void init(void) {
  window = window_create();

  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  window_set_click_config_provider(window, click_config_provider);

  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  dir_text[0] = dir1;
  dir_text[1] = dir2;
  dir_text[2] = dir3;
  dir_text[3] = dir4;
  dir_text[4] = dir5;
  dir_text[5] = dir6;
  dir_text[6] = dir7;
  dir_text[7] = dir8;
  dir_text[8] = dir9;

  const uint32_t inbound_size = 250;
  const uint32_t outbound_size = 16;
  app_message_open(inbound_size, outbound_size);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
