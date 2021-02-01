// [main.c]
// this template is provided for the 2D shooter game.

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <math.h>
#include <stdlib.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 800;
// Display (screen) height.
const int SCREEN_H = 600;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2,
    // [HACKATHON 3-7]
    // TODO: Declare a new scene id.
    SCENE_SETTINGS = 3,
    SCENE_GAMEOVER = 4
};

/* Input states */

// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_100;
ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;

//gameover
ALLEGRO_BITMAP* img_copyofstart;
ALLEGRO_BITMAP* img_gameover;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* start_img_plane2;
ALLEGRO_BITMAP* start_img_boss;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_bullet_player;
ALLEGRO_BITMAP* img_bullet_enemy;
ALLEGRO_BITMAP* img_bullet_boss_1_down;
ALLEGRO_BITMAP* img_bullet_boss_1_left;
ALLEGRO_BITMAP* img_bullet_boss_1_right;

ALLEGRO_COLOR color_rec;
ALLEGRO_COLOR color_life;
ALLEGRO_FONT* font_pirulen_12;
ALLEGRO_BITMAP* img_coin;

/*setting sources*/
ALLEGRO_SAMPLE* setting_bgm;
ALLEGRO_SAMPLE_ID setting_bgm_id;
ALLEGRO_BITMAP* setting_img_plane;
ALLEGRO_BITMAP* setting_img_plane2;

/*gameover sources*/
ALLEGRO_SAMPLE* gameover_bgm;
ALLEGRO_SAMPLE_ID gameover_bgm_id;

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.
    ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 3
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET_PLAYER 4
#define MAX_BULLET_ENEMY 4      //per enemy
#define MAX_BULLET_BOSS_1 10
MovableObject plane;
MovableObject plane2;
MovableObject boss;
int chooseplane=0;  //not be chose by user
int bossappear=0;
MovableObject enemies[MAX_ENEMY];
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
MovableObject bullets_player[MAX_BULLET_PLAYER];
MovableObject bullets_enemies[MAX_ENEMY][MAX_BULLET_ENEMY];
MovableObject bullets_boss_1[MAX_BULLET_BOSS_1];


// variables for life
#define MAX_LIFE_PLANE 250
#define MAX_LIFE_ENEMY 100
#define MAX_LIFE_BOSS 1000
#define DAMAGE_ENEMY 50
#define DAMAGE_PLANE 50
#define DAMAGE_BOSS 100
int life_plane;
int life_enemies[MAX_ENEMY];
int life_boss;
int win =0;


//variables for score
MovableObject coin;
int score_int;
char score_char[10];


// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
const float MAX_COOLDOWN_PLAYER = 0.2f;
double last_shoot_timestamp_player;
const float MAX_COOLDOWN_ENEMY = 0.2f;
double last_shoot_timestamp_enemies[MAX_ENEMY];
double last_shoot_timestamp_boss;
const float MAX_COOLDOWN_BOSS = 0.4f;

