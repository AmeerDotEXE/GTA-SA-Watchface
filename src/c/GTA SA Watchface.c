#include <pebble.h>
#include <math.h>

// Persistent storage key
#define SETTINGS_KEY 1



typedef struct ClaySettings {
	bool hasArmour;
	bool hasBreath;
	bool isZoomed;
	int weekStartOffset;
} ClaySettings;



static ClaySettings settings;
static Window *s_window;

static BitmapLayer *s_background_layer;
static GBitmap *s_weapon_bitmap;

static float batteryPercent = 1;
static float stepsPercent = 0;
static float stepsGoal = 5000;
static int selectedWeapon = 0;

static Layer *armour_canvas_layer;
static Layer *breath_canvas_layer;
static Layer *health_canvas_layer;

static Layer *armour_bg_canvas_layer;
static Layer *breath_bg_canvas_layer;
static Layer *health_bg_canvas_layer;

static Layer *star_canvas_layer;
static GPath *stars_list[6];

static TextLayer *s_time_layer;
static TextLayer *s_money_layer;

static GFont s_font_gta_original;



static const GPathInfo STAR_PATH_INFO = {
	.num_points = 10,
	.points = (GPoint []) {
		{11, 0},
		{8.5, 8},
		{0, 8},
		{6.829, 13.348},
		{4.125, 22},
		{11, 16.615},
		{17.875, 22},
		{15.171, 13.348},
		{22, 7.9}, //supposed to be 8 not 7.9 but line appeared a bit high
		{13.5, 8}
	}
};
// static const GPathInfo STAR_PATH_INFO = {
// 	.num_points = 10,
// 	.points = (GPoint []) {
// 		{11, 0},
// 		{8.5, 8},
// 		{0, 8},
// 		{6.829, 13.348},
// 		{4.125, 22},
// 		{11, 16.615},
// 		{17.875, 22},
// 		{15.171, 13.348},
// 		{22, 7.9}, //supposed to be 8 not 7.9 but line appeared a bit high
// 		{13.5, 8}
// 	}
// };
static const GPathInfo ZoomedSTAR_PATH_INFO = {
	.num_points = 10,
	.points = (GPoint []) {
		{13, 0},
		{10, 9},
		{0, 9},
		{7.927, 15.219},
		{4.333, 26},
		{13, 19.2},
		{21.667, 26},
		{18.073, 15.219},
		{26, 8.9}, //supposed to be 8 not 7.9 but line appeared a bit high
		{16, 9}
	}
};

static const uint32_t ICON_LIST_64[] = {
	RESOURCE_ID_FIST_64,
	RESOURCE_ID_WEAPON_AK47,
	RESOURCE_ID_WEAPON_BAT,
	RESOURCE_ID_WEAPON_BRASS_KNUCKLES,
	RESOURCE_ID_WEAPON_CAMERA,
	RESOURCE_ID_WEAPON_CANE,
	RESOURCE_ID_WEAPON_CELLPHONE,
	RESOURCE_ID_WEAPON_CHAINSAW,
	RESOURCE_ID_WEAPON_COMBAT_SG,
	RESOURCE_ID_WEAPON_DESERT_EAGLE,
	RESOURCE_ID_WEAPON_DETONATOR,
	RESOURCE_ID_WEAPON_FIRE_EXTINGUISHER,
	RESOURCE_ID_WEAPON_FLAMETHROWER,
	RESOURCE_ID_WEAPON_FLOWER,
	RESOURCE_ID_WEAPON_GOLF_CLUB,
	RESOURCE_ID_WEAPON_GRENADE,
	RESOURCE_ID_WEAPON_HEAT_SKRL,
	RESOURCE_ID_WEAPON_JETPACK,
	RESOURCE_ID_WEAPON_KATANA,
	RESOURCE_ID_WEAPON_KNIFE,
	RESOURCE_ID_WEAPON_M4,
	RESOURCE_ID_WEAPON_MICRO_UZI,
	RESOURCE_ID_WEAPON_MINIGUN,
	RESOURCE_ID_WEAPON_MOLOTOV_COCKTAIL,
	RESOURCE_ID_WEAPON_MP5,
	RESOURCE_ID_WEAPON_NIGHTSTICK,
	RESOURCE_ID_WEAPON_NIGHT_VISION,
	RESOURCE_ID_WEAPON_PARACHUTE,
	RESOURCE_ID_WEAPON_PISTOL,
	RESOURCE_ID_WEAPON_POOLCUE,
	RESOURCE_ID_WEAPON_PUMP_SG,
	RESOURCE_ID_WEAPON_RIFLE,
	RESOURCE_ID_WEAPON_ROCKET_LAUNCHER,
	RESOURCE_ID_WEAPON_SATCHEL_CHARGES,
	RESOURCE_ID_WEAPON_SAWN_OFF_SG,
	RESOURCE_ID_WEAPON_SHOVEL,
	RESOURCE_ID_WEAPON_SILENCED_9MM,
	RESOURCE_ID_WEAPON_SKATEBOARD,
	RESOURCE_ID_WEAPON_SNIPER_RIFLE,
	RESOURCE_ID_WEAPON_SPRAY_CAN,
	RESOURCE_ID_WEAPON_TEAR_GAS,
	RESOURCE_ID_WEAPON_TEC9
};
static int ICON_LIST_64_length = (int)(sizeof(ICON_LIST_64) / sizeof(ICON_LIST_64[0]));



