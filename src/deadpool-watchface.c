#include <pebble.h>

static Window *window;
static TextLayer *time_text_layer;
static BitmapLayer *background_layer;
static GBitmap *background_bitmap;
static GFont time_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(time_text_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  #if defined (PBL_PLATFORM_BASALT)
  background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DEADPOOL_FACE_SQUARE);
  #else
  background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DEADPOOL_FACE);
  #endif
  background_layer = bitmap_layer_create(bounds);

  bitmap_layer_set_bitmap(background_layer, background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

  time_text_layer = text_layer_create(
      GRect(0, 30, bounds.size.w, 50));

  text_layer_set_background_color(time_text_layer, GColorClear);
  text_layer_set_text_color(time_text_layer, GColorWhite);
  text_layer_set_text(time_text_layer, "00:00");
  text_layer_set_text_alignment(time_text_layer, GTextAlignmentCenter);

  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MILITARY_36));

  text_layer_set_font(time_text_layer, time_font);

  layer_add_child(window_layer, text_layer_get_layer(time_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(time_text_layer);
  bitmap_layer_destroy(background_layer);
  gbitmap_destroy(background_bitmap);
  fonts_unload_custom_font(time_font);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_set_background_color(window, GColorBlack);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  window_stack_push(window, true);

  update_time();
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