/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
    // Set random seed for better random outcome.
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");
    
    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");
    // TODO: Initialize other addons such as video, ...
    
    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "I2P(I)_2018_Yang Final Project <student_id>");
    
    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");
    
    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");
    
    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));
    
    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...
    
    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    /* Shared resources*/
    font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
    if (!font_pirulen_32)
        game_abort("failed to load font: pirulen.ttf with size 32");
    
    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");
    
    font_pirulen_12 = al_load_font("pirulen.ttf", 12, 0);
    if(!font_pirulen_12)
        game_abort("failed to load font: pirulen.ttf with size 12");
    
    font_pirulen_100 = al_load_font("pirulen.ttf", 100, 0);
    if(!font_pirulen_12)
        game_abort("failed to load font: pirulen.ttf with size 100");
    
    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);
    
    main_bgm = al_load_sample("S31-Night Prowler.ogg");
    if (!main_bgm)
        game_abort("failed to load audio: S31-Night Prowler.ogg");
    
    // [HACKATHON 3-4]
    // TODO: Load settings images.
    // Don't forget to check their return values.
    // Uncomment and fill in the code below.
    img_settings = al_load_bitmap("settings.png");
    if (!img_settings)
        game_abort("failed to load image: settings.png");
    img_settings2 = al_load_bitmap("settings2.png");
    if (!img_settings2)
        game_abort("failed to load image: settings2.png");
    
    /* Start Scene resources*/
    start_img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);
    
    start_img_plane = al_load_bitmap("plane.gif");
    //http://www.gamebase.com.tw/forum/39005/topic/91362736/1
    if (!start_img_plane)
        game_abort("failed to load image: plane.gif");
    
    start_img_plane2 = al_load_bitmap("plane2.png");
    //http://file06.16sucai.com/2016/0321/247b0ab26a1227f4975a660ebc35ee9a.jpg
    if(!start_img_plane2)
        game_abort("failed to load image: plane2.png");
    
    start_img_boss = al_load_bitmap("boss.png");
    if(!start_img_boss)
        game_abort("failed to load image: boss.png");
    
    start_img_enemy = al_load_bitmap("smallfighter0006.png");
    if (!start_img_enemy)
        game_abort("failed to load image: smallfighter0006.png");
    
    start_bgm = al_load_sample("mythica.ogg");
    if (!start_bgm)
        game_abort("failed to load audio: mythica.ogg");
    
    // [HACKATHON 2-5-1]
    // TODO: Initialize bullets.
    // 1) Search for a bullet image online and put it in your project.
    //    You can use the image we provided.
    // 2) Load it in by 'al_load_bitmap' or 'load_bitmap_resized'.
    // 3) If you use 'al_load_bitmap', don't forget to check its return value.
    // Uncomment and fill in the code below.
    img_bullet_player = al_load_bitmap("image12.png");
    if (!img_bullet_player)
        game_abort("failed to load image: image12.png");
    
    img_bullet_enemy = al_load_bitmap("bullet_down.png");
    if(!img_bullet_enemy)
        game_abort("failed to load image: bullet_down.png");
    
    img_bullet_boss_1_down = al_load_bitmap("bullet_down.png");
    if(!img_bullet_boss_1_down)
        game_abort("failed to load image: bullet_down.png");
    
    img_bullet_boss_1_left = al_load_bitmap("bullet_left.png");
    if(!img_bullet_boss_1_left)
        game_abort("failed to load image: bullet_left.png");
    
    img_bullet_boss_1_right = al_load_bitmap("bullet_right.png");
    if(!img_bullet_boss_1_right)
        game_abort("failed to load image: bullet_right.png");
    
    img_coin = al_load_bitmap("coin.png");
    //https://www.freepik.com/free-vector/dollar_2900482.htm#term=dollar%20coin&page=1&position=0
    if(!img_coin)
        game_abort("failed to load imge: coin.png");
    
    /*setting resources*/
    setting_bgm = al_load_sample("setting.ogg");
    if(!setting_bgm)
        game_abort("failed to load audio: In_the_Atmosphere.ogg");
    setting_img_plane = al_load_bitmap("plane_setting.gif");
    if(!setting_img_plane)
        game_abort("failed to load imge: plane_setting.gif");
    setting_img_plane2 = al_load_bitmap("plane2_setting.png");
    if(!setting_img_plane2)
        game_abort("failed to load imge: plane2_setting.png");
    
    //from youtube free resources
    
    /*gameover scene resources*/
    gameover_bgm = al_load_sample("gameover.ogg");
    if(!gameover_bgm)
        game_abort("failed to load audio: gameover.ogg");
    //from youtube free resources
    
    img_gameover = al_load_bitmap("gameover.png");
    //https://www.google.com/search?q=gameover&source=lnms&tbm=isch&sa=X&ved=0ahUKEwj3maqj7-LfAhWMdd4KHThjC5MQ_AUIDigB&biw=1440&bih=716#imgrc=sLWLaMma4JQT0M:
    if(!img_gameover)
        game_abort("failed to load image: gameover.png");
    
    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (event.mouse.dx != 0 || event.mouse.dy != 0) {
                // Event for mouse move.
                game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            } else if (event.mouse.dz != 0) {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more
        // entries inside Scene.
        
        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
            if (redraws > 1)
                game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}