// static int generateNumber(int lower, int upper) {
// 	srand(time(NULL)); // seed the random number generator with the current time
//     int range = upper - lower + 1;
//     return rand() % range + lower;
// }



// Initialize the default settings
static void default_settings() {
	settings.hasArmour = false;
	settings.hasBreath = true;
	settings.isZoomed = false;
	settings.weekStartOffset = 6;
}

// Read settings from persistent storage
static void load_settings() {
	// Load the default settings
	default_settings();
	// Read settings from persistent storage, if they exist
	persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void save_settings() {
	persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	// Update the display based on new settings
	// update_display();
}



static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}
static void update_date() {
	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	static char s_buffer[10];
	char *timeFormat = "$%Y%m%d";
	if (settings.isZoomed) timeFormat = "$%y%m%d";
	strftime(s_buffer, sizeof(s_buffer), timeFormat, tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_money_layer, s_buffer);
}
static void update_icon() {
	if (settings.isZoomed) return;
	// APP_LOG(APP_LOG_LEVEL_INFO, "selecting.");

	// time_t now = time(NULL);
    // struct tm *tm_now = localtime(&now);
    // int day = tm_now->tm_mday;
    // int hour = tm_now->tm_hour;

	int weaponSeed = time(NULL);
	weaponSeed = weaponSeed - (weaponSeed % 3600); // hour timestamp
	srand(weaponSeed);

	// random but predectable
	// uses day and hour
	selectedWeapon = rand() % ICON_LIST_64_length;
	// APP_LOG(APP_LOG_LEVEL_INFO, "Done selected.");
    // printf("The number is: %d and %d,%d so %d\n", selectedWeapon, day, hour, ICON_LIST_64_length);
    // printf("The process: %lf\n", (hour+1)*100/(day+1)*ICON_LIST_64_length);
    // printf("The process: florred %d\n", (int)((hour+1)*100/(day+1)*ICON_LIST_64_length));
    // printf("The process: florred %lf\n", floor((hour+1)*100/(day+1)*ICON_LIST_64_length));
    // printf("The process: florred %d\n", (int)(floor((hour+1)*100/(day+1)*ICON_LIST_64_length)));

	gbitmap_destroy(s_weapon_bitmap);
	s_weapon_bitmap = gbitmap_create_with_resource(ICON_LIST_64[selectedWeapon]);
	bitmap_layer_set_bitmap(s_background_layer, s_weapon_bitmap);
}
static void update_per_hour() {
	time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    int minutes = tm_now->tm_min;
    if (minutes != 0) {
        return;
    }

	update_date();
	update_icon();
	layer_mark_dirty(star_canvas_layer); //manual update
}



