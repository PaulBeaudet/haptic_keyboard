//keyDefinitions.h -- Copyright Paul Beaudet 2014 See license for reuse info
#define KB_INSERT      209 // defined in SpecialToKBD!
#define KB_HOME        210
#define KB_PAGE_UP     211
#define KB_DELETE      212
#define KB_END         213
#define KB_PAGE_DOWN   214
#define KB_RIGHT_ARROW 215
//Backspace
//tab
//new line
#define KB_LEFT_ARROW  216
#define KB_DOWN_ARROW  217
//cariage return
#define KB_UP_ARROW    218 // till here!
#define KB_FUNC_F1     194
#define KB_FUNK_F2     195
#define KB_FUNC_F3     196
#define KB_FUNK_F4     197
#define KB_FUNC_F5     198
#define KB_FUNK_F6     199
#define KB_FUNC_F7     200
#define KB_FUNK_F8     201
#define KB_FUNC_F9     202
#define KB_FUNK_F10    203
#define KB_FUNC_F11    204
#define KB_FUNK_F12    205
#define KB_ESC         177
#define KB_CAPS_LOCK   193

//control char Bluefruit presses  NOT USED 
#define BT_LEFT_CTRL      224
#define BT_LEFT_SHIFT     225
#define BT_LEFT_ALT       226
#define BT_LEFT_GUI       227
#define BT_RIGHT_CTRL     228
#define BT_RIGHT_SHIFT    229
#define BT_RIGHT_ALT      230
#define BT_RIGHT_GUI      231

//keycodes as understood by bluefruit and keyWriter
//These get passed around as the actual representations in tenkey codebase
#define INSERT         1
#define HOME           2
#define PAGE_UP        3
#define DELETE         4
#define END            5
#define PAGE_DOWN      6
#define RIGHT_ARROW    7
#define BACKSPACE      8    
#define TAB_KEY        9
#define NEW_LINE       10 //determines end of a message in buffer "10"
#define LEFT_ARROW     11
#define DOWN_ARROW     12
#define CARIAGE_RETURN 13
#define UP_ARROW       14
#define FUNC_F1        15
#define FUNK_F2        16
#define FUNC_F3        17
#define FUNK_F4        18
#define FUNC_F5        19
#define FUNK_F6        20
#define FUNC_F7        21
#define FUNK_F8        22
#define FUNC_F9        23
#define FUNK_F10       24
#define FUNC_F11       25
#define FUNK_F12       26
#define ESC            27
#define CAPS_LOCK      28
#define SCROLL_LOCK    29
#define BREAK          30
#define NUM_LOCK       31
#define SPACEBAR       32

// 33 through 127 are covered by printable characters
// 128 through 159 are covered by marcro modes
// 160 through 192
// mouse reports - both BT and USB use masked 1-2-4 convention
#define LEFT_CLICK_IN    160
#define RIGHT_CLICK_IN   162
#define MIDDLE_CLICK_IN  163
#define LEFT_CLICK_OUT   1
#define RIGHT_CLICK_OUT  2
#define MIDDLE_CLICK_OUT 4

// These are piped/ored|together to pass a modifier argument
#define LEFT_CTRL   1   //0 #define KB_LEFT_CTRL   128
#define LEFT_SHIFT  2   //1 #define KB_LEFT_SHIFT  129
#define LEFT_ALT    4   //2 #define KB_LEFT_ALT    130
#define LEFT_GUI    8   //3 #define KB_LEFT_GUI    131
#define RIGHT_CTRL  16  //4 #define KB_RIGHT_CTRL  132
#define RIGHT_SHIFT 32  //5 #define KB_RIGHT_SHIFT 133
#define RIGHT_ALT   64  //6 #define KB_RIGHT_ALT   134
#define RIGHT_GUI   128 //7 #define KB_RIGHT_GUI   135