void game_update(void) {
    if (active_scene == SCENE_START) {
        plane.vx = plane.vy = 0;
        if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
            plane.vy -= 1;
        if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
            plane.vy += 1;
        if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
            plane.vx -= 1;
        if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
            plane.vx += 1;
        // 0.71 is (1/sqrt(2)).
        plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
        plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
        // [HACKATHON 1-1]
        // TODO: Limit the plane's collision box inside the frame.
        //       (x, y axes can be separated.)
        // Uncomment and fill in the code below.
        //plane
        if ( ( plane.x - plane.w/2) < 0)
            plane.x = plane.w/2;
        else if ((plane.x + plane.w/2) > SCREEN_W)
            plane.x = SCREEN_W - plane.w/2;
        if ( (plane.y - plane.h/2)< 0)
            plane.y = plane.h/2;
        else if (plane.y + plane.h/2 > SCREEN_H)
            plane.y = SCREEN_H - plane.h/2;
        
        int i,j;
        
        //enemies
        for(i=0;i<MAX_ENEMY;i++){
            if(enemies[i].hidden == false){
                if(plane.x - enemies[i].x > 0){
                    enemies[i].vx = 2;
                }else{
                    enemies[i].vx = -2;
                }
                enemies[i].x += enemies[i].vx;
                for(j=0;j<i;j++){
                    if( abs((enemies[j].x - enemies[i].x)) < (enemies[j].w /2) )
                        enemies[i].x -= enemies[i].vx;
                }
                //be inside the box
                if( (enemies[i].x - enemies[i].w/2) < 0 )
                    enemies[i].x = enemies[i].w/2;
                else if( (enemies[i].x + enemies[i].w/2) > SCREEN_W )
                    enemies[i].x = SCREEN_W - enemies[i].w/2;
            }
        }
        
        
        // [HACKATHON 2-6]
        // TODO: Update bullet coordinates.
        // 1) For each bullets, if it's not hidden, update x, y
        // according to vx, vy.
        // 2) If the bullet is out of the screen, hide it.
        // Uncomment and fill in the code below.
        for (i=0;i<MAX_BULLET_PLAYER;i++) {
            if (bullets_player[i].hidden)
                continue;
            bullets_player[i].x += bullets_player[i].vx;
            bullets_player[i].y += bullets_player[i].vy;
            if (bullets_player[i].y < 0)
                bullets_player[i].hidden = true;
        }
        
        for (i=0;i<MAX_ENEMY;i++) {
            for(j=0;j<MAX_BULLET_ENEMY;j++){
                if (bullets_enemies[i][j].hidden)
                    continue;
                bullets_enemies[i][j].x += bullets_enemies[i][j].vx;
                bullets_enemies[i][j].y += bullets_enemies[i][j].vy;
                if (bullets_enemies[i][j].y > SCREEN_H)
                    bullets_enemies[i][j].hidden = true;
            }
        }
        
        // [HACKATHON 2-7]
        // TODO: Shoot if key is down and cool-down is over.
        // 1) Get the time now using 'al_get_time'.
        // 2) If Space key is down in 'key_state' and the time
        //    between now and last shoot is not less that cool
        //    down time.
        // 3) Loop through the bullet array and find one that is hidden.
        //    (This part can be optimized.)
        // 4) The bullet will be found if your array is large enough.
        // 5) Set the last shoot time to now.
        // 6) Set hidden to false (recycle the bullet) and set its x, y to the
        //    front part of your plane.
        // Uncomment and fill in the code below.
        double now = al_get_time();
        if (key_state[ALLEGRO_KEY_SPACE] && (now - last_shoot_timestamp_player) >= MAX_COOLDOWN_PLAYER) {
            for (i=0;i<MAX_BULLET_PLAYER;i++) {
                if (bullets_player[i].hidden)
                    break;
            }
            if (i == MAX_BULLET_PLAYER){}
            else{
                last_shoot_timestamp_player = now;
                bullets_player[i].hidden = false;
                bullets_player[i].x = plane.x;
                bullets_player[i].y = plane.y - plane.h / 2 - 1;
            }
        }
        
        for(i=0;i<MAX_ENEMY;i++){
            if(enemies[i].hidden == false){
                if((now - last_shoot_timestamp_enemies[i]) >= MAX_COOLDOWN_ENEMY){
                    for(j=0;j<MAX_BULLET_ENEMY;j++){
                        if(bullets_enemies[i][j].hidden)
                            break;
                    }
                    if(j == MAX_BULLET_ENEMY)
                        continue;
                    last_shoot_timestamp_enemies[i] = now;
                    bullets_enemies[i][j].hidden = false;
                    bullets_enemies[i][j].x = enemies[i].x;
                    bullets_enemies[i][j].y = enemies[i].y + enemies[i].h / 2;
                }
            }
        }
        
        //life_plane
        for(i=0;i<MAX_ENEMY;i++){
            for(j=0;j<MAX_BULLET_ENEMY;j++){
                if( bullets_enemies[i][j].hidden == false){
                    if( abs(bullets_enemies[i][j].x - plane.x) < (bullets_enemies[i][j].w/2 + plane.w/2) ){
                        if( abs(bullets_enemies[i][j].y - plane.y) < (bullets_enemies[i][j].h/2 + plane.h/2) ){
                            bullets_enemies[i][j].hidden = true;
                            life_plane -= DAMAGE_ENEMY;
                            if(life_plane<=0)
                                game_change_scene(SCENE_GAMEOVER);
                        }
                    }
                }
            }
        }
        for(i=0;i<MAX_BULLET_BOSS_1;i++){
            if (bullets_boss_1[i].hidden == false){
                if( abs(bullets_boss_1[i].x - plane.x) < (bullets_boss_1[i].w/2 + plane.w/2) ){
                    if( abs(bullets_boss_1[i].y - plane.y) < (bullets_boss_1[i].h/2 + plane.h/2) ){
                        bullets_boss_1[i].hidden = true;
                        life_plane -= DAMAGE_BOSS;
                        if(life_plane<=0)
                            game_change_scene(SCENE_GAMEOVER);
                    }
                }
            }
        }
        
        //life_enemies
        for(i=0;i<MAX_BULLET_PLAYER;i++){
            if(bullets_player[i].hidden == false){
                for(j=0;j<MAX_ENEMY;j++){
                    if(enemies[j].hidden == false){
                        if( abs(bullets_player[i].x - enemies[j].x) < (bullets_player[i].w/2 + enemies[j].w/2) ){
                            if(abs(bullets_player[i].y - enemies[j].y) < (bullets_player[i].h/2 + enemies[j].h/2)){
                                bullets_player[i].hidden = true;
                                life_enemies[j] -= DAMAGE_PLANE;
                                if(life_enemies[j]<=0){
                                    score_int += 1000;
                                    enemies[j].hidden = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        //life_boss
        if(boss.hidden == false){
            for(i=0;i<MAX_BULLET_PLAYER;i++){
                if (bullets_player[i].hidden == false){
                    if( abs(bullets_player[i].x - boss.x) < (bullets_player[i].w/2 + boss.w/2) ){
                        if( abs(bullets_player[i].y - boss.y) < (bullets_player[i].h/2 + boss.h/2) ){
                            bullets_player[i].hidden = true;
                            life_boss -= DAMAGE_PLANE;
                            if(life_boss<=0){
                                win = 1;
                                score_int += 5000;
                                boss.hidden = true;
                            }
                        }
                    }
                }
            }
        }
        
        bossappear =1;
        for(i=0;i<MAX_ENEMY;i++){
            if( life_enemies[i] !=0){
                bossappear = 0;
                break;
            }
        }
        if(bossappear && win ==0)
            boss.hidden = false;
        if(boss.hidden == false){
            if(boss.x <400)
                boss.x += boss.vx;
            if(boss.y <100)
                boss.y += boss.vy;
        }
        for (i=0;i<MAX_BULLET_BOSS_1;i++) {
            if (bullets_boss_1[i].hidden)
                continue;
            bullets_boss_1[i].x += bullets_boss_1[i].vx;
            bullets_boss_1[i].y += bullets_boss_1[i].vy;
            if (bullets_boss_1[i].y > SCREEN_H)
                bullets_boss_1[i].hidden = true;
        }
        
        now = al_get_time();
        if(boss.hidden == false){
            if( (now - last_shoot_timestamp_boss) >= MAX_COOLDOWN_BOSS) {
                for (i=0;i<MAX_BULLET_BOSS_1;i++) {
                    if (bullets_boss_1[i].hidden)
                        break;
                }
                if(i != MAX_BULLET_BOSS_1){
                    last_shoot_timestamp_boss = now;
                    bullets_boss_1[i].hidden = false;
                    bullets_boss_1[i].x = plane.x;
                    bullets_boss_1[i].y = boss.y + boss.h / 2;
                }
            }
        }
        
    }
}

void game_draw(void) {
    if (active_scene == SCENE_MENU) {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Space Shooter");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key / click screen to start");
        // [HACKATHON 3-5]
        // TODO: Draw settings images.
        // The settings icon should be located at (x, y, w, h) =
        // (SCREEN_W - 48, 10, 38, 38).
        // Change its image according to your mouse position.
        // Uncomment and fill in the code below.
        if (pnt_in_rect(mouse_x, mouse_y, (SCREEN_W - 48), 10, 38, 38))
            al_draw_bitmap(img_settings2, (SCREEN_W - 48 ), 10, 0);
        else
            al_draw_bitmap(img_settings, (SCREEN_W - 48) , 10 , 0);
    } else if (active_scene == SCENE_START) {
        int i,j;
        al_draw_bitmap(start_img_background, 0, 0, 0);
        // [HACKATHON 2-8]
        // TODO: Draw all bullets in your bullet array.
        // Uncomment and fill in the code below.
        for (i=0;i<MAX_BULLET_PLAYER;i++)
            draw_movable_object(bullets_player[i]);
        draw_movable_object(plane);
        for (i = 0; i < MAX_ENEMY; i++){
            draw_movable_object(enemies[i]);
            for(j=0;j<MAX_BULLET_ENEMY;j++){
                draw_movable_object(bullets_enemies[i][j]);
            }
        }
        draw_movable_object(boss);
        for(i=0;i<MAX_BULLET_BOSS_1;i++){
            draw_movable_object(bullets_boss_1[i]);
        }
        
        //life
        color_rec = al_map_rgb(38,38,38);
        color_life = al_map_rgb(0,255,65);
        
        al_draw_rectangle(20,14,272,25,color_rec,4);
        al_draw_filled_rectangle(21,15,271,24,al_map_rgb(231,222,226));
        if(life_plane+21 >0)
            al_draw_filled_rectangle(21,15, (life_plane+21) ,24,color_life);
        al_draw_text(font_pirulen_12, color_rec, 20, 27, ALLEGRO_ALIGN_LEFT,"HP");
        
        //score
        al_draw_bitmap(coin.img, coin.x, coin.y, 0);
        al_draw_filled_rectangle(coin.x + 30, coin.y, coin.x + 110, coin.y + 20 ,al_map_rgb(255,255,0));
        sprintf(score_char,"%d",score_int);
        al_draw_text(font_pirulen_12,al_map_rgb(28,28,28), coin.x + 100,coin.y+5,ALLEGRO_ALIGN_RIGHT,score_char);
        
        if(win){
            al_draw_text(font_pirulen_100,al_map_rgb(229,0,11),30,250,ALLEGRO_ALIGN_LEFT,"You Win !!!");
            if (pnt_in_rect(mouse_x, mouse_y,650, 560, 140, 20))
                al_draw_filled_rectangle(650,560,790,580,al_map_rgb(73,6,248));
            else
                al_draw_filled_rectangle(650,560,790,580,al_map_rgb(83,138,139));
            al_draw_text(font_pirulen_12,al_map_rgb(250, 250, 250),720,563,ALLEGRO_ALIGN_CENTER,"play again");
        }
        
    }
    // [HACKATHON 3-9]
    // TODO: If active_scene is SCENE_SETTINGS.
    // Draw anything you want, or simply clear the display.
    else if (active_scene == SCENE_SETTINGS) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font_pirulen_32,al_map_rgb(250, 250, 250),100,50,ALLEGRO_ALIGN_LEFT,"Choose your plane!");
        if(chooseplane == 0){
            if (pnt_in_rect(mouse_x, mouse_y,100, 100, 280, 400))
                al_draw_filled_rectangle(100, 100, 380, 500, al_map_rgb(39,25,178));
            else
                al_draw_filled_rectangle(100, 100, 380, 500, al_map_rgb(140,140,140));
            
            if (pnt_in_rect(mouse_x, mouse_y,420, 100, 280, 400))
                al_draw_filled_rectangle(420, 100, 700, 500, al_map_rgb(39,25,178));
            else
                al_draw_filled_rectangle(420, 100, 700, 500, al_map_rgb(140,140,140));
        }else if(chooseplane == 1){
            al_draw_filled_rectangle(100, 100, 380, 500, al_map_rgb(39,25,178));
            if (pnt_in_rect(mouse_x, mouse_y,420, 100, 280, 400))
                al_draw_filled_rectangle(420, 100, 700, 500, al_map_rgb(39,25,178));
            else
                al_draw_filled_rectangle(420, 100, 700, 500, al_map_rgb(140,140,140));
        }else if(chooseplane ==2){
            if (pnt_in_rect(mouse_x, mouse_y,100, 100, 280, 400))
                al_draw_filled_rectangle(100, 100, 380, 500, al_map_rgb(39,25,178));
            else
                al_draw_filled_rectangle(100, 100, 380, 500, al_map_rgb(140,140,140));
            al_draw_filled_rectangle(420, 100, 700, 500, al_map_rgb(39,25,178));
        }
        al_draw_bitmap(setting_img_plane,105,100+(400-222)/2,0);
        al_draw_bitmap(setting_img_plane2,425,100+(400-344)/2,0);
        
        if (pnt_in_rect(mouse_x, mouse_y,650, 560, 140, 20))
            al_draw_filled_rectangle(650,560,790,580,al_map_rgb(73,6,248));
        else
            al_draw_filled_rectangle(650,560,790,580,al_map_rgb(83,138,139));
        al_draw_text(font_pirulen_12,al_map_rgb(250, 250, 250),720,563,ALLEGRO_ALIGN_CENTER,"back to menu");
    }
    
    else if(active_scene == SCENE_GAMEOVER){
        //want to take the start scene as background but....
        //?? al_clone_bitmap(img_copyofstart);
        //?? al_draw_tinted_bitmap("scene start image",(al_map_rgba_f(0.5, 0.5, 0.5, 0.5)),0,0,0);
        //?? al_draw_bitmap(img_copyofstart,0,0,0);
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(img_gameover,160,50,0);
        if (pnt_in_rect(mouse_x, mouse_y,700, 560, 90, 20))
            al_draw_filled_rectangle(700,560,790,580,al_map_rgb(73,6,248));
        else
            al_draw_filled_rectangle(700,560,790,580,al_map_rgb(83,138,139));
        al_draw_text(font_pirulen_12,al_map_rgb(250, 250, 250),745,563,ALLEGRO_ALIGN_CENTER,"restart");
        
    }
    
    al_flip_display();
}

void game_destroy(void) {
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);
    
    /* Menu Scene resources*/
    al_destroy_bitmap(main_img_background);
    al_destroy_sample(main_bgm);
    // [HACKATHON 3-6]
    // TODO: Destroy the 2 settings images.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_settings2);
    
    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
    al_destroy_bitmap(start_img_plane);
    al_destroy_bitmap(start_img_enemy);
    al_destroy_sample(start_bgm);
    // [HACKATHON 2-9]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_bullet_player);
    al_destroy_bitmap(img_bullet_enemy);
    
    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
    
    al_destroy_font(font_pirulen_12);
    al_destroy_font(font_pirulen_100);
    al_destroy_bitmap(img_coin);
    al_destroy_bitmap(img_gameover);
    al_destroy_bitmap(img_copyofstart);
    al_destroy_sample(gameover_bgm);
    al_destroy_bitmap(start_img_plane2);
    al_destroy_sample(setting_bgm);
    al_destroy_bitmap(setting_img_plane);
    al_destroy_bitmap(setting_img_plane2);
    al_destroy_bitmap(start_img_boss);
    al_destroy_bitmap(img_bullet_boss_1_down);
    al_destroy_bitmap(img_bullet_boss_1_left);
    al_destroy_bitmap(img_bullet_boss_1_right);
}

void game_change_scene(int next_scene) {
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU) {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    } else if (active_scene == SCENE_START) {
        al_stop_sample(&start_bgm_id);
        game_log("stop audio (bgm)");
    } else if(active_scene == SCENE_SETTINGS){
        al_stop_sample(&setting_bgm_id);
        game_log("stop audio (bgm)");
    } else if (active_scene == SCENE_GAMEOVER){
        al_stop_sample(&gameover_bgm_id);
        game_log("stop audio (bgm)");
    }
    active_scene = next_scene;
    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU) {
        if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    } else if (active_scene == SCENE_START) {
        if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
            game_abort("failed to play audio (bgm)");
        win = 0;
        
        int i,j;
        if(chooseplane <=1)
            plane.img = start_img_plane;
        else if(chooseplane == 2)
            plane.img = start_img_plane2;
        plane.x = 400;
        plane.y = 500;
        plane.w = plane.h = 51;
        for (i = 0; i < MAX_ENEMY; i++) {
            enemies[i].w = 28;
            enemies[i].h = 68;
            enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
            enemies[i].y = 80;
            enemies[i].img = start_img_enemy;
            enemies[i].hidden = false;
        }
        // [HACKATHON 2-5-2]
        // TODO: Initialize bullets.
        // For each bullets in array, set their w and h to the size of
        // the image, and set their img to bullet image, hidden to true,
        // (vx, vy) to (0, -3).
        // Uncomment and fill in the code below.
        for (i=0;i<MAX_BULLET_PLAYER;i++) {
            bullets_player[i].w = 30;
            bullets_player[i].h = 60;
            bullets_player[i].img = img_bullet_player;
            bullets_player[i].vx = 0;
            bullets_player[i].vy = -3;
            bullets_player[i].hidden = true;
        }
        for(i=0;i<MAX_ENEMY;i++){
            for(j=0;j<MAX_BULLET_ENEMY;j++){
                bullets_enemies[i][j].w = 30;
                bullets_enemies[i][j].h = 60;
                bullets_enemies[i][j].img = img_bullet_enemy;
                bullets_enemies[i][j].vx = 0;
                bullets_enemies[i][j].vy = 2;
                bullets_enemies[i][j].hidden = true;
            }
        }
        
        //boss
        bossappear = 0;
        boss.w = 900;
        boss.h = 364;
        boss.x = -boss.w ;
        boss.y = -boss.h;
        boss.img = start_img_boss;
        boss.vx = 20;
        boss.vy = 10;
        boss.hidden = true;
        
        for(i=0;i<MAX_BULLET_BOSS_1;i++){
            bullets_boss_1[i].w = 30;
            bullets_boss_1[i].h = 60;
            bullets_boss_1[i].hidden = true;
            if( ((i+1)%3) == 1){
                bullets_boss_1[i].vx = -2;
                bullets_boss_1[i].vy = 2;
                bullets_boss_1[i].img = img_bullet_boss_1_left;
            }else if(((i+1)%3) == 2){
                bullets_boss_1[i].vx = 0;
                bullets_boss_1[i].vy = 3;
                bullets_boss_1[i].img = img_bullet_boss_1_down;
            }else if(((i+1)%3) == 0){
                bullets_boss_1[i].vx = 2;
                bullets_boss_1[i].vy = 2;
                bullets_boss_1[i].img = img_bullet_boss_1_right;
            }
        }
        
        //life
        life_plane = MAX_LIFE_PLANE;
        for(i=0;i<MAX_ENEMY;i++)
            life_enemies[i] = MAX_LIFE_ENEMY;
        life_boss = MAX_LIFE_BOSS;
        
        //score
        coin.x = 650;
        coin.y = 15;
        coin.w = 10;
        coin.h = 10;
        coin.img = img_coin;
        coin.vx = 0;
        coin.vy = 0;
        coin.hidden = false;
        score_int = 0;
    
    }else if(active_scene == SCENE_SETTINGS){
        if(!al_play_sample(setting_bgm, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP,&setting_bgm_id))
            game_abort("failed to play audio (bgm)");
    }else if(active_scene == SCENE_GAMEOVER){
        if (!al_play_sample(gameover_bgm, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &gameover_bgm_id))
            game_abort("failed to play audio (bgm)");
        
    }
}

void on_key_down(int keycode) {
    if (active_scene == SCENE_MENU) {
        if (keycode == ALLEGRO_KEY_ENTER)
            game_change_scene(SCENE_START);
    }
}

void on_mouse_down(int btn, int x, int y) {
    // [HACKATHON 3-8]
    // TODO: When settings clicked, switch to settings scene.
    // Uncomment and fill in the code below.
    if (active_scene == SCENE_MENU) {
        if (btn == 1) {
            if (pnt_in_rect(x, y, (SCREEN_W - 48), 10, 38, 38))
                game_change_scene(SCENE_SETTINGS);
            else
                game_change_scene(SCENE_START);
        }
    }else if(active_scene == SCENE_GAMEOVER){
        if (btn ==1){
            if(pnt_in_rect(x,y,700, 560, 90, 20))
                game_change_scene(SCENE_MENU);
        }
    }else if(active_scene == SCENE_SETTINGS){
        if (btn ==1){
            if(pnt_in_rect(x,y,650, 560, 140, 20))
                game_change_scene(SCENE_MENU);
            if(pnt_in_rect(x,y,100, 100, 280, 400)){
                chooseplane = 1;
            }
            if(pnt_in_rect(x,y,420, 100, 280, 400)){
                chooseplane = 2;
            }
        }
    }else if(active_scene == SCENE_START){
        if (btn ==1){
            if(pnt_in_rect(x,y,650, 560, 140, 20))
                game_change_scene(SCENE_MENU);
        }
    }
}

void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();
    
    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
                          al_get_bitmap_width(loaded_bmp),
                          al_get_bitmap_height(loaded_bmp),
                          0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);
    
    game_log("resized image: %s", filename);
    
    return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {     //圖片的座標點是左上角的值
    if( (px <= (x + w)) && (px >= x ) ){
        if( (py <= (y + h)) && (py >= y) ){
            return true;
        }
    }
    return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}