static void star_1_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	int starWidth = 22;
	if (settings.isZoomed) starWidth += 4;
	GPathInfo usingShape = STAR_PATH_INFO;
	if (settings.isZoomed) usingShape = ZoomedSTAR_PATH_INFO;

	int xOffset = 4 - 1;
	if (settings.isZoomed) xOffset += 1;
	int yOffset = 64 + 26;
	if (settings.isZoomed) yOffset -= 12;
	int xGap = 1;
	// if (settings.isZoomed) xGap = 0;
	int maxStars = 6;
	if (settings.isZoomed) maxStars = 5;

	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	const int weekDay = tick_time->tm_wday;
	const int weekStars = (weekDay + settings.weekStartOffset) % 7;

	for (int i = 0; i < maxStars; i++) {
		int starNum = (maxStars - 1) - i;

		GPath *star_path = gpath_create(&usingShape);
		// Translate by (5, 5):
		gpath_move_to(star_path, GPoint(xOffset + (i * (starWidth + xGap)), yOffset));


		// Fill the path:
		GColor fillColor = GColorDarkGray;
		if (starNum <= weekStars || (weekStars >= maxStars && maxStars > 5)) fillColor = GColorYellow;

		graphics_context_set_fill_color(ctx, fillColor);
		gpath_draw_filled(ctx, star_path);
		// Stroke the path:
		// graphics_context_set_stroke_color(ctx, fillColor);
		gpath_draw_outline(ctx, star_path);

		stars_list[i] = star_path;
	}
}
static void stars_update_proc(Layer *layer, GContext *ctx) {
	if (stars_list[0] == NULL) star_1_update_proc(layer, ctx);

	// if (settings.isZoomed) xGap = 0;
	int maxStars = 6;
	if (settings.isZoomed) maxStars = 5;

	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	const int weekDay = tick_time->tm_wday;
	const int weekStars = (weekDay + settings.weekStartOffset) % 7;

	for (int i = 0; i < maxStars; i++) {
		int starNum = (maxStars - 1) - i;

		GPath *star_path = stars_list[i];

		// Fill the path:
		GColor fillColor = GColorDarkGray;
		if (starNum <= weekStars || (weekStars >= maxStars && maxStars > 5)) fillColor = GColorYellow;

		graphics_context_set_fill_color(ctx, fillColor);
		gpath_draw_filled(ctx, star_path);
		// Stroke the path:
		graphics_context_set_stroke_color(ctx, GColorClear);
		gpath_draw_outline(ctx, star_path);
	}
}

static void style_bar(int index, GContext *ctx, float fillPercent) {

	// Disable antialiasing (enabled by default where available)
	graphics_context_set_antialiased(ctx, false);

	int xOffset = 64 + 7 + 1;
	if (settings.isZoomed) xOffset -= 20;
	int yOffset = 26;
	if (settings.isZoomed) yOffset += 8;
	const int yGap = 7;
	const int barHeight = 7;
	int barWidth = 68;
	if (settings.isZoomed) barWidth += 18;
	if (settings.isZoomed) index = 0;

	const int finalWidth = barWidth * (float)fillPercent;

	GRect rect_bounds = GRect(xOffset, yOffset + ((barHeight + yGap) * index), finalWidth, barHeight);

	// Draw a rectangle
	graphics_draw_rect(ctx, rect_bounds);

	// Fill a rectangle with rounded corners
	const int corner_radius = 0;
	graphics_fill_rect(ctx, rect_bounds, corner_radius, GCornersAll);
}

static void armour_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorWhite);

	style_bar(0, ctx, 1);
}

static void breath_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	#if defined(PBL_HEALTH)
	// Use the step count metric
	HealthMetric metric = HealthMetricStepCount;

	// Create timestamps for midnight (the start time) and now (the end time)
	// time_t start = time_start_of_today();
	// time_t end = time(NULL);

	// Check step data is available
	// HealthServiceAccessibilityMask mask =
	// 	health_service_metric_accessible(metric, start, end);
	
	stepsPercent = 1 - ((float)health_service_sum_today(metric) / stepsGoal);
	#else
	// Health data is not available here
	#endif

	if (stepsPercent <= 0) {
		return;
	}


	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorCeleste);

	style_bar(1, ctx, stepsPercent);
}

static void health_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorRed);

	style_bar(2, ctx, batteryPercent);
}

static void armour_bg_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorLightGray);
	// Set the line color
	graphics_context_set_stroke_color(ctx, GColorDarkGray);

	// Set the stroke width (must be an odd integer value)
	graphics_context_set_stroke_width(ctx, 5);

	style_bar(0, ctx, 1);
}

static void breath_bg_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	if (stepsPercent <= 0) {
		return;
	}

	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorCadetBlue);
	// Set the line color
	graphics_context_set_stroke_color(ctx, GColorDarkGray);

	// Set the stroke width (must be an odd integer value)
	graphics_context_set_stroke_width(ctx, 5);

	style_bar(1, ctx, 1);
}

static void health_bg_update_proc(Layer *layer, GContext *ctx) {
	// Custom drawing happens here!

	// Set the fill color
	graphics_context_set_fill_color(ctx, GColorDarkCandyAppleRed);
	// Set the line color
	graphics_context_set_stroke_color(ctx, GColorDarkGray);

	// Set the stroke width (must be an odd integer value)
	graphics_context_set_stroke_width(ctx, 5);

	style_bar(2, ctx, 1);
}



static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	update_per_hour();
}

static void handle_battery(BatteryChargeState charge_state) {
	batteryPercent = (float)charge_state.charge_percent / 100;

	layer_mark_dirty(health_canvas_layer); //manual update
}



static void window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);



	window_set_background_color(s_window, GColorBlack);


	int fistSize = 64;
	if (settings.isZoomed) fistSize = 42;
	int font_gta_big_size = 30;
	if (settings.isZoomed) font_gta_big_size = 38;

	if (settings.isZoomed) {
		s_font_gta_original = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_PRICEDOWN_BL_FONT_38));
	} else {
		s_font_gta_original = fonts_load_custom_font(
			resource_get_handle(RESOURCE_ID_PRICEDOWN_BL_FONT_30));
	}
	const int side_spacing = 4;

	// Create GBitmap
	if (settings.isZoomed) {
		s_weapon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FIST_42);
	} else {
		s_weapon_bitmap = gbitmap_create_with_resource(ICON_LIST_64[0]);
	}

	// Create BitmapLayer to display the GBitmap
	int weaponYOffset = 0;
	if (settings.isZoomed) weaponYOffset = 2;
	s_background_layer = bitmap_layer_create(GRect(side_spacing, weaponYOffset, fistSize, fistSize));

	// Set the bitmap onto the layer and add to the window
	bitmap_layer_set_bitmap(s_background_layer, s_weapon_bitmap);
	bitmap_layer_set_background_color(s_background_layer, GColorClear);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));


	//creating font
	// Create the TextLayer with specific bounds
	int fontExtraHeight = -10;
	if (settings.isZoomed) fontExtraHeight = -13;
	s_time_layer = text_layer_create(GRect(side_spacing, fontExtraHeight + 2, bounds.size.w - (side_spacing * 2), font_gta_big_size));
	s_money_layer = text_layer_create(GRect(0, fistSize - 6, bounds.size.w - (side_spacing * 1) + 1, font_gta_big_size));
	if (settings.hasArmour && !settings.isZoomed) armour_bg_canvas_layer = layer_create(bounds);
	if (settings.hasArmour && !settings.isZoomed) armour_canvas_layer = layer_create(bounds);
	if (settings.hasBreath && !settings.isZoomed) breath_bg_canvas_layer = layer_create(bounds);
	if (settings.hasBreath && !settings.isZoomed) breath_canvas_layer = layer_create(bounds);
	health_bg_canvas_layer = layer_create(bounds);
	health_canvas_layer = layer_create(bounds);
	star_canvas_layer = layer_create(bounds);

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_font(s_time_layer, s_font_gta_original);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_money_layer, GColorClear);
	text_layer_set_text_color(s_money_layer, GColorMayGreen);
	text_layer_set_text(s_money_layer, "$350");
	text_layer_set_font(s_money_layer, s_font_gta_original);
	text_layer_set_text_alignment(s_money_layer, GTextAlignmentRight);

	if (settings.hasArmour && !settings.isZoomed) layer_set_update_proc(armour_bg_canvas_layer, armour_bg_update_proc);
	if (settings.hasArmour && !settings.isZoomed) layer_set_update_proc(armour_canvas_layer, armour_update_proc);
	if (settings.hasBreath && !settings.isZoomed) layer_set_update_proc(breath_bg_canvas_layer, breath_bg_update_proc);
	if (settings.hasBreath && !settings.isZoomed) layer_set_update_proc(breath_canvas_layer, breath_update_proc);
	layer_set_update_proc(health_bg_canvas_layer, health_bg_update_proc);
	layer_set_update_proc(health_canvas_layer, health_update_proc);
	layer_set_update_proc(star_canvas_layer, stars_update_proc);
	// layer_mark_dirty(armour_canvas_layer); //manual update
	// layer_mark_dirty(breath_canvas_layer); //manual update
	// layer_mark_dirty(health_canvas_layer); //manual update


	battery_state_service_subscribe(handle_battery);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_money_layer));
	if (settings.hasArmour && !settings.isZoomed) layer_add_child(window_layer, armour_bg_canvas_layer);
	if (settings.hasArmour && !settings.isZoomed) layer_add_child(window_layer, armour_canvas_layer);
	if (settings.hasBreath && !settings.isZoomed) layer_add_child(window_layer, breath_bg_canvas_layer);
	if (settings.hasBreath && !settings.isZoomed) layer_add_child(window_layer, breath_canvas_layer);
	layer_add_child(window_layer, health_bg_canvas_layer);
	layer_add_child(window_layer, health_canvas_layer);
	layer_add_child(window_layer, star_canvas_layer);

	handle_battery(battery_state_service_peek());
	update_date();
	update_icon();
	layer_mark_dirty(star_canvas_layer); //manual update
}

static void window_unload(Window *window) {
	battery_state_service_unsubscribe();

	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_money_layer);

	int maxStars = 6;
	if (settings.isZoomed) maxStars = 5;

	for (int i = 0; i < maxStars; i++) {
		gpath_destroy(stars_list[i]);
	}

	if (settings.hasArmour && !settings.isZoomed) layer_destroy(armour_canvas_layer);
	if (settings.hasArmour && !settings.isZoomed) layer_destroy(armour_bg_canvas_layer);
	if (settings.hasBreath && !settings.isZoomed) layer_destroy(breath_canvas_layer);
	if (settings.hasBreath && !settings.isZoomed) layer_destroy(breath_bg_canvas_layer);
	layer_destroy(health_canvas_layer);
	layer_destroy(health_bg_canvas_layer);
	layer_destroy(star_canvas_layer);

	fonts_unload_custom_font(s_font_gta_original);

	// Destroy GBitmap
	gbitmap_destroy(s_weapon_bitmap);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
}



static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	// Read preferences
	Tuple *has_armour_t = dict_find(iter, MESSAGE_KEY_HasArmour);
	if(has_armour_t) {
		settings.hasArmour = has_armour_t->value->int32 == 1;
	}

	Tuple *has_breath_t = dict_find(iter, MESSAGE_KEY_HasBreath);
	if(has_breath_t) {
		settings.hasBreath = has_breath_t->value->int32 == 1;
	}

	Tuple *is_zoomed_t = dict_find(iter, MESSAGE_KEY_IsZoomed);
	if(is_zoomed_t) {
		settings.isZoomed = is_zoomed_t->value->int32 == 1;
	}

	Tuple *week_start_t = dict_find(iter, MESSAGE_KEY_WeekStart);
	if(week_start_t) {
		settings.weekStartOffset = week_start_t->value->int32;
	}

	save_settings();
}



static void init(void) {
	load_settings();
    // printf("The weapons count is %d\n", ICON_LIST_64_length);

	// Open AppMessage connection
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(128, 128);



	s_window = window_create();

	window_set_window_handlers(s_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	const bool animated = true;
	window_stack_push(s_window, animated);



	// Make sure the time is displayed from the start
	update_time();

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
	tick_timer_service_unsubscribe();
	window_destroy(s_window);
}



int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing.");

	app_event_loop();
	deinit();
}
