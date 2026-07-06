/*!\file window.c
 * \author Farès BELHADJ, amsi@up8.edu
 * \author Anparasan ANPUKKODY
 * \date November 25, 2024
 */

#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL.h>
#include <SDL_mixer.h>

static char version = 1;


// #define MAX_TRAIL_POINTS 400
#define MAX_TRAIL_POINTS 1000
// #define NUM_STARS 4000
#define NUM_STARS 30000

#define CHOCO_W 100
#define CHOCO_H 68
#define NUM_STARS_CHOCO (CHOCO_W * CHOCO_H)

#define CREDIT1_W 150
#define CREDIT1_H 25
#define NUM_STARS_CREDIT1 (CREDIT1_W * CREDIT1_H)

#define CREDIT2_W 200
#define CREDIT2_H 53
#define NUM_STARS_CREDIT2 (CREDIT2_W * CREDIT2_H)

// #define CREDIT3_W 300
// #define CREDIT3_H 48
#define CREDIT3_W 300
#define CREDIT3_H 50
#define NUM_STARS_CREDIT3 (CREDIT3_W * CREDIT3_H)

#define CREDIT4_W 200
#define CREDIT4_H 33
#define NUM_STARS_CREDIT4 (CREDIT4_W * CREDIT4_H)

Mix_Music* gMusic = NULL;

typedef struct vec3 {
  GLfloat x;
  GLfloat y;
  GLfloat z;
} vec3;

typedef struct {
  int index;
  vec3 pos;
  vec3 trailPos[MAX_TRAIL_POINTS];
  int trailCount;
  float rotationLeftRight;
  float rotationUpDown;
  vec3 color;
  vec3 vD;
} Sphere;

Sphere spheres[2];

Sphere *s1 = &spheres[0];
Sphere *s2 = &spheres[1];

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

static void initStars(void);
static void drawStars(float fade);
static void initStarsChoco(void);
static void drawStarsChoco(float fade);
static void initStarsCredit1(void);
static void drawStarsCredit1(float fade);
static void initStarsCredit2(void);
static void drawStarsCredit2(float fade);
static void initStarsCredit3(void);
static void drawStarsCredit3(float fade);
static void initStarsCredit4(void);
static void drawStarsCredit4(float fade);

static void reset_trail(void);
static void calculate_trail(GLfloat x, GLfloat y, GLfloat z, short maxTrailCount, Sphere *sphere);
static vec3 posConv(const short *path, short index);
static void catmull_position(const short pathSphere[], short numPoints, float t1, float totalPath, Sphere *sphere);

static void draw_init(float fade, float intensity);
static void draw_sphere(Sphere *sphere, char isTrail, short nbTrail);
static void draw_explosion(GLfloat xT, GLfloat yT, GLfloat zT, float increase, float dt, char initExplosion);

static void scene1(void);
static void scene2(void);
static void scene3(void);
static void scene4(void);
static void scene5(void);
static void scene6(void);
static void scene7(void);
static void scene8(void);
static void scene9(void);
static void scene10(void);
static void scene11(void);
static void scene12(void);
static void scene13(void);
static void scene14(void);

int getCharWeight(char c);

static void draw_clouds(float intensity);

static vec3 catmull_rom(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3);

/*!\brief largeur et hauteur de la fenêtre */
// static int _wW = 1366, _wH = 760;
// static int _wW = 1660, _wH = 900;
static int _wW = 1920, _wH = 1080;

/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
static GLuint _ribbon = 0;
static GLuint _pIdStars = 0;
static GLuint _color = 0;
static GLuint _explosion = 0;
static GLuint _isExplosion = 0;
static GLuint _sphere = 0;
static GLuint _timeStars = 0;
static GLuint _isBack = 0;
static GLuint _fade = 0;
static GLuint _isCredit = 0;
static GLuint _colorIntensity = 0;
static GLuint _timeClouds = 0;

static float ribbonVertices[MAX_TRAIL_POINTS * 2 * 3];

static GLuint _trailVao = 0;
static GLuint _trailVbo = 0;

static GLuint _starsVao = 0;
static GLuint _starsVbo = 0;

static GLuint _chocoVao = 0;
static GLuint _chocoVbo = 0;

static GLuint _credit1Vao = 0;
static GLuint _credit1Vbo = 0;

static GLuint _credit2Vao = 0;
static GLuint _credit2Vbo = 0;

static GLuint _credit3Vao = 0;
static GLuint _credit3Vbo = 0;

static GLuint _credit4Vao = 0;
static GLuint _credit4Vbo = 0;

static GLuint _pIdClouds = 0;

static int currentScene = 1;

static float xCam = 0.0f;
static float yCam = 0.0f;
static float zCam = 0.0f;

static float xEye = 0.0f;
static float yEye = 0.0f;
static float zEye = 0.0f;

static float xUp = 0.0f;
static float yUp = 0.0f;
static float zUp = 0.0f;

static char sceneInit = 1;

static float t0 = 0.0f;
static float t1 = 0.0f;
static float t;
static float dt;
static float lastFrame = 0.0f;
static float sceneTimeBegin = 0.0f;;
static float currentTime;


static int _numStarsChoco = 0;
static int _numStarsC1 = 0;
static int _numStarsC2 = 0;
static int _numStarsC3 = 0;
static int _numStarsC4 = 0;

static char choco[] =

"                                     WWWWWNNNNNNNNNNNNNWWWWWWWWWWWWWWWWWWWWW                        "
"                                 WWWNNNNNXXXXXXXXXXXXXNNXXXXXXXXXXXXXXXXXXXXNNWWW                   "
"                               WWNNNNNNXXXXXKKKKXXXXXXXXXXXKKXXXXKKXXXXXKKKKK00KKXNNWWW             "
"                              WNNNNXXXXXXKKKKKKXXXXXXXXXXKKKKKKKKKKKKXXXXKK0OkkxkO0KKKXXNW          "
"                            WNXXXXXXKKKKKK000KXXXXXKKKKKKKKKKKKKKKKKKXXXXXKKOkxddxk0KKKKKXXWW       "
"                          WNXK0KKKKK000000000KKKKKKKKKK00000000KKKKKKKXXXXXKKOxoclok00000000KXNW    "
"                        WWX0OO00000OOkOO0OOO00KKKK00OO0000OO00000KKKKKXXXXXXXK:::;cdkOOOOOOOkkOKNW  "
"                       WX0kxxkkOOOOkkkOOOOOO0KKK0OOkxxxkkOOOO00000KKKKKXXXXXXXK:::;oxkkkkkOkxddxOKW "
"                      NO;::ldxkkOkkkkOOOOOOO0000OxxxddddxkO00000000KKKKKXXXXXXX:::;ldxxxxxkkdoodxk0X"
"                     WX;'.':oxkkOOOOOOOOOkkOOOkxdllllllodxkO0KKKK0000KKKKXXXXXXKk;;cdddddxxxdlclooxk"
"                     WN;:;:clxkO00000000Okkkkxxolc:::::clodxkO00KKKKKKKKKKKKXXKKO;:cdddoddxxdc::clox"
"                     WNk;:ldkO0000000000Okxxxdlc;,,,.. ..,:coxkkO0KKKKKKKKKKKKKKO::codooodddoc::clod"
"                     WNOxkO0K0000000000Okxddoc:,'...      .,;codxkO0KKKKKKKKKK00ku;;odooodddol::ccod"
"                    WWNKKK0000000OOOOOOOkxdlcc:,'...   ;  ....';ldkO000K00KKK00Oxo;;odoooddoolc::clo"
"                WNNXK0000OOOOOOOOOOOOOkkkxdolccc;,'............';cdkO000000000Okd;:lodooooooollc:clo"
"             WNK0OOOkkkkkkxxxxkkkkkkkkkkkxxdoooolccc:;,,,,,;:clooodxkOOO000000Oko::lodooodooolllccco"
"         WXKOkkxxxxxxxdddoodddxxkkkkkkkkkkxxdooddddddoolooodxxkkkxxxxO00000000Oko::lodddddoolllllllo"
"      WX0kdoollllccccclllllloodxxkkkkkkkkxxxxddxxxxxxxxxdxxxkkkkxxkkkO00KK0000Oxl;:loddddooolllllooo"
"    NKkdollllcc::;;;;;::ccllooodxxkkkkxxxxxxxxxxxkkkkkkxxxxxkkkkkkOOOO00KKK000Oxc;:loooooooolloooodd"
"   Xxolcccccc:::;;;,,,,;:cllloooddxkkkkxxxxxxxxxkkkOOkkxxxxkkkkkOOOOO000KKK000kd:;cooooooooo,ooooddd"
"  Wk:;:::::::;;;;;,,,,'',:llllooddxxkkkkkkkkkkkkkOOOOOkkxxxxkkOOOOO00000KKK00Oxo::lolloooooooolllool"
"  Wk;.';;;;,'..'',,,,,''',ccclooddxxkkOOkkkkkkkkkkOOOOOOkkxxkkOOOOO000000KK00Oxo::llllloooooolllcccl"
"   Nx,.';,'.   .'''''''..';:clodxxxkkkOOOOOkkkkkkkkkOOO000OOOOOO0000000000KK0Oxl::lllllooooollccccld"
"    0c,;,''.   .''''''..',;ccloxxkkkOOOOkOkkkkkkkkkkkOOO00KKK0000000000OOOO00Oxc;:lllclooooolcccodkK"
"    Xo::,'''...........',;:clodxkkkOOOOOkkkkkkkkkkkkkkOO000KKKKKKKKKK0OkxxkOOOxc;;clcollooollloxO0KX"
"    W0l;''''..........',::cllddxkkkOOOOkOkkkkkkkkkkkOOO000000KKKKKKKK0Okxdxkkxoc;:cloclloooodxkO0KKK"
"     WKd:,'.........'',;:cllodddxkkkkkkkkkkkkkkkkOOOOOOO000000KKK0000OOkkxxxxdoc::cllclllodxkkO0KKKK"
"      W0d:,'''...'''',;:clooddddxxkkkkkkkkkkkkOOOOOOOOOkkOOOO00KKKKK0OOOOkxdddolccclllllodkkkO0KKKKK"
"       N0dc;;,'..''',;:clloddddddxkkkkOkkkkkOOOOOOOOOOOkkkxxxkkO00KK0OO0OkxddoolloolllloxkOOO00KKKKK"
"        WKxocc:;,''',;:cllodddxxxxkkkkOOOOOOOO00000OOOkkkxddollloxO00OOOOkdddooodddddddkkkOO00KKKKKK"
"         WXOo:;:cll:;,;cllodddxxxxkkkkOOOOOOOO0OOOOkkxddol:;,'.,:dOOkOkkxdddddddxxxxkkkkkkO00KKKKKKK"
"           WX0kk0NNXOdlc:cllodxxxxxkxkkOOkOOOOOkxxddoc,....   .:okOkkkxdddddddxxxxxkkxxkOO000KK000KK"
"                    WKxlccccloddddxxxxkkkOkkkxxdoc;,...  .....;okOOkxddddxxxxxkkxkkkxxxkO0000KK00KKX"
"                WNKOxocdooodoclllodddxxxdddxdolc,.....  ....':dkkkkxxxxxxxxkkkkkkkxxxxOO0000000KKKXX"
"              WXOdcddooodooodooodoooclloc:loc;'.....  ....':oxkkkkxxxxxkkkkkkkkxxxxxkOO0000000KKXXXX"
"             N0xoccdooodooodooodooooclol:,'............';cdxkkkkkkkkkkkkkkkkkxxdxxkkOO000000KKKKXXXX"
"            W0dooodooodooodooodooollodd:...... .....',;ldkkkOOOOOOkkkkkkkkxxdddxkkOOOOOOO00KKKXXXXKK"
"           WKkdollllllccloodooolooddddl,.........',;coxkkkOO00000OOkkkkkxxxxxxkkOOOOOOO00KKKXXXKKKK0"
"           N0xolldooodooodooodooodoooc,''...'';coxkO00OOO00000000OkkkkkkxxxxkkOOOkkOO00KKKXXXKK00000"
"           Xkdodooodooodooodooodoooo:'.'''',cxKNNXXXXKK000000000OkxkkOOOkkkkkkkkkkO000KKXXXKK0000KKK"
"           Xkollllllloooddddddodddo:,''',;cdKW   WNNXXXK0000OOOOkkkkOOOOOkkkxxkkO00KKKKXXXKKKKKKKKK0"
"           NOdoolllllooooodddoodol:,,,;:okKN      WWNXXXKKKK0000OkkOOOkkkxxxkO00KKKKKKKKKKKKKKKKKKK0"
"           WXOxdoollodddddddoolc:;:coxOKNW          WNXXXKKKK000OOOkkxxdxxkkO000KKKKKKKKKKKKKKKK0000"
"            WXOxoolodxxdddddxkxxkOKXW               WNXXXXXXK00OOOOkxxxxkkOO00000000KKKKKKKKKKK00000"
"              WXKOkkkxxxxk0KNWWW                     WNNNNXXK00OOOOkkkkOOOO00000000KKKKKKKKKK000000K"
"                   WNXXXNW                            WNNNNXXK00OkkOOOOO000000000000KKK0K000000000KK"
"                                                       WNNNNXXK00OO0000000000000K0000K000000000000KK"
"                                                       WNNNNXXK00000KKKKKK0000000000000000000000000K"
"                                                       WNNNXXXXKKKKKKKKKK000000000000000000000000000"
"                                                       WNNXXXXXXXXKKKK000000000000000000000000000000"
"                                                       WNXXXNNXXXKK000000000000000000000000000000000"
"                                                       WNXXNXXXKK00000000000KKK000000000000000000000"
"                                                        WNNXXXKK000OOO0000KKKKKK00000000000000000000"
"                                                        WNXXXKKK000OOOO00KKKKKKKK0000000000000000000"
"                                                         WNXXXKKKKK000000KKKKKKK00000000000000000000"
"                                                          WXXXXXXKKKKKK00KKKKKKK00000000000000000000"
"                                                           WNXXXXKKKKKKKKKKKKKKK00000000000000000000"
"                                                             WNKKKK000000KKKKKKKK0000000000000000000"
"                                                              WNKK00OkkkO00KKKKKKK000000000000000000"
"                                                                NXK000OkkO00KKKKKK000000000000000000"
"                                                                 WNXKK0OOO000KKKKK000000000000000000"
"                                                                    WNKK0O000KKKK000000000000OO00000"
"                                                                      WXK0000KKKK00000000OOOOOO00000"
"                                                                       WNXK00KKKK00000OOOOOOOOO00000"
"                                                                        WNKKKKKKK000000OOOOOOOOOOOOO"
"                                                                         NKKKKKKK00000000OOOOOOOOOOO"
"                                                                         N0O0KKKK0000000OOOOOOOOOOkk"
"                                                                         NKOO0KKK000000OOOOOOOOOOkkk";


static char credit1[] = 
"                                                                      .',.                                                                            "
"                                                .....               ;kXNNd.                                                                           "
"                                          .;coxO0KKKKOx:.         .oNMMWx.                                                                            "
"                                      'cdOXWMMMMMMMMMMMWO,       'kWMMNo.                                                                             "
"                                  .,oONWMMMMWXOdlclOWMMMWo      ;0MMMK:                                                                               "
"                                'o0NMMMMWKkl,.    'OWMMM0,     :KMMWO,                                                                                "
"                             .:kNMMMMWKd;.      .lKWMMW0,    .lXMMNx.                                                                                 "
"                           .cOWMMMMXx;.       .c0WMMMNd.    .oNMMXl.                                                                                  "
"                         .:OWMMMW0l.       .,oKWMMMNk,     .dNMMK:                                                                                    "
"                        ,kNMMMW0c.     .cdd0NMMMWXx,      .xWMWO,                                                                                     "
"                      .oXMMMWKc.       'OWMMMWXkc.       'kWMWk'                                                                                      "
"                     ,OWMMMNd.          .:ool:.         'OWMNd.   .:oddc.            .':lodl;.              .,:lodxdl;.           .':lodo;..          "
"                    :KMMMMK:                           ,0MMNo. .ckXWMMMWd.        .:d0NMMMMMWK0x;       .,lkKWMMMMMMMNd.       .;d0NWMMMMWK0k:        "
"                   cXMMMWk'                           ;0MMXl.,dKWMWMMMMK;       'oKWMMN0dokNMMMM0'    'o0NMMWXkocxNMMWx.     'o0WMMN0dlkNMMMMK,       "
"                  :XMMMWx.                    .      ;KMMXo:xNMWKOXMMMK:      'xXMMW0o,.  .oNMMMO.  'dXMMMNkc.  ,OWMWO'    'dXMMWKd,.  .lNMMMO'       "
"                 '0MMMWx.                   .oO;    :KMMXkkXMW0lcOWMMK:     .oXMMW0c.     'OWMMX: .cKMMMXd'     cKX0l.   .oXMMW0c.     .kWMMXc        "
"                 oWMMMO'                  'oKWK,   :KMMMNNMW0l.,OWMMK:    ,o0WMMXl.      ,OWMMXc .dNMMNd'        ...    ,OWMMXo.      'OWMMXl         "
"                '0MMMNc                .;xXMMWd.  ;KMMMMMWKo. ,0MMMK:    ;KMMMWO,      .cKMMWK: .dWMMK:                ;0MMWO;       :KMMMK:          "
"                ;KMMM0'             .,o0WMMMKl.  ;KMMMMMXd.  '0MMMK:    ,0MMMWx.      'xNMMNx'  cNMMX:                '0MMWk.      'xNMMWk,           "
"                ,KMMM0,          .;o0NMMMWKo.   ;KMMMMNk,   .kWMMXc    .kWMMWk.     .oXMMW0c.  .kMMMk.              .;OWMMO.     .lKWMWKc.            "
"                .dWMMWk,.   ..;lxKWMMMMNOc.    ,0MMMWO:    .dWMMNo   .c0WMMMNc   .,dKWMWKo.    .OMMMO.           .:d0WMMMNl   .,oKWMMXo.              "
"                 .dNMMMN0OkO0XWMMMMWXOl,      'OWMWKl.     '0MMMXl,:dKWMMMMMWk:cd0NMMWKo.       oNMMWOc,'',;:cox0XWMNXNMMWk:coONMMWKo'                "
"                   ,xXWMMMMMMMWNKko:.        .kWMNd'       'OMMMMWNWMWNO0WMMMMMMMMWKx:.         .lKWMMMWNNWWMMMWN0xc'.dNMMMMMMMWXx:.                  "
"                     .;loooolc;'.            ,O0k;          ,d0XXXK0xc' .:xkO00Oxl;.              .:dOKXXXK0kdl:'.    .;dO00Oxl;.                     "
"                                              ..              .....          ..                       ......              ..                          ";

static char credit2[] = 
"                                            .,:lxl.                                                                                                                                                     "
"                                         .:xKWWMMX;                                                                                                                                                     "
"                                      .;dKWNKXMMNo.                                                                                                                                                     "
"                                    .cONWXd:oXMWx.                                                                                                                                                      "
"                                  .l0WW0l..:XMWk.                       ,c'                                                                                                                             "
"                                .oKWWOc.  :KMWO'                      .oXXc                                                                                                                             "
"                              .lKWWO:.   :KMM0,                      'kW0:                                                                     ..''..                                                   "
"                            .c0WW0c.    ;KMMK;    'ldl. .:oddc.     :KNk' .,::'         .','. ...       ,lxc. ':oxd;      ..','. ...      .;ldOKXNNXO,       .',,.....      'ldl. .:odd:.               "
"                          .:OWMXo.     ;KMMK:    cKMNklxXWMMMO'   .lXNd:ckKWMMX:    .;oOXNNX0O0Xk.    .oXMXxlkXWMMWk.  .cx0XNXK0OKXl   .ckKNNKOkKWMWO'   .,oOKNNXKO0XO'   .cXMNxlkXWMMMO.               "
"                   .:odxxk0NMMMXkxxxddxKMMXc   .oNMWXKX0xKMM0;   .xNW0OXXklkWMX:  .l0WWX    KMM0;    .xNMWXXXOONMWO'.,xXWW0    NMWx. .lKWNkl,    ll;.  .l0WWX    KMMK:   .oNMWXKXOxKMW0;                "
"                  .o000NMMMWXKKXXXXXNWMMMXl   .dNMMM    kNWk'   ,OWMWWK   .kWNo..oKWW0     KMMO,   'ckWMMMNk;,kWNd',kNMNx     NMNd. .oWMKc.          .lKWW0l    0WM0,   .xWMMM    kWNx.                 "
"                   ...cKMMXl.......'oXMMNl.  'kWMMN    0WXo.   :KMMWK    'OWNd.;0WMK      NMM0,   ,0WWMMNk;   :o;.lXMWO      WMWd.  .oNMNKOxoc;.    ,OWMKl     NMM0,   'OWMMN    0WXl.                  "
"                    .oXMWO,        'OWMNo.  ;0WMW0    KMK:   .lXMWK    .lKWKc.;KMWk     XWMMK;   :KMMMNk;       .oNMNo     NMMWk.   .,;ldk0XNWWXk, ,0MWO     KMMMX:   ;0MMWO    KMK:    ..              "
"                   'kWMNo.        .kWMNd.  :KMWKl    0MK:   ,xNMNd    l0WNx' .kMWk    XWWWMWo  .lXMWN0:.        ;XMNl    NWWWMK;  .oKk.     ,xNMWd'xWWO     WWWMWd. .cXMWKc.   0MK;   ,dKl              "
"                  ,0MMK:         .kWMNd. .lXMNx'    lNMO;':xXMMW0c;lkXWNk;.  '0MWkkkkkWXxl0MNd,lOWMXo,.          cNMXddKWWKodNMKc;dKWMNd;,;cokKWNx''OMWkoONMNxl0MWd;cONMNd.    oWMO;'ckXXd'              "
"                  oWWO,         .xWMXl.  'kXO:.     ,OWWNNWMMNO0WWWWN0o,     .lXMMWNOl' .oNMWNNXXk,             'kWMMWXk:. 'OWMWNXkkXMMWWWWNKkl,   lXWMMN0o'  lNMWWNXXO:      ;0WWNNXOl'                "
"                  .::.          ;ddl,     ...        .;cdKMMXl..;lc:'.         ':c:'      ,cl:'..                .,cc;.     .;cc;.  ':lllc;'.       .:c:'.     'cl:,...        .;cc:'.                  "
"                                                       .dNM0;                                                                                                                                           "
"                                                      'kWWk'                                                                                                                                            "
"                                                     ;0WNd.                                                                                                                                             "
"                                                    cKMXc.                                                                                                                                              "
"                                                   ,KW0;                                                                                                                                                "
"                                                   .lo.                                                                                                                                                 "
"                                                                                                                                                                                                        "
"                                                                                                                                                                                                        "
"                                                                                                                                                                                                        "
"                                                                                                                                                                                                        "
"                                                                                                                                                                                                        "
"                                                                                                                                                                                                        "
"                                                                                                                                                                             .                          "
"                                             .,:o:.                                                            .,c;.            ':c.                                      .o00:                         "
"                                         .,oONNWMX:                                                           ;0WK:           'xNNd.                                     :KWNo.                         "
"                                       'l0NWXNMMWd.                                                          lXMK:           ,0WNo.                                    .oNMKc                           "
"                                     ;xXWNkldXMWk.                                                         .oNW0;           ;KMXl.                                    .kWW0;                            "
"                                  .:kNWXd,.;KMWO'                       .,.                               .dNWO'           cKMXc                                     ,0WWk'                             "
"                                .:OWWKo'  ;0MM0,                      .cKXl                              .xWWk.           lXWK;                                     :KMNd.                              "
"                              .:OWWKl.   ,0MMK;                      .xNKc.                             .kWWx.          .oNW0,                                    .lXMXl.                               "
"                             ;OWWKo.    ,0MMX:    .:lc. .,cll;      ,ONO,  .',.      ..      .,c;.     'OWNo    dOo.   .dNWO    lOx'      .,cll:..          .';:;,dNMK:    .        .'.                 "
"                           ,kNMXd.     'OMMXc    ;0WWkco0NMMM0'    cKNx;:oOXNW0;   'xXO      ONNd.    ,0WNl    OWWx.  .xWWk    dNM0,   .;xKNNXWWX0l.     'cxKNWWWWWW0;   .dKo.    .lKNd.                "
"                   .;cloookXMMMKxooollo0WMNl    cXMWX0XKkKWMKc   .oNW0kKX0dOWMNc  ;0WK      KMNd.   .;KMXc. :kNMXo.  .kWNd. ,dXMWk,  .l0WNO    KMMO.  .ckNWNOolkNMWO,   .kWX:    'xNWO'                 "
"                  .oKXXNMMMWWNNNNNWWWMMMMNo.  .oNMMM    dNWO;   .xWMWW     xWWx. :KMK     dXMXl.   :kKMXo:dKWWXd,   'OWWk:lONMNO:.  :0WNk;    cXMNc .c0WWK    dNMWO'   'OWXc   .cKWWx.                  "
"                   ..'cKMMNd,''''',,oXMMNd.  .xWMMW    kWNx.   ,0WMMX     xNWx..lXM0    lKWMXl    cXMMMNXWMNOl.    ,OWMWXNMWKd,   .oNWKc     lXMXl.'kWMKl   lKWMWO'   ;0WK:  .:OWMNo.                   "
"                    .cKMW0:        .kWMWx.  'kWMWK    0MXl.   cKMMXd    :0WNo..lNM0   :0WMMNo.  .lXMMMMMMXd'      ,0WMMMMWk;.    .oNWO      kWW0;.;0MWk    OWMMMO'   cKM0; .;kNMMXl.  ..                "
"                   .xNMNx.        .xWMWx.  ;0WMXd    OWXc   .oNMNk    :kNWO;  cXMK;.:ONNNMWx.  ;kNMKkdOWMNo.    .:KMNxxNMWk.     :XM0     oXWXo. '0MWk   kNNKNM0,  .oNM0;.:ONMMMK:  .lOd.               "
"                  'OWMXl.        .xWMWx.  :KMWO,    lNMO,.,oKWMW0:':d0WW0c.  'OMMKx0WNxl0MNo':kNMMK:. .dNMNx;.'cONMNo. cXMW0c..;xXMM0;'cxXWXd'   cNMNxlONNk:oXWx',oKWMW0x0NKKWMK;  oKXk;                "
"                  oWM0;         .dWMNd.  'OWKl.     ;KWWXXNMMWKKWNNWWXk:.    ,0MMMWXd, .kWMNXNNWMK:    .:ONMNXNWWMNo.   'xXWWXXNXOKWWNNWXkc.     ,0WMWN0o,  cNMNXNNXWMMMNOloKW0: dK0o'                  "
"                  'll'          ;kOd;.    .,.        .coxKMMNd..ldol;.        ,odo:.    .:ool;';c'       .,lool;;c;.      'codl:. .:llc;.         .:c:'.    .;odoc'':ooc,.:KMNOOXOc.                    "
"                                                       .lXMKc                                                                                                           'dXMMMNO:.                      "
"                                                      .dNW0,                                                                                                          .oXMMMNO:.                        "
"                                                     'kWWx.                                                                                                          :KWMMWO:.                          "
"                                                    ;0WNo.                                                                                                          cXMMMNd.                            "
"                                                   '0WKc                                                                                                           '0MMMXl.                             "
"                                                   .ox,                                                                                                            .oOOo'                               ";

// static char credit3[] = 
// "                                                               .,'.      .,'.                     .,;.                                                           ...                                                             .:l:.                                         ';,                          "
// "                              ,c:.              .:ldkd.     .ckXNNx.  .:kXNNk.                   :0N0;                                  'cdxl'                .;xXX0:                                              .clol'       .kWMXc                                       'xXNo.         .;c;            "
// "                            .lXWk.           .:xXWMMM0,   .c0X  WWx. :OX  NWk.          .''.   .oNWO,                               .;oONMMW0;     .,;;,.      'kMMNl                                            .lKWMMX:      'kWMNl.                               .'.    ;0WXl.         .xNWd.           "
// "                           .oNWO'          'oKW   WMX:   ,kN    WO  kN    WO,          ;OXO'  .xWNd.                             :kOKNWMMMNx.    .l0WWWK      .dNMWx.                                           ;OWMMMMk.     ,0WMKc                               .oKXl   cKW0;          .kWWx.            "
// "                          .dNWk'         ;xNWK   XMNl  .lXN   KNk lKN   0Wk'     ..'',oXMNx  ;kWXl.                              lKX0KNWW0:     c0WMWO       'kWMNx.                                          .oXMNNMMNc    .cKMW0;                          ..'',:OWW0  ,oXWk'          'OWNo.             "
// "                          lNXl.       .;kNW0    KMNo. .xNK   NXo xNK   XNd.    ;kXNNNWWMWWN NNWK:                                 .;dXWNd      OWMMX         0WMXl.                                          ,OWW0oOMM0'   .dNMWk'                         .o0XNNWWMWW   NWNd.          .xW0:               "
// "                          .;'        ;kNWO     KMWd. 'OWO   N0  kWO   N0:      ;ood0WMKo   dNWO,                                  ;OWW0       NMMW         dNMWO;                                          .cKMN  :KMWd.  ,OWMXo.                          .cookNMNk:;,c0MXl             ';.                "
// "                                   ,kNW0      KMWx. 'OWk  KXd  kWO  KNx.          cXWO    :KWk'.,ldd;      .;codd:.             .dNMX       NMMMW        c0WMXo.     .,;,..'.     .'      .;,             .xNMX   dWMK; .cKMW0:     .:lddc,.     ,od:  :oxd,  'kWNo   .kWK    odl.                          "
// "                              ...,dNMN       KMWk. .kW0 kNO   xW0 kN0:          .oNWk    cXNx:lONMMWd.  ,oOXNXWMNd.            ;0WW0      NMMMMW        kWMNk,   .;dOXXKKKKNO'   cX0,    cKWk            ,OWW     0MWx..xNMNk'   .:xXNKOKWWK:  .lXWXkkKWMMNl.,0WXc    kW0c:xXWMM0,                          "
// "                          .ck00KKNMMMWXK0000XMWO' .dWWKXKo. .oWWKXXo.          .dNWx    cXW0k   XMWx. .dNW0    cc.           .oXMN      NMWNWMN       KNMWO:   .l0WN     WW0;  .lNN     dNWk            :KMW      NMNo:0WMXl.  .c0NX    cXMX: .dNMWNNNkNMNo.:KWK     OWXkOXKKWMK;                           "
// "                          .codKWMW0doooddxKWMM0,  cXMMNx,   :XMMNk,           .xWNd    cXMWN   kNWx.  cNMKxo                .dNMX     NMWOkXMW      XMMW0c.   :0WN      WM0,  .dNX    lKWNd           .oXMN       MMN0XMWO,   'kWN      OWNl.'kWMMN   lX0:.cXW0     OWWN    KMK;                            "
// "                            .oXMXo.      ,0MM0,  .kMWO:    .xMW0:.           .xWNo    cXMMN   xWWx.  .cXMWXOo'             .xWMK     MWOcc0MW      MW0dc.   .oNMK     XMM0,  .kWX    0WMXl           .dNMX       KMMMMMXo.   'OWX      KWKc.,0WMNk    ... :XM0     OWMW    KMK;                             "
// "                           ,OWW0;       'OWMK;   ;KMO'    'xNM0,    'od'    .oNWd   .dXMW0   oNWk. .cOXXk     .;xo.        lNMNk   MNO:..kWMX   NMNk:.      cNM0     WMMNc  ,OWX    WMMX    xo.     .kWM0       lNMMMWO;    .dWX      NNx'.:KMWO:.       ,0MK     0WMX    KMX:  ,do.                        "
// "                          :KMNx.       .kWMK;    :XM0:..:xXWWMKc..:xXXd.    .OMNo':xXWMKl   ;XMXl,l0WMWk     o0N0c        .OMMWWWWKx;.  :XMMWWWWKd,        .dWWkokNW0kXMK  dXMMK   XNMK    KO:.    'OWWO       .kMMMXo.     .OMK    NNk; .lXWXl.         lNMO;,l0WMNk,   xWWk;:kXXl.                        "
// "                         .xWXl.       .kWWO,     .dNWNXXXOo:dNWWXXXOo,      .oXWNNXNWNx'    ,0WWNXXkx0NNX0KKX0d:.         .c0NNKxc.     ;0WWNKx:.           ;0WWNXk:..oNWNXKXWMWKdo0WKooOKd,.     .kWW0,       .dKKk,        lXWNNXOo'   'kKx,           ,0WWNXNNW0:.   .dNWNNX0o'                          "
// "                          .:'         ,oo:.        ':c:;.    ':c:;..          .:c:'';'       .;c:;.  .,:cc:;..              .''.         .,;'.               .,:,.     '::,..;:,.;OWWXK0o.        .clc.          ..           .,;,.       ..              .,::,',,.      .,:c:'.                            "
// "                                                                                                                                                                               'dXMMW0l.                                                                                                                    "
// "                                                                                                                                                                             .lKMMWKl.                                                                                                                      "
// "                                                                                                                                                                            .xWMMNx.                                                                                                                        "
// "                                                                                                                                                                            oWMMXl.                                                                                                                         "
// "                                                                                                                                                                            ;kOd,                                                                                                                           "
// "                                                                                                                                                                                                                                                                                                            "
// "                                                                                                                                                                                                                                                                                                            "
// "                                                                                                                                                                                                                                                                                                            "
// "                                                                                                                                                                                                                                                                                                            "
// "                                                                                                                                                                                                                                                                                                            "
// "                                                                                                                                                                                                                                                                                                            "
// "                .;oo'                                             ...,;;:;'                                                     ....        .;dk:                                                                                  ...,;;:;'                                                                "
// "               'xNNo.                                       .':ldk0XNNWWWWK;                                              .':ok0KXXKOc.    .dNWk'                                                                            .':ldk0XNNWWWWK;                                                               "
// "              :0WKc                                     .;lkKNWX0kdolxNMMKc.                                           .:d0NWNKk  0WMX:   'OWNo.                                                                         .;lkKNWX0kdolxNMMKc.                                                               "
// "            .lXWO,                                   .cxKWNKxl;..   :0WNx'                                           ,dKWWXkl     KMWx.  ;0WKc                                                                        .cxKWNKxl;..   :0WNx'                  ':;.                                           "
// "           .dNWx.                                 .:kXWNOl,.      .dNWKc.                                          ;kNMNOc.     kNMXo.  :KWO,                                                                      .:kXWNOl,.      .dNWKc.                 .lXWO'                                           "
// "          'OWNo.                                .oKWW0l'         ;OWNx'                                          ,xNMNx;.    o0WWKd'   cXWk.                                                                     .oKWW0l'         ;OWNx'                   .lxl.                                            "
// "         ;0MNd,...                            .lKWNk:.         .oXMKc.                                         .lXMWk;     l0X0x:.   .lNNd. ....         ....                ..                                .lKWNk:.         .oXMKc.           ..                ....',.      .....',.                   "
// "        :KMMNXXXNKo.   'dl.    .okl.         'kWW0:.          ,OWWO,     .:lxxxolol.   .oo.    .lko.          .kWMXl.       ...     .oNXo  d0XNx.    .;cx0XXKko,   'dk;    ,kKk'                              'kWW0:.          ,OWWk,   'dk;    ,kKk'  ,dKNk,  kOO000KKXNXo  kOO000KKXNXl   :d;     ;xx;    "
// "       cXWX0   xNMK;  ;0Wx    :0WK:         .kWMO'          .cKMXo.   .:xXW    NMNx.  'kWO'   ,OWXl.         .kWM0;                .dNXxoOXNWMNl   ,xXWN    KMMx. ;0WO'   cXW0            ....'',.           .kWMO'          .cKMXo.   ;0WO    cKW0    KMMXc'  xddx0WMW0o   xxddx0WMW0o,. .lXX     oXWk'    "
// "      cXWO     lNWk. :KWx     NW0;          ;XMWk'  .,:.   .xNW0;   .c0WN      WNd.  ,OWO     KWKc          .dWM0,           .ll. .dNWK    KWNl  .dNNO      KMX:.:KWk    xNWO           .l0XXNNNWK:          ;XMWk'  .,:.   .xNW0;    :KWO    xNWO    oNMK:     .;xXXk:.      .;xXXk:.   .oNX     OWNx.     "
// "     :XWO     lXWk'.lXNx    KWWk'           .lKWWX0OKKd.  :0WWk:'. 'kWW0     NMWd.  ;KWO     WM0;           ;KMNc          'o0Nd..dNMW    OWNl. ,0WK       KWKc.lXWk    KMWO            'dkxdolc:;.          .lKWWX0OKKd.  :0WWk:'   lXWk    KMWO    oNW0;     l0NXd,       .cONXd,     .xNX     NMNo.      "
// "    ,0W0     kNXo..oNNd    WMWx. .,'          .;lool:'  .oXMMNK0l.'OWW      WMWk.  cXWk     MWO'  ''        cNM0'       .ckXWXo..dNMW    OWNl. '0WK      xNWk,.lNWk    WMM0   .,.                              .;lool:'  .oXMMNK0l. lXWk    WMM0    xNWO'    oKWXd'       'l0WXd'     .;OWK    XMMX   .,.   "
// "   .dWX     NNx, .dWWO   NWMNd   kKo.                .,lOWMMNKx;  lWWk     NNMX; .dNM0    WMWx   xKx.       ;KMXl....;lkXWW0o' .oNMXo   xWWd..:OWNl    dKW0c. :XMK    KXWN   lKk.                                     .,lOWMMNKx;  :XMK    KXWNl   KWWk.   0WMW0c''      XNWW0c'';:lox0NMN    WWMK   l00;   "
// "   .xWNkxKNKx,   ,KMWNNKOKWNo  OKx;.             .,lxKWMMNkc,.    lNWK0NW0l:OMNkxXWMMN   0WNx   Kk:.         ;ONWXKKXWWXOo;.   lNNk'   ,KMWOd0NNWW0x0NWNOc.  .dWMNNN0c;kMNOkKXx,                                  .,lxKWMMNkc,.   .dWMNNN0l kWNOxKNWMW0dxKWMMMWXKXNNNNXKXMMMWXKXNNNNXKXMMW  0OXW0   K0o'    "
// "    .oOOko;.     .o00xc;dNW0x00o'             .;d0NNNWMWK:        .ckOko;.  ,xO0kodO0kl;oXW0x00d,             .,loool:,.      .ok:.    .ck00Od:,cxOkdc;'      ;k00x:.  ,x00kl'                                 .;d0NNNWMWK:        ,k00x:.  ,x00kl,;d0K0kookOkxdoc:;,...;xOkxdol:;,...;x0Od::OWXkkKk:.      "
// "                     .,kNMWNOc.             'l0N    XWWk'                             'dXMWN0o.                                           ..                    .                                            'l0N   xXWWk'           .                ..                                . .c0WMWXx;         "
// "                    'xXMMWOc.             'dXW    dXWKl.                            .oKWMWKl.                                                                                                              'dXW    dXWKl.                                                                :OWMMXx,           "
// "                  .cKMMWKc.              ,0W0   lKWNx'                             ;0WMMXo.                                                                                                               ,0W0    KWNx'                                                                .xNMMWk,             "
// "                  lNMMWO,               .dWNx  XWNk;                              :KMMM0;                                                                                                                .dWNx  XWNk;                                                                 .kWMMNo.              "
// "                 .kWWXo.                 :KWWNXOo,                               .oWMNx'                                                                                                                  :KWWNXOo,                                                                   ;KMW0:                "
// "                  .::.                    .;;,.                                   .;:'                                                                                                                     .;;,.                                                                       ,:;.                 ";

static char credit3[] = 
"                                                                 ...'..                          .:dkkc.    'lxkx;                              .:ol.                         ..',;::;;'.                  .cxkx:.                                                                                          "
"                                                            .,cdk0XXNNKk:                      .oKXXWMK;  ,xXXXWMk.                            :0WKc                     .;cdk0XNWWMMMWWXO,              'dKX WM0'                                                                                          "
"                                                         .:d0NMWXOxokNMM0'                   .lKNk  WNo..dXX  0WX:                           .oXWO,                  .;oOXWMWX0kdokNMWWNXk'            .oXN   WNl                                                                                           "
"                                                      .,dKWMNOo;.  .dNMNo.                  ,kNK   NNd'cKW    WKc                           .xWNx.                .:xKWMWKkl;..   .;:;,'.      .,;.   ;0W0   NXl.                                                                                           "
"                                                    .:ONMW0o,.   .l0WWKc.                 .lXN    WXl;xNX   0W0;                           ;0WXl.                ;OWMWKx:.                    ;ONNl .oXN    WKc                                                                                             "
"                                                   ;OWMNk:.  .,:dKWWKo.                  .xNX    W0::OW0   XNx'                           cKWK:                 cXMMKl.                      .lKOl.'kWK   KWO,                                                                                              "
"                                                 'xNMWO;.   .oXWWXkc.                   .kW    NNd':KWk   WKl.                          .oNWO'                 .xMMWO;.                        .  'OW0   NXo.                                                                                               "
"                                                ;0WMKl.      .;:;.        .';cc;.';'   .kW   OW0:.;KWk  KNk'            .;oxkkdc,.     .xWNd.                   :KWMMNKkdl:,..           .;dOx'  'OWO  0NO;   .,coxkOx,    ;xO    dkOd'     .;lxkOOx:.     .;ldkOkl.                                        "
"                                               cXMWO,                  .:x0NNXKKXNNx. .xWX dXXd. ,0W0okNKc.           ,dKNX  KWMNl    ,OWXl.                     .cx0XWMMMMWX0xl,.      ;OWMWO' .kWKoxXXo. .;xKNN  MWO,  .dNMNO  NWMMK;  .:kXWX  0WM0'  .lOXWNXWMNo.                                        "
"                                              ;KMWk.            .,.  .l0WW     KMNx. .oNWXKNO;  .kWWKXXd.           ,xNW0     NMNc   :KWK:  ..                      ..,:lox0XNMMNO:.    oWMWk. .dWWXXNk,  'kWW0    c;.  .xWMWNN0xONW0; .:0WNO    0W0;  cKWX     c'                                          "
"                                             .OMM0'           .:O0,.cKWW      NMWd.  ;KMMWKc.   lNMMWO;           .lXWK      0WNx;:lxXMW0xxO00k:.                           .,lKMMNc   ;0WWx.  :XMMW0:.  .dWMKd        ,OWMM    xNWx' .dNWO:.   .;;.  ,KMNOo                                                "
"                                             :XMNl          ,o0WNo,xNMX     0WMWx.  .xWMXd.    '0MWKc.           .dNWk      KWXxxXWMMMNXNMMNk:,.                             .:0MMNl  cKMNo.  .kMMXo.    .lNMWNKx     :0MMW    OWXo. .dWWx.           'OWMWXOo.                                             "
"                                             :XMX:      .'lONWNk;.oWMK     WMMMO'  .dNMXc     'kWM0,    .;d;     cNWk     kNWO,.cKWMNd,.:0MWx.             .;;.          .':d0WMMXo. cXMXc.  ,kNMK;     ;xKXO      .ckXMMK    OWXc   cXM0'         .cx0XKx      ,ol.                                        "
"                                             .kWW0c;,:lx0NMWKd,. '0MN    WN0KWWd.'oKWMMK:  .;xXMMMO'  .:kNK:    .xMNo   kNW0c.  ,OWXl    OMWd.             lNNOdlc::ccodkKNWMMN0o'  :XMNo..;kNWMM0,  .:kNMNo.   .;dKWMMNd.   oWWd..,dXMMK:.   ..;lxKWMM0;   .'ckNNo.                                        "
"                                              .l0NWWWWWNKko;.    .xWWNWWXx;.oNMXKXXko0WMXOOKNXxxKMWKOOKN0o'      lNMXKNWKx:.   ;0WK      NWk.              ;OWMMMMMMMMMMMWX0xc,.    :KMWKOKNXxo0WWXOOKNXXWWKkdxOKNXNWW0:     lNWXO0X0xxXWN0OO0KXXKxlxXMNOxdk0XX0o,                                          "
"                                                .,:c::,..         .:ddo:.   .:dxdc'  .cxkxdl;.  'oxkxdc,.        .;odol;.     cXW0      WNx.                .;lodddddolc:,.          ,oxkxo:.  .lxkxdl;..:oxkkxdl;..;;.      .;oxxo;.  'cdxkxdl:'.   'ldxkkxo:'.                                            "
"                                                                                                                            .oNWk     KWKc.                                                                                                                                                                 "
"                                                                                                                           .oNWk     NXd.                                                                                                                                                                   "
"                                                                                                                           lNWx   ONXd'                                                                                                                                                                     "
"                                                                                                                          ,KMXxd0NKd'                                                                                                                                                                       "
"                                                                                                                          cNMMWXkc.                                                                                                                                                                         "
"                                                                                                                          .;ll:.                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                                                                                                                                                                                                                                                                                                            "
"                  .:ol.                                          .;:'             .                                                                               ...                                      ...''...                                                                                         "
"                 ;0WXl.                               ..     .'ckKWWx.         .lO0Oc                                                                           ,x0O,                               ..;cdxOKXXNNNXKOo.                                                                                      "
"               .lXW0:                               .c00dccld0NMMMWx.         'kWMWx.                                                                          cXW0;                            .'cx0XWMWXKOk0WMMMMWO'                                                                                      "
"              .xNWk'                                .dNMMMMWXXWMMXo.         :0WMXl.   ...                                                                   .lXW0,           ...             ,o0NMMN0xl;..  ,lllc:,.                                                                                       "
"             ,OWNd.                                  .,:cc::l0WW0:         .lXMW0;   .oKXo.                                                                 .oNWO'          .dKXo.          ,kNMMNOl,.                                                                                                      "
"            cKMXl.                                         :KWWk'         .xNMWk'    lXXx,                                                                 .dNWk.           lXXx'          cXMMXd'                                                                                                          "
"          .lXMK:                                         .oNMNo.         ,OWMNd.     .'.                                                                  .xWNd   ..        .'.           .OMMWx.                                                     .;;.                                                  "
"         .dNMWOoodo:.     ..      .,;.            ......,kWMNx,.......''lKWMKc   .cdo'    .:do' .:odo,    .,codl;..     ..      .,,.    ',.     .co;     'kWNo    OO:   .cdo'             .oNMMWKxoc;'.                 ..','....     .:l:     .,.   .xWWd.     ..,,.....     .:oo'  :odo,      .;lool,.    "
"        .xWMMMNXXWMWx.  .lX0,    'kNXc           .oXXXKKNMMMWNXXXXXXXNNNWMW0;  .dXWMK;   ,OWW0dkXWMMWo  ,d0NNKKWWNk.  .oXO'    'kNXc  .dNX:    cKWK:    ,OWXl    XMK; 'dXWMK;               ;d0NWMMMWNKOxl,.         .cx0XNXK0KXO' .;xXWW    .dXXl    0MNl   'cxKXNXK0KXx.   ,OWW0   WMMWd.  .:xKNX0XWWXo.  "
"       .kWWOo  .,OMMk. .oNNo   .cKWXl.            .lx0NMMNOxxxxxxxxxx0WMMWk'   cXMMK:   :KMMWN  0NMNd';kNW0    OMM0' .dNNl     KMXl. 'kWN     oNM0;    ;0WKc   XWNk,  lNMM0;                  ..;coxO0XWMMWKd'     ,xXWN    0WM0: .oNMMW     KWNo    kWNd..;xXWN   lKWW0,   :0WMWNXOkXMWx. .c0WN     KMWx.  "
"      .kWWd     :KMK; .xWNo    xNMK:                'kWMXl.         ,OWMNd.   .xNW0;  .lXMMMNk   WXl;dNW0c    cXMXc .xWXl     NMK:  ,OWX     OWM0,   :xKMNOd0WWXx;   .xWWO,                          ..,cxXMM0,  'xNWX     0WM0,   :KMW     WMNo    0WXo.,kNWX     KMWO'   cXMMM   c0WXl. ,kWNk     dNM0,   "
"     .xWWd     lXW0; 'kWXl    XMW0;                ;0WMK:          :0MMXl.   'kWWO'  .dNMMNk;    :',kWNd     oNWK:.,OWX     XMWO,  ;0WX     NMM0,   :KMMMWWWXk      ,OWWk'                               :KMMX:.:KMWk     NMMK;   .xWWO    MMWx   oXW0:.lXMN      NMMO'  .oNMMN   cXW0;  ;KMXl     kWWO,    "
"     lNWx     OWNx. ;0WXc   XMMWO'  'c'          .lXMWO,         .cXMMK:    ,OWWx.  'kWMWO;.      .xWNo     OWNx'.;0WX     MMWk'  :KMXc   XWMMK;  .oXMWNNMMNo.     ;0MNx.  .:'         ''            .,lkNMMNo.:KMNd    KWMMXc   ;OWWOl   NWMO  c0WXd. cXMN     XMMMK;  .xNMWO   cXWO'  '0MX      KWXo.     "
"    '0MK;   ONNk;  ,0MNklxXNWMWk..,xKK:         .xNMWk'         .oNMWO,    'OMWx. .lKWWKc.        ;XM0    ONNO:  ;KMNxlkXNWMWx..;xNMWkcxXN0KWWo..c0WMKx:;OWMK:.  'dXMWd. .lKXc        lXKdc;''',;:ldkKNMMWXx, .xWWk    WXXWMk..;kNMMN0KN0oxWWklONNk,  .kMWx   NWXXWWd..:OWMKl.  ,0M0, .;OWWx    0WNx,       "
"    '0MNOkXNKx;.   oWMWNNKk0WNx;:kKOl.         .xWMNd.         .dNMWk'     ;KMW0xkXWMNd.          ,KMNOOXWXx;.   oWMWNNKk0WNd,:kXNWMMWWNO:'xMW0kKWMMK:.  .dXMNOxOXWWMW0xkXN0c.        lXMMMWWNNWWMMMMWXOd:.   .dWWXKWWKo,lNMXk0NWWMMMW0c..xWMWWXd,    .xWWXXWW0o,oNMKk0WMNx'    ;KMXxd0XNWMXk0NWKd,         "
"     ,dOOko:.      ,x00xc;dNW0x0Kk;.           .:oo:.          'oxdc.       ;x0K0koll;             ,dkOko:.      ,k0Oxc;dNW0x0Kx;,o0KOo,.  ,d00Odoxx;      ,oO00kl;:x0K0ko;.           ,lxkOOOOOkxdl:,.        .lkOkd:.  .cO0Oxc:d0Od;.   'oxd:.       .okOko;.  .lO0Oxol;.      ;xO0Od;':xOOxl;.           "
"                     .  'xNMWNKd,                                             ...                                  . .,kNMWWKd'    ..         .               .      ...                                                    .                                       .                                       "
"                      'dXMMMXd'                                                                                     'xXMMWXd'                                                                                                                                                                               "
"                    .cKMMMNd'                                                                                     .lXMMMXd'                                                                                                                                                                                 "
"                   .oNMMMK:                                                                                      .dNMMMK:                                                                                                                                                                                   "
"                   ;KMMWO,                                                                                       ;XMMWk,                                                                                                                                                                                    "
"                   .lxd:.                                                                                        .oxd:.                                                                                                                                                                                     ";

static char credit4[] =
""
"                                                            .......                   ':c:;'...                                                                                                         "
"                                                         ,oOK0KXNX0o.              .lONMMMWNXK0:                                                                                                        "
"                                                      .:kNMMMMMMMMNl.            ,dXWMMMMMMMMNx.                                                                                                        "
"                                                    .:OWMMMMMMMMMXl.          .:kNMMMMMMMMMW0:                                                                                                          "
"                                                  .;OWMMMMMMMMMMXc          .cOWMMMMMMMMMMNx.                                                                                                           "
"                                                 ,kNMMMMMMMMMMMK:         .c0WMMMMMMMMMMWKc                                                                                                             "
"                                               .dXMMMMMMMMMMMM0;        .l0WMMMMMMMMMMMNx'                                                                                       ':ll;                  "
"                                             .lKMMMMMWNWMMMMWO'       .l0WMMMMWNWMMMMMKc.                                                                                      ,xXMMMK,                 "
"                                            ;OWMMMMW0OXMMMMWk'      .l0WMMMMWK0NMMMMWk'                                                                                       ;KMMMMWd.                 "
"                                          'xNMMMMWKooKMMMMWx.     .l0WMMMMW0dkNMMMMXl.                                                                                        oWMMW0c.                  "
"                                        .cKWMMMMXd,cXMMMMNd.    .c0WMMMMW0lcOWMMMWO,                                                                                          .col;.                    "
"                                       ,OWMMMMWO;.cXMMMMNl.   .c0WMMMMWKl,lXMMMMNd.                                                                                                                     "
"                                     .oXMMMMMKl..lNMMMMXc   .:OWMMMMWKl.'xNMMMW0:                ..'',;,'.           .,;:,      .';:;'.             ..,;::;'.            .;c:.                          "
"                                    ;0WMMMMNx' .oNMMMMK:   ;kNMMMMWKo..;0WMMMNx.            .,cdk0XNNWWWNKl.       ,xKWWMO.  'lxKNWMWNk'       .,cdOKNWWMMWN0o.       .;xKWMWK;                         "
"                                  .dNMMMMW0c. .dNMMMMK;  ,xNMMMMMXd' .oXMMMMXl.          'cxKNMMMMMMMMMMMWd.      cXMMMMK:.ckNMMMMMMMMX:    .;d0NMMMMWNNWMMMMNl     .c0WMMMMMK;                         "
"                                 ;0WMMMMNx.  .dNMMMM0, 'dXMMMMMXd'  'kWMMMWO;         .:kXMMMMMWKkKMMMMMXd.     .oNMMMW0odKWMWXXWMMMMNo.  'o0WMMMMN0d:';kWMMMX:    ,OWMMMMMMK;                          "
"                               .dNMMMMW0:   .dWMMMWO,.oKMMMMMNx,   :0WMMMNd.         ,kWMMMMNOo,. ;kOkdc'      .xNMMMWX0XWMWOodKWMMMNo. 'dXMMMMW0o,.  .dNMMMXc     'OWMMMMW0,                           "
"                              ;OWMMMMNx.   .xWMMMWO:l0WMMMMNk,   .lXMMMMXc.         :KMMMWKd,.                'OWMMMMWWMMNO:.;KMMMMK: .lKMMMMNk:.     :XMMNk,      .xWMMMWO'                            "
"                            .oXMMMMMKc    .xWMMMW0dOWMMMMNk;    .xWMMMW0,    ..    .kMMMWO:',:lo;.           ;0MMMMMMMMNk;.  oWMMWO, .xWMMMNk;        .;c:'       .kWMMMWk.                             "
"                           ,OWMMMMWk'    .xWMMMMNKNMMMMNO;.    ,0WMMMWx.   .:Od.   .xMMMMNKXWMMMNc          cKMMMMMMMNk;.    ,kKOl. 'OWMMMKc.                    ,OWMMMNd.                              "
"                         .lXMMMMMXl.    .xWMMMMMMMMMMWO:.     cKMMMMNo.   ;kNM0,    ,OWMMMMMMNKkc.        .oNMMMMMMNk;.        .   .kWMMMO,                     ;KMMMMNo.                               "
"                        'xNMMMMWO,     .xWMMMMMMMMMWO:.     .oNMMMMK:  .:kNMMMK;  .:d0WMMWKxc'.         .;xNMMMMMWO:.              lNMMMK;                    .cXMMMMXc       ..                        "
"                       :0WMMMMNd.     .xWMMMMMMMMWO:.      .dNMMMM0;..lOWMMMW0:..dKWMMWKd:.           'o0WMMMMMW0c.               .kMMMMx.                  .cONMMMM0;      .l0K;                       "
"                     .oXMMMMMXc.     .dWMMMMMMMWOc.       .xWMMMMNxlkXWMMMWKo. 'OWMMWKl.          .:okNMMMMMMMXo.                 '0MMMMx.              .'ckXWMMMMM0,     .l0WMX:                       "
"                    'kWMMMMW0;      .oNMMMMMMW0c.        .dWMMMMMWWMMMMMW0l.   oWMMMWx.        .:xKWMMMMMMMMWk,                   .kMMMMXo.       ..';ld0NMMMMMMMMWo   .,dKWMMNo.  .:ool'               "
"                   :KMMMMMWx.       ,KMMMMMWO:.          ,KMMMMMMMMMMMXx:.     cNMMMMNOoccccldOXWMMMMMMMMMMKl.                     ,0WMMMWXOxdxxkO0XNWMMMWXkxXMMMMWKxxkKWMMMWO:  .dXWMMMk.              "
"                 .oXMMMMMNo.        .:OXNXk:.            .xWMMMMMMMNOl'        .c0WMMMMMMMMMMMMMWXkxxONWMNx'                        .oKWMMMMMMMMMMMMWNKko;.  :0WMMMMMMMMMWKx;.  .xWMMMMWo.              "
"                .xWMMMMMXl             ...                .ck0KKOxc'             .:ok0KXXXXK0Odl;.   .';:,.                           .;ok0KXXKKOkdl;'.       .cx0XNXX0xl;.     .OMMMW0c.               "
"               'OWMMMMMK:                                    ...                     ........                                             ......                 ......          .clc;.                 "
"              .xWMMMMMK;                                                                                                                                                                                "
"               :ONWMW0;                                                                                                                                                                                 "
"                ..;::.                                                                                                                                                                                  "
"                                                                                                                                                                                                        ";

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Choco - API8X", 20, 20, 
			 _wW, _wH, GL4DW_FULLSCREEN | GL4DW_FULLSCREEN_DESKTOP))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données. */
static void init(void) {
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    exit(0);

  if(Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048 ) <0)
    exit(0);

  if((gMusic = Mix_LoadMUS("assets/music.mid")) == NULL)
    exit(0);

  

  _pId = gl4duCreateProgram(
    "<imvs>basic.vs</imvs>\n\
    #version 330\n\
    layout (location=0) in vec3 vsiPosition;layout (location=1) in vec3 vsiNormal;layout (location=2) in vec3 vsiColor;out vec4 vsoColor;out vec3 vsoNormal;uniform mat4 projMat,viewMat,modMat;out vec4 vsoPos;out vec3 vsoNorm;void main(void){vsoPos=vec4(vsiPosition,1.0);vsoNorm=vsiNormal;vsoColor=vec4(vsiColor,1.0);gl_Position=projMat*viewMat*modMat*vec4(vsiPosition,1.0);}",

    "<imgs>basic.gs</imgs>\n\
    #version 330\n\
    in vec3 vsoNormal[];in vec4 vsoColor[];in vec4 vsoPos[];in vec3 vsoNorm[];out vec4 gsoModPos;out vec3 gsoModNorm;out float noshade;out vec4 gsoColor;uniform mat4 projMat,viewMat,modMat;uniform int isRibbon;layout(triangles) in;layout(triangle_strip,max_vertices=24) out;void emit_one(vec4 v,vec3 mn,vec4 c){vec4 mv=modMat*v;gl_Position=projMat*viewMat*mv;gsoModPos=mv;gsoModNorm=mn;gsoColor=c;noshade=0.0;EmitVertex();}void main(void){if(isRibbon==1){for(int i=0;i<gl_in.length();i++){gl_Position=gl_in[i].gl_Position;gsoColor=vec4(1.0,0.5,0.0,1.0);noshade=1.0;EmitVertex();}EndPrimitive();return;}if(gl_in.length()==3){vec3 A=(vsoPos[1]-vsoPos[0]).xyz;vec3 B=(vsoPos[2]-vsoPos[1]).xyz;vec3 N=cross(A, B);N=normalize(N);vec3 mN=(transpose(inverse(modMat))*vec4(N,0.0)).xyz;emit_one(vsoPos[0],mN,vsoColor[0]);emit_one(vsoPos[1],mN,vsoColor[1]);emit_one(vsoPos[2],mN,vsoColor[2]);EndPrimitive();}}",
  
    "<imfs>basic.fs</imfs>\n\
    #version 330\n\
    in vec4 gsoColor;in vec4 gsoModPos;in vec3 gsoModNorm;in float noshade;out vec4 fC;uniform vec3 sC;uniform vec3 camPos;uniform mat4 modMat;uniform float expl;uniform float isExpl;void main(void){if(noshade>0.0){fC=vec4(sC,1.0f);return;}vec3 normal=normalize(gsoModNorm);vec3 view=normalize(camPos-gsoModPos.xyz);float fr=pow(1.0-max(dot(normal,view),0.0),2.5);vec3 a=mix(sC,vec3(1.0),fr*0.8);vec3 lN=normalize((inverse(modMat)*vec4(normal,0.0)).xyz);vec3 eR=normalize(vec3(0.35,0.25,0.9));vec3 eL=normalize(vec3(-0.35,0.25,0.9));float dR=distance(lN,eR);float dL=distance(lN,eL);float e=smoothstep(0.12,0.22,dR)*smoothstep(0.12,0.22,dL);if(isExpl==0.0){fC=vec4(a*(0.4+e*0.6),1.0);}else{fC=vec4(a,expl);}}",
    NULL);

  _pIdStars = gl4duCreateProgram(
    "<imvs>stars.vs</imvs>\n\
    #version 330\n\
    layout (location=0) in vec4 vsiPosition;uniform mat4 projMat, viewMat;uniform float t;uniform float isBack;uniform float isCredit;out float vW;out float f;out float back;out float credit;void main(){if(isBack>=1.0){vW=vsiPosition.w;gl_Position=projMat*viewMat*vec4(vsiPosition.xyz,1.0);float p=sin(t*2.0+vsiPosition.x)*0.5+0.5;gl_PointSize=(2.0+vW*2.5)*(0.8+p*0.4);f=sin(t*2.0+vsiPosition[0]+vsiPosition[1]);back=1.0;credit=0.0;}else if(isCredit>=1.0){vW=vsiPosition.w;vec3 pos=vsiPosition.xyz/100.0;pos.x-=35.0;pos.y-=20.0;gl_Position=projMat*viewMat*vec4(pos,1.0);gl_PointSize=1000.0/gl_Position.w;gl_PointSize=clamp(gl_PointSize,1.0,60.0);f=sin(t*1.5+pos.x*0.3);back=0.0;credit=1.0;}else{vW=vsiPosition.w;vec3 pos=vsiPosition.xyz/100.0;pos.x-=30.0;pos.y-=34.0;gl_Position=projMat*viewMat*vec4(pos,1.0);float cBS=(400.0+vW*250.0);gl_PointSize=cBS/gl_Position.w;gl_PointSize=clamp(gl_PointSize,1.0,45.0);f=sin(t*3.0+pos.x*0.5+pos.z);back=0.0;credit=0.0;}}", 
    
    "<imfs>stars.fs</imfs>\n\
    #version 330\n\
    out vec4 fragc;in float vW;in float f;in float back;in float credit;uniform float fade;void main(){float d=distance(gl_PointCoord,vec2(0.5));if(d>0.5)discard;float g=pow(1.0-d*2.0,2.0);vec3 c;if(back>=1.0){if(vW>=2.2){c=vec3(0.694,0.871,1);}else if(vW>=1.5){c=vec3(0.961,0.522,0.675);}else if(vW>=0.5){c=vec3(1,0.886,0.384);}else{c=vec3(0.949,0.259,0.604);}}else if(credit>=1.0){if(vW>=2.0){c=vec3(2.5,2.3,2.0);}else{c=vec3(1.8,1.8,2.5);}}else{if(vW>=4.5){c=vec3(3.0,2.7,1.8);}else if(vW>=3.5){c=vec3(2.0,1.0,0.4);}else if(vW>=2.5){c=vec3(1.2,0.15,0.6);}else if(vW>=1.5){c=vec3(0.7,0.0,1.0);}else{c=vec3(0.1,0.4,1.0);}}float i=(0.3+vW*0.2)*(1.0+f*0.2);vec3 fCol=c*i*g*(1.0-fade);fragc=vec4(fCol,g*(1.0-fade));}",
    NULL);
  _pIdClouds = gl4duCreateProgram(
    "<imvs>clouds.vs</imvs>\n\
    #version 330\n\
    layout (location=0) in vec3 vsiPosition;uniform mat4 projMat,viewMat;out vec3 vDir;void main(){vDir=vsiPosition;gl_Position=projMat*viewMat*vec4(vsiPosition,1.0);}",
          
    "<imfs>clouds.fs</imfs>\n\
    #version 330\n\
    out vec4 fC;in vec3 vDir;uniform float time;uniform float colorIntensity;float mod289(float x){return x-floor(x*(1.0/289.0))*289.0;}vec4 mod289(vec4 x){return x-floor(x*(1.0/289.0))*289.0;}vec4 perm(vec4 x){return mod289(((x*34.0)+1.0)*x);}float noise(vec3 p){vec3 a=floor(p);vec3 d=p-a;d=d*d*(3.0-2.0*d);vec4 b=a.xxyy+vec4(0.0,1.0,0.0,1.0);vec4 k1=perm(b.xyxy);vec4 k2=perm(k1.xyxy+b.zzww);vec4 c=k2+a.zzzz;vec4 k3=perm(c);vec4 k4=perm(c+1.0);vec4 o1=fract(k3*(1.0/41.0));vec4 o2=fract(k4*(1.0/41.0));vec4 o3=o2*d.z+o1*(1.0-d.z);vec2 o4=o3.yw*d.x+o3.xz*(1.0-d.x);return o4.y*d.y+o4.x*(1.0-d.y);}float fbm(vec3 p){return noise(p)*0.5+noise(p*2.02)*0.25+noise(p*4.03)*0.125;}void main(){vec3 dir=normalize(vDir);float d=fbm(dir*2.5+time*0.05);vec3 col1=vec3(0, 0, 0);vec3 col2=vec3(1, 0.2, 0.4);vec3 col3=vec3(0.0,0.8,0.9);vec3 col4=vec3(1, 0.2, 0.4); vec3 clCol;if(d<0.5){clCol=mix(col2,col3,d*2.0);}else{clCol=mix(col3,col4,(d-0.5)*2.0);}vec3 nebula=mix(col1,clCol*d,colorIntensity);vec3 finC=nebula*colorIntensity;fC=vec4(finC,1.0);}",
    NULL);
  _ribbon = glGetUniformLocation(_pId, "isRibbon");
  _color = glGetUniformLocation(_pId, "sC");
  _explosion = glGetUniformLocation(_pId, "expl");
  _isExplosion = glGetUniformLocation(_pId, "isExpl");
  _timeStars = glGetUniformLocation(_pIdStars, "t");
  _isBack = glGetUniformLocation(_pIdStars, "isBack");
  _fade = glGetUniformLocation(_pIdStars, "fade");
  _isCredit = glGetUniformLocation(_pIdStars, "isCredit");
  _colorIntensity = glGetUniformLocation(_pIdClouds, "colorIntensity");
  _timeClouds = glGetUniformLocation(_pIdClouds, "time");
  
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_PROGRAM_POINT_SIZE);
  gl4duGenMatrix(GL_FLOAT, "projMat");
  gl4duGenMatrix(GL_FLOAT, "modMat");
  gl4duGenMatrix(GL_FLOAT, "viewMat");
  gl4duBindMatrix("projMat");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _wH) / _wW, (1.0f * _wH) / _wW, 2.0f, 1000.0f);
  glViewport(0, 0, _wW, _wH);

  glGenVertexArrays(1, &_trailVao);
  glGenBuffers(1, &_trailVbo);

  glBindVertexArray(_trailVao);
  glBindBuffer(GL_ARRAY_BUFFER, _trailVbo);

  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glBindVertexArray(0);

  initStars();
  initStarsChoco();
  initStarsCredit1();
  initStarsCredit2();
  initStarsCredit3();
  initStarsCredit4();

  _sphere = gl4dgGenSpheref(20, 20);

  s1->index = 1;
  s2->index = 2;

  s1->color = (vec3){0.4f, 0.7f, 1.0f};
  s2->color = (vec3){0.922f, 0.29f, 0.447f};

  Mix_PlayMusic(gMusic, 1);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {

  if(version == 2){
    if(Mix_PlayingMusic() == 0)
      quit();
  }
  
  t = gl4dGetElapsedTime();
  dt = (t - lastFrame) / 1000.0f;
  lastFrame = t;
  t0 += dt;
  t1 += dt;
  currentTime += dt;
  switch(currentScene){
    case 1:
      scene1();
      break;
    case 2:
      scene2();
      break;
    case 3:
      scene3();
      break;
    case 4:
      scene4();
      break;
    case 5:
      scene5();
      break;
    case 6:
      scene6();
      break;
    case 7:
      scene7();
      break;
    case 8:
      scene8();
      break;
    case 9:
      scene9();
      break;
    case 10:
      scene10();
      break;
    case 11:
      scene11();
      break;
    case 12:
      scene12();
      break;
    case 13:
      scene13();
      break;
    case 14:
      scene14();
      break;
    default:
      break;
  }

  glUseProgram(0);

}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  if(_trailVao)
    glDeleteVertexArrays(1, &_trailVao);
  if(_trailVbo)
    glDeleteBuffers(1, &_trailVao);

  if(_starsVao)
    glDeleteVertexArrays(1, &_starsVao);
  if(_starsVbo)
    glDeleteBuffers(1, &_starsVbo);

  if(_chocoVao)
    glDeleteVertexArrays(1, &_chocoVao);
  if(_chocoVbo)
    glDeleteBuffers(1, &_chocoVbo);

  if(_credit1Vao)
    glDeleteVertexArrays(1, &_credit1Vao);
  if(_credit1Vbo)
    glDeleteBuffers(1, &_credit1Vbo);

  if(_credit2Vao)
    glDeleteVertexArrays(1, &_credit2Vao);
  if(_credit2Vbo)
    glDeleteBuffers(1, &_credit2Vbo);

  if(_credit3Vao)
    glDeleteVertexArrays(1, &_credit3Vao);
  if(_credit3Vbo)
    glDeleteBuffers(1, &_credit3Vbo);

  if(_credit4Vao)
    glDeleteVertexArrays(1, &_credit4Vao);
  if(_credit4Vbo)
    glDeleteBuffers(1, &_credit4Vbo);
  gl4duClean(GL4DU_ALL);
  Mix_FreeMusic( gMusic );
  gMusic = NULL;
  Mix_Quit();
  SDL_Quit();
  exit(0);
}

static void initStars(void){
  float width = 2000.0f;
  float starCoords[NUM_STARS * 4];
  for(int i = 0; i<NUM_STARS * 4; i+=4){
    starCoords[i] = ((float)rand() / RAND_MAX) * width - (width / 2.0f);
    starCoords[i+1] = ((float)rand() / RAND_MAX) * width - (width / 2.0f);
    starCoords[i+2] = ((float)rand() / RAND_MAX) * width - (width / 2.0f);
    starCoords[i+3] = (float)(rand() % (int)((3.0f + 1.0f) - 0.5f) + 0.5f);

  }
  // for(int i = 0; i < NUM_STARS * 4; i++) {
  //     // starCoords[i] = ((float)rand() / RAND_MAX) * width - (width / 2.0f);
  //     // starCoords[i] = (float)(rand()) / (float)(RAND_MAX) * width * 2.0f - width;
  // }

  glGenVertexArrays(1, &_starsVao);
  glGenBuffers(1, &_starsVbo);
  glBindVertexArray(_starsVao);
  glBindBuffer(GL_ARRAY_BUFFER, _starsVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(starCoords), starCoords, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

static void drawStars(float fade){
  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 1.0f);
  glUniform1f(_isCredit, 0.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duSendMatrices();
  gl4duBindMatrix("viewMat"); 
  gl4duSendMatrices();

  glUniform1f(_timeStars, gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(_starsVao);
  glDrawArrays(GL_POINTS, 0, NUM_STARS);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

static void reset_trail(){
  for(int i = 0; i<MAX_TRAIL_POINTS-1; ++i){
    s1->trailPos[i] = (vec3){0.0f, 0.0f, 0.0f};
    s2->trailPos[i] = (vec3){0.0f, 0.0f, 0.0f};
  }
  s1->trailCount = 0;
  s2->trailCount = 0;
}

static void calculate_trail(GLfloat x, GLfloat y, GLfloat z, short maxTrailCount, Sphere *sphere){
  for(int i = MAX_TRAIL_POINTS - 1; i > 0; i--) {
    sphere->trailPos[i].x = sphere->trailPos[i-1].x;
    sphere->trailPos[i].y = sphere->trailPos[i-1].y;
    sphere->trailPos[i].z = sphere->trailPos[i-1].z;
  }

  sphere->trailPos[0].x = x;
  sphere->trailPos[0].y = y;
  sphere->trailPos[0].z = z;

  if(sphere->trailCount < MAX_TRAIL_POINTS && sphere->trailCount < maxTrailCount) sphere->trailCount++;

  for(int i = 0; i < sphere->trailCount - 1; i++) {
    float dx = sphere->trailPos[i+1].x - sphere->trailPos[i].x;
    float dy = sphere->trailPos[i+1].y - sphere->trailPos[i].y;
    
    float len = sqrt(dx*dx + dy*dy);
    float nx = 0, ny = 0;

    if (len > 0.0001f) {
        float w = 0.5f; 
        nx = (-dy / len) * w;
        ny = ( dx / len) * w;
    }
    ribbonVertices[i*6 + 0] = sphere->trailPos[i].x + nx;
    ribbonVertices[i*6 + 1] = sphere->trailPos[i].y + ny;
    ribbonVertices[i*6 + 2] = sphere->trailPos[i].z;

    ribbonVertices[i*6 + 3] = sphere->trailPos[i].x - nx;
    ribbonVertices[i*6 + 4] = sphere->trailPos[i].y - ny;
    ribbonVertices[i*6 + 5] = sphere->trailPos[i].z;
    }
}

static vec3 catmull_rom(float t, vec3 P0, vec3 P1, vec3 P2, vec3 P3){
  float t2 = t * t;
  float t3 = t2 * t;

  vec3 res;

  res.x = 0.5 * (((-t3 + 2*t2 - t) * P0.x) + 
                ((3*t3 - 5 * t2 + 2) * P1.x) + 
                ((-3*t3 + 4*t2 + t) * P2.x) +
                ((t3 - t2) * P3.x));

  res.y = 0.5 * (((-t3 + 2*t2 - t) * P0.y) + 
                ((3*t3 - 5 * t2 + 2) * P1.y) + 
                ((-3*t3 + 4*t2 + t) * P2.y) +
                ((t3 - t2) * P3.y));

  res.z = 0.5 * (((-t3 + 2*t2 - t) * P0.z) + 
                ((3*t3 - 5 * t2 + 2) * P1.z) + 
                ((-3*t3 + 4*t2 + t) * P2.z) +
                ((t3 - t2) * P3.z));

  return res;

}

static vec3 posConv(const short *path, short index){
  vec3 p;
  p.x = path[index * 3 + 0] * 0.1f;
  p.y = path[index * 3 + 1] * 0.1f;
  p.z = path[index * 3 + 2] * 0.1f;
  return p;
}

static void catmull_position(const short pathSphere[], short numPoints, float t1, float totalPath, Sphere *sphere){
  vec3 pos = {0.0f, 0.0f, 0.0f};

  float progressTime;
  static int index = 0;
  float ti;

  progressTime = (t1 / totalPath) * (numPoints - 3);
  index = (int)progressTime;
  ti = progressTime - index;

  if (index >= numPoints - 3){
    index = numPoints - 4;
    ti = 1.0f;
  }

  if(index < numPoints - 3){
    vec3 p0 = posConv(pathSphere, index);
    vec3 p1 = posConv(pathSphere, index+1);
    vec3 p2 = posConv(pathSphere, index+2);
    vec3 p3 = posConv(pathSphere, index+3);
    pos = catmull_rom(ti, p0, p1, p2, p3);
  }
  else{
    pos.x = sphere->pos.x;
    pos.y = sphere->pos.y;
    pos.z = sphere->pos.z;
  }

  sphere->pos.x = pos.x;
  sphere->pos.y = pos.y;
  sphere->pos.z = pos.z;

  float futureTi = ti + 0.01f;
  int futureIndex = index;
  float len;

  if (futureTi >= 1.0f && index + 1 < numPoints - 3) {
    futureTi -= 1.0f;
    futureIndex += 1;
  }

  vec3 futurePos = pos;
  if(futureIndex < numPoints - 3) {
    vec3 p0 = posConv(pathSphere, futureIndex);
    vec3 p1 = posConv(pathSphere, futureIndex+1);
    vec3 p2 = posConv(pathSphere, futureIndex+2);
    vec3 p3 = posConv(pathSphere, futureIndex+3);
    futurePos = catmull_rom(futureTi, p0, p1, p2, p3);
  }

  sphere->vD.x = futurePos.x - pos.x;
  sphere->vD.y = futurePos.y - pos.y;
  sphere->vD.z = futurePos.z - pos.z;

  len = sqrt(sphere->vD.x*sphere->vD.x + sphere->vD.y*sphere->vD.y + sphere->vD.z*sphere->vD.z);
  if (len > 0.0001f) {
      sphere->vD.x /= len;
      sphere->vD.y /= len;
      sphere->vD.z /= len;
  }

  sphere->rotationLeftRight = atan2(sphere->vD.x, sphere->vD.z) * (180.0f / M_PI);
  sphere->rotationUpDown = asin(-sphere->vD.y) * (180.0f / M_PI);
}

static void draw_init(float fade, float intensity){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("viewMat");
  gl4duLoadIdentityf();
  gl4duLookAtf(xCam, yCam, zCam, xEye, yEye, zEye, xUp, yUp, zUp);

  draw_clouds(intensity);
  gl4duBindMatrix("viewMat");
  gl4duLoadIdentityf();

  gl4duLookAtf(xCam, yCam, zCam, xEye, yEye, zEye, xUp, yUp, zUp);
  glUseProgram(_pId);

  glUniform1f(_isExplosion, 0.0f);

  glUniform3f(glGetUniformLocation(_pId, "camPos"), xCam, yCam, zCam);

  drawStars(fade);
  glUseProgram(_pId);
}

static void draw_sphere(Sphere *sphere, char isTrail, short nbTrail){
  glUniform1i(_ribbon, 0);
  glUniform3f(_color, sphere->color.x, sphere->color.y, sphere->color.z);
  gl4duBindMatrix("modMat");
  gl4duLoadIdentityf();
  gl4duTranslatef(sphere->pos.x, sphere->pos.y, sphere->pos.z);
  gl4duRotatef(sphere->rotationLeftRight, 0.0f, 1.0f, 0.0f);
  gl4duRotatef(sphere->rotationUpDown, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_sphere);

  if(isTrail){
    calculate_trail(sphere->pos.x, sphere->pos.y, sphere->pos.z, nbTrail, sphere);
    glUniform1i(_ribbon, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindBuffer(GL_ARRAY_BUFFER, _trailVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ribbonVertices), ribbonVertices, GL_DYNAMIC_DRAW);

    gl4duBindMatrix("modMat");
    gl4duLoadIdentityf();
    gl4duSendMatrices();

    glBindVertexArray(_trailVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (sphere->trailCount - 1) * 2);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
  }
}

static void draw_explosion(GLfloat xT, GLfloat yT, GLfloat zT, float increase, float dt, char initExplosion){
  static GLfloat xScale = 0.01f, yScale = 0.01f, zScale = 0.01f, alphaExplosion = 1.0f;
  if(initExplosion){
    xScale = 0.01f;
    yScale = 0.01f;
    zScale = 0.01f;
    alphaExplosion = 1.0f;
  }
  glUniform1i(_ribbon, 0);
  if(currentScene == 13)
    glUniform3f(_color, 0.663f, 0.498f, 0.725f);
  else
    glUniform3f(_color, 0.961f, 0.933f, 0.875f);

  glUniform1f(_explosion, alphaExplosion);
  glUniform1f(_isExplosion, 1.0f);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  gl4duBindMatrix("modMat");
  gl4duLoadIdentityf();
  gl4duTranslatef(xT, yT, zT);
  gl4duScalef(xScale, yScale, zScale);
  gl4duSendMatrices();
  gl4dgDraw(_sphere);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  xScale += increase * dt;
  yScale += increase * dt;
  zScale += increase * dt;
  if(alphaExplosion > 0.0f)
    alphaExplosion -= 0.25f * dt;
  else
    alphaExplosion = 0.0f;
}

static void scene1(void){
  if(sceneInit){
    xCam = -60.0f;
    yCam = 0.0f;
    zCam = 40.0f;

    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;

    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;

    s1->pos.x = 0.0f;
    s1->pos.y = 0.0f;
    s1->pos.z = -1.5f;

    s1->rotationLeftRight = 0.0f;

    sceneInit = 0;
  }

  GLfloat xCamFinalPos = 2.0f;
  GLfloat zCamFinalPos = 0.0f;

  static GLfloat speedCam = 0.1f;
  static GLfloat initSpeedCam = 0.1f;
  static GLfloat maxSpeedCam = 1.6;
  static GLfloat accdecThreshold = 15.3f;
  static char acc = 1;

  static char changeCam = 0;

  static float nextSceneTime;

  if(changeCam == 0){
    if(xCam <= xCamFinalPos - 2.0f && zCam >= zCamFinalPos - 2.0f){
      xCam += 0.06f * speedCam;
      zCam -= 0.03f * speedCam;
      if(speedCam <= maxSpeedCam && acc){
        speedCam += 0.01f;
      }
      if(acc && (abs(zCamFinalPos - zCam) <= accdecThreshold || abs(zCamFinalPos - zCam) <= accdecThreshold)){
        acc = 0;
      }
      if(speedCam > initSpeedCam && !acc){
        speedCam -= 0.003f;
      }
      if(t0 >= 1.0f){
        s1->rotationLeftRight += 0.5f;
      }
    } else{
      changeCam = 1;
      nextSceneTime = t0;
    }
  } else if(changeCam == 1) {
    yCam = 2.0f;

    static float cameraAngle = 0.0f;
    static float sphereRotationSpeed = 0.5f;
    cameraAngle -= 0.3f * dt;

    xCam = -15.5f * cos(cameraAngle);
    zCam = -15.5f * sin(cameraAngle);

    s1->rotationLeftRight += sphereRotationSpeed;
    if(t0 >= nextSceneTime + 3.0f && sphereRotationSpeed >= 0.0f){
      sphereRotationSpeed -= 0.001f;
    }
    if(t0 >= nextSceneTime + 7.0f && sphereRotationSpeed >= 0.0f){
      changeCam = 3;
    }
  } else {
    xCam = 15.0f;
    yCam = 0.0f;
    zCam = 15.0f;
    s1->rotationLeftRight = 90.0f;
    currentScene = 2;
    sceneInit = 1;
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(s1, 0, 0);
}

static void scene2(void){
  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    t1 = 0.0f;
    currentTime = 0.0f;
    s1->pos.x = 0.0f;
    s1->pos.y = 0.0f;
    s1->pos.y = 0.0f;

    xCam = 15.0f;
    yCam = 0.0f;
    zCam = 15.0f;

    xEye = s1->pos.x;
    yEye = s1->pos.y;
    zEye = s1->pos.z;

    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;

    sceneInit = 0;
  }
  const short path[] = {
    -330, 0, 0,
    3, 0, 0,
    600, 0, 0,
    630, 0, 0
  };

  static const float nextSceneTime = 6.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 3;
    sceneInit = 1;
    return;
  }

  static const float totalTime = 10.0f;

  short numPoints = sizeof(path) / sizeof(path[0]) / 3;

  catmull_position(path, numPoints, t1, totalTime, s1);

  xEye = s1->pos.x;
  yEye = s1->pos.y;
  zEye = s1->pos.z;
  draw_init(0.0f, 0.7f);
  draw_sphere(s1, 1, 300);
}

static void scene3(void){
  static short currentShot = 1;
  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    t0 = 0.0f;
    currentTime = 0.0f;
    reset_trail();
    xCam = 0.0f;
    yCam = 50.0f;
    zCam = 8.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 0.0f;
    zUp = -1.0f;
    sceneInit = 0;
  }

  static const float nextSceneTime = 15.0f;
  static const float nextShotTime = 3.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 4;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 5){
    currentShot++;
    t0 = 0.0f;
  }

  short numPoints;
  static short *path;

  const short path1[] = {
    -400, 0, -40,
    -400, 0, -40,
    200, 0, -30,
    500, 0, -30,
    500, 0, -30
  };

  const short path2[] = {
    300, 0, -170,
    200, 0, -150,
    -250, 0, 300,
    -350, 0, 300,
    -400, 0, 400
  };

  const short path3[] = {
    400, 0, 300,
    300, 0, 200,
    -200, 0, -200,
    -250, 0, -200,
    -400, 0, -300
  };

  const short path4[] = {
    400, 0, 70,
    300, 0, 60,
    -400, 0, -30,
    -400, 0, -30,
    -500, 0, -50
  };

  const short path5[] = {
    -400, 60, 60,
    -150, 300, 50,
    130, 300, 50,
    150, 300, 50,
    400, 350, 40
  };

  if(currentShot == 1 && path != path1){
    path = path1;
    numPoints = sizeof(path1) / sizeof(path1[0]) / 3;
    reset_trail();
  } else if(currentShot == 2 && path != path2){
    path = path2;
    numPoints = sizeof(path2) / sizeof(path2[0]) / 3;
    reset_trail();
  } else if(currentShot == 3 && path != path3){
    path = path3;
    numPoints = sizeof(path3) / sizeof(path3[0]) / 3;
    reset_trail();
  } else if(currentShot == 4 && path != path4){
    path = path4;
    numPoints = sizeof(path4) / sizeof(path4[0]) / 3;
    reset_trail();
  } else if(currentShot == 5 && path != path5){
    path = path5;
    numPoints = sizeof(path5) / sizeof(path5[0]) / 3;
    reset_trail();
  }

  static const float totalTime = 5.0f;
  catmull_position(path, numPoints, t0, totalTime, s1);

  draw_init(0.0f, 0.7f);
  draw_sphere(s1, 1, 100);
}

static void scene4(void){
  static short currentShot = 1;
  float shotsTime[] = {
    5.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 3.0f
  };
  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    reset_trail();
    xCam = 5.0f;
    yCam = 0.0f;
    zCam = 10.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    s1->rotationLeftRight = 90.0f;
    s1->rotationLeftRight = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 27.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 5;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 7){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short path[] = {
    -400, 0, 0,
    -300, 0, 0,
    -200, 0, 0,
    -50, 0, 0,
    0, 0, 0,
    50, 0, 0
  };

  short numPoints = sizeof(path) / sizeof(path[0]) / 3;

  if(currentShot == 1){
    catmull_position(path, numPoints, t0, nextShotTime, s1);
  } else if(currentShot == 2){
    if(s1->rotationLeftRight <= 50.0f + 90.0f){
      s1->rotationLeftRight += 30.0f * dt;
    }
  } else if(currentShot == 3){
    if(s1->rotationLeftRight >= -30.0f){
      s1->rotationLeftRight -= 30.0f * dt;
    }
  } else if(currentShot == 4){
    zCam = 100.0f;
  } else if(currentShot == 5){
    zCam = 10.0f;
    if(s1->rotationLeftRight <= 90.0f){
      s1->rotationLeftRight += 30.0f * dt;
    }
  } else if(currentShot == 6){
    if(s1->rotationUpDown <= 20.0f){
      s1->rotationUpDown += 5.0f * dt;
    }
  } else if(currentShot == 7){
    if(s1->rotationUpDown>= 0.0f){
      s1->rotationUpDown -= 50.0f * dt;
    }
  }

  xEye = s1->pos.x + 0.1f;
  yEye = s1->pos.y + 0.1f;
  zEye = s1->pos.z + 0.1f;

  draw_init(0.0f, 0.7f);
  draw_sphere(s1, 1, 200);
}

static void scene5(void){
  static short currentShot = 1;
  float shotsTime[] = {
    4.0f, 8.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    t0 = 0.0f;
    currentTime = 0.0f;
    reset_trail();
    s1->pos.x = 0.0f;
    s1->pos.y = 0.0f;
    s1->pos.z = 0.0f;
    xCam = -20.0f;
    yCam = 1.0f;
    zCam = 10.0f;
    xEye = 200.0f;
    yEye = 0.0f;
    zEye = -10.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 10.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 6;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 1){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short path[] = {
    -10, 0, 0,
    0, 0, 0,
    400, 0, -100,
    800, 0, -100,
    1200, 0, -100,
    1400, 0, -100,
    1600, 0, -100,
    1750, 0, -100,
    1900, 0, -100,
    2000, 0, -100
  };

  short numPoints = sizeof(path) / sizeof(path[0]) / 3;

  if(currentShot == 1)
    s1->rotationLeftRight = 90.0f;
  else if(currentShot == 2){
    catmull_position(path, numPoints, t0, nextShotTime, s1);

    static float nXCam = 5.0f;
    static float nYCam = 0.0f;
    static float nZCam = 10.0f;

    xCam = nXCam;
    yCam = nYCam;
    zCam = nZCam;
    xEye = s1->pos.x;
    yEye = s1->pos.y;
    zEye = s1->pos.z;

    nYCam += 2.8f * dt;
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(s1, 1, 200);

  glUniform1i(_ribbon, 0);
  glUniform3f(_color, sin(currentTime * 5.0f) * 0.5f + 0.5f, 
                          sin(currentTime * 5.0f) * 0.5f + 0.5f, sin(currentTime * 5.0f) * 0.5f + 0.5f);
  gl4duBindMatrix("modMat");
  gl4duLoadIdentityf();
  gl4duTranslatef(200.0f, 0.0f, -10.0f);

  gl4duSendMatrices();
  gl4dgDraw(_sphere);

}

static void scene6(void){
  static short currentShot = 1;
  float shotsTime[] = {
    4.0f, 10.0f, 10.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    reset_trail();
    xCam = -10.0f;
    yCam = 1.0f;
    zCam = 20.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static float rSphere2 = 0.0f;
  static float gSphere2 = 0.0f;
  static float bSphere2 = 0.0f;

  static const float nextSceneTime = 23.0f;
  
  if(currentTime >= nextSceneTime){
    currentScene = 7;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 2){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  static short *path;
  static short numPoints;

  const short path1[] = {
    -200, 0, 0,
    -150, 0, 0,
    -70, 0, 0,
    -50, 0, 0,
    -40, 0, 0,
    -30, 0, 0,
    -20, 0, 0
  };

  const short path2[] = {
    -40, 0, 0,
    -30, 0, 0,
    20, 30, -30,
    40, 20, -20,
    30, 20, 20,
    30, 20, 20,
    30, 20, 20,
    30, 20, 20,
    20, 10, 40,
    0, 0, 60,
    -40, -20, 60
  };

  const short path3[] = {
    -50, -20, 60,
    0, 0, 90,
    -50, 30, 60,
    -50, 30, 60,
    -50, 30, 60,
    -50, 30, 60,
    0, 30, -90,
    0, 30, -90,
    0, 30, -90,
    0, 30, -90,
    10, 40, -100
  };

  if(currentShot == 1 && path != path1){
    path = path1;
    numPoints = sizeof(path1) / sizeof(path1[0]) / 3;
    reset_trail();
  } else if(currentShot == 2 && path != path2){
    path = path2;
    numPoints = sizeof(path2) / sizeof(path2[0]) / 3;
    reset_trail();
  } else if(currentShot == 3 && path != path3){
    path = path3;
    numPoints = sizeof(path3) / sizeof(path3[0]) / 3;
    reset_trail();
  }

  catmull_position(path, numPoints, t0, nextShotTime, &spheres[0]);

  if(currentShot == 1){
    static float flicker = 5.0f;
    flicker += dt * 3.5f;

    rSphere2 = sin(currentTime * flicker) * 0.5f + 0.5f;
    gSphere2 = sin(currentTime * flicker) * 0.5f + 0.5f;
    bSphere2 = sin(currentTime * flicker) * 0.5f + 0.5f;

  } else if(currentShot == 2){

    static float nRSphere2 = 0.0f;
    static float nGSphere2 = 0.0f;
    static float nBSphere2 = 0.0f;
    
    nRSphere2 = nRSphere2 < 0.922f ? nRSphere2 + 0.922f * dt * 1.1f : 0.922f;
    nGSphere2 = nGSphere2 < 0.29f ? nGSphere2 + 0.29f * dt * 1.1f : 0.29f;
    nBSphere2 = nBSphere2 < 0.447f ? nBSphere2 + 0.447f * dt * 1.1f : 0.447f;

    rSphere2 = nRSphere2;
    gSphere2 = nGSphere2;
    bSphere2 = nBSphere2;

  } else if(currentShot == 3){
    xCam = spheres[0].pos.x;
    yCam = spheres[0].pos.y;
    zCam = spheres[0].pos.z;
    
  }

  spheres[0].vD.x = 0.0f - spheres[0].pos.x;
  spheres[0].vD.y = 0.0f - spheres[0].pos.y;
  spheres[0].vD.z = 0.0f - spheres[0].pos.z;

  float len = sqrt(spheres[0].vD.x*spheres[0].vD.x + spheres[0].vD.y*spheres[0].vD.y + spheres[0].vD.z*spheres[0].vD.z);
  if (len > 0.0001f) {
      spheres[0].vD.x /= len;
      spheres[0].vD.y /= len;
      spheres[0].vD.z /= len;
  }

  spheres[0].rotationLeftRight = atan2(spheres[0].vD.x, spheres[0].vD.z) * (180.0f / M_PI);
  spheres[0].rotationUpDown = asin(-spheres[0].vD.y) * (180.0f / M_PI);

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 50);

  glUniform1i(_ribbon, 0);
  glUniform3f(_color, rSphere2, gSphere2, bSphere2);
  gl4duBindMatrix("modMat");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, 0.0f);

  spheres[1].vD.x = spheres[0].pos.x;
  spheres[1].vD.y = spheres[0].pos.y;
  spheres[1].vD.z = spheres[0].pos.z;

  len = sqrt(spheres[1].vD.x*spheres[1].vD.x + spheres[1].vD.y*spheres[1].vD.y + spheres[1].vD.z*spheres[1].vD.z);
  if (len > 0.0001f) {
      spheres[1].vD.x /= len;
      spheres[1].vD.y /= len;
      spheres[1].vD.z /= len;
  }

  spheres[1].rotationLeftRight = atan2(spheres[1].vD.x, spheres[1].vD.z) * (180.0f / M_PI);
  spheres[1].rotationUpDown = asin(-spheres[1].vD.y) * (180.0f / M_PI);

  if(currentShot==3){
    gl4duRotatef(spheres[1].rotationLeftRight, 0.0f, 1.0f, 0.0f);
    gl4duRotatef(spheres[1].rotationUpDown, 1.0f, 0.0f, 0.0f);
  }
  gl4duSendMatrices();
  gl4dgDraw(_sphere);
}

static void scene7(void){
  static short currentShot = 1;
  float shotsTime[] = {
    3.0f, 0.5f, 10.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    reset_trail();
    spheres[0].pos.x = -3.0f;
    spheres[0].pos.y = 3.0f;
    spheres[0].pos.z = 0.0f;

    spheres[1].pos.x = 3.0f;
    spheres[1].pos.y = -3.0f;
    spheres[1].pos.z = 0.0f;

    xCam = 0.0f;
    yCam = 0.0f;
    zCam = 20.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 14.0f;
  
  static float t3 = 0.0f;
  static float t2 = 0.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 8;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 2){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short pathSphere1[] = {
    -11, 11, 10,
    -11, 11, 0,
    -60, 20, -400,
    -10, -40, -900,
    10, -20, -1300,
    -40, -40, -1400
  };

  const short pathSphere2[] = {
    11, -11, 10,
    11, -11, 0,
    50, 10, -400,
    10, 50, -900,
    30, -10, -1300,
    40, 40, -1400
  };

  short numPoints1 = sizeof(pathSphere1) / sizeof(pathSphere1[0]) / 3;
  short numPoints2 = sizeof(pathSphere2) / sizeof(pathSphere2[0]) / 3;
  
  yCam += 1.1f * dt;
  zCam -= 1.1f * dt;
  zEye -= 5.0f * dt;

  if(currentShot == 1){
    spheres[0].rotationLeftRight = 90.0f;
    spheres[0].rotationUpDown = 45.0f;
    spheres[1].rotationLeftRight = -90.0f;
    spheres[1].rotationUpDown = -45.0f;

    spheres[0].pos.x += 0.6f * dt;
    spheres[0].pos.y -= 0.6f * dt;

    spheres[1].pos.x -= 0.6f * dt;
    spheres[1].pos.y += 0.6f * dt;
  } else if(currentShot == 2){
    if(spheres[1].rotationLeftRight >= -180.0f)
      spheres[1].rotationLeftRight -= 200.0f * dt;

    spheres[0].pos.x += 0.1f * dt;
    spheres[0].pos.y -= 0.1f * dt;
  } else if(currentShot == 3){
    static int sphere1Turn = 1;
    if(sphere1Turn && spheres[0].pos.z - spheres[1].pos.z <= 15.0f){
      spheres[0].rotationLeftRight += 30.0f * dt;
    } else {
      sphere1Turn = 0;
      catmull_position(pathSphere1, numPoints1, t3, nextShotTime, &spheres[0]);
      t3 += dt;
    }
    catmull_position(pathSphere2, numPoints2, t2, nextShotTime, &spheres[1]);
    t2 += dt;
  }

  draw_init(0.0f, 0.7f);

  if(currentShot != 1){
    draw_sphere(&spheres[0], 1, 200);
    draw_sphere(&spheres[1], 1, 200);
  } else{
    draw_sphere(&spheres[0], 0, 0);
    draw_sphere(&spheres[1], 0, 0);
  }
}

static void scene8(void){
  static short currentShot = 1;
  float shotsTime[] = {
    5.0f, 2.0f, 4.0f, 3.0f, 4.0f, 2.0f, 8.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].pos.x = -3.0f;
    spheres[0].pos.y = 3.0f;
    spheres[0].pos.z = 50.0f;
    spheres[1].pos.x = 3.0f;
    spheres[1].pos.y = -3.0f;
    spheres[1].pos.z = 50.0f;
    xCam = 0.0f;
    yCam = 0.0f;
    zCam = 50.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 30.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 29.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 9;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 6){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short pathSphere1[] = {
    -30, 30, 600,
    -40, -20, 500, 
    -33, -24, 430,
    -20, -30, 300,
    -23, -12, 250,
    -30, 20, 150,
    -25, 31, 50,
    -5, 42, 0,
    0, 42, -100,
    10, 50, -150,
    15, 30, -250,
    12, 20, -450,
    -30, 20, -650,
    -20, -70, -800,
    -20, -300, -800,
    -25, -500, -900,
    -20, -550, -1000,
    -20, -550, -1000, 
    -20, -550, -1000,
    -20, -550, -1000,
    -20, -550, -1000,
    -50, -460, -1100,
    -50, -370, -1200,
    0, -280, -1250,
    50, -190, -1200,
    0, -100, -1150,
    -50, -10, -1100,
    -50, 80, -1200, 
    0, 170, -1250, 
    50, 260, -1200, 
    0, 350, -1150,
    -50, 440, -1200, 
    0, 530, -1250, 
    50, 620, -1200, 
    0, 660, -1150,
    -45, 695, -1150,
    -40, 710, -1140
  };

  const short pathSphere2[] = {
    30, 30, 600,
    40, 20, 500,
    20, 30,  100,
    40, 60, -200,
    30, 20, -600,
    30, -100, -800,
    20, -500, -950,
    0, -550, -1100,
    0, -350, -1200,
    0, 200, -1200,
    0, 1500, -1200,
    -100, 1600, -1200,
    -200, 1400, -1200,
    -250, 1100, -1190,
    -150,  900, -1170,
    -55,  705, -1150,
    -40,  690, -1160
  };

  short numPoints1 = sizeof(pathSphere1) / sizeof(pathSphere1[0]) / 3;
  short numPoints2 = sizeof(pathSphere2) / sizeof(pathSphere2[0]) / 3;

  if(currentShot != 7){
    catmull_position(pathSphere1, numPoints1, t1, 20.0f, &spheres[0]);
    catmull_position(pathSphere2, numPoints2, t1, 20.0f, &spheres[1]); 
  }

  if(currentShot == 1){
    xCam = 0.0f;
    yCam = 10.0f;
    zCam -= 6.5f * dt;
    yEye = 5.0f;
    zEye -= 6.0f * dt;
  } else if(currentShot == 2){
    zEye -= 1.7f * dt;
    yEye -= 0.05f * dt;
    zCam -= 5.0f * dt; 
  } else if(currentShot == 3){
    static char shot3init = 1;
    if(shot3init){
      xCam = spheres[0].pos.x - 8.0f;
      yCam = spheres[0].pos.y;
      zCam = spheres[0].pos.z + 8.0f;
      shot3init = 0;
    }

    xEye = spheres[1].pos.x - 3.0f;
    yEye = spheres[1].pos.y - 1.0f;
    zEye = spheres[1].pos.z;
    if(spheres[0].pos.y <= -54.0f && spheres[0].pos.y >= -56.0f){
      spheres[0].rotationLeftRight = 0.0f;
    }
  } else if(currentShot == 4){
    xCam = spheres[0].pos.x - 20.0f;
    yCam = spheres[0].pos.y + 15.0f;
    zCam = spheres[0].pos.z - 15.0f;

    xEye = spheres[0].pos.x;
    yEye = spheres[0].pos.y;
    zEye = spheres[0].pos.z;
  } else if(currentShot == 5){
    xCam = spheres[1].pos.x - 20.0f;
    yCam = spheres[1].pos.y + 50.0f;
    zCam = spheres[1].pos.z - 30.0f;

    xEye = 0.0f;
    yEye = 50.0f;
    zEye = -115.0f;
  } else if(currentShot == 6){
    static char shot6init = 1;
    if(shot6init){
      xCam = -30.0f;
      yCam = 50.0f;
      zCam = -50.0f;
      shot6init = 0;
    }

    float dx = xEye - xCam;
    float dy = yEye - yCam;
    float dz = zEye - zCam;

    float dist = sqrt(dx*dx + dy*dy + dz*dz);

    float zoomSpeed = 30.0f; 

    if(dist > 5.0f){
      xCam += (dx / dist) * zoomSpeed * dt;
      yCam += (dy / dist) * zoomSpeed * dt;
      zCam += (dz / dist) * zoomSpeed * dt;
    }

    xEye = -5.0f;
    yEye = 70.0f;
    zEye = -115.0f;
  } else if(currentShot == 7){
    static float pushSpeed = 150.0f;
    static float pushSphereSpeed = 11.0f;

    float dx = xCam - xEye;
    float dy = yCam - yEye;
    float dz = zCam - zEye;

    float dist = sqrt(dx*dx + dy*dy + dz*dz);

    if(dist > 0.0f){
      xCam += (dx / dist) * pushSpeed * dt;
      yCam += (dy / dist) * pushSpeed * dt;
      zCam += (dz / dist) * pushSpeed * dt;
    }

    spheres[0].pos.x += pushSphereSpeed * dt; 
    spheres[0].pos.y -= pushSphereSpeed * dt;

    spheres[1].pos.x -= pushSphereSpeed * dt;
    spheres[1].pos.y += pushSphereSpeed * dt;

    if(pushSpeed > 20.0f)
      pushSpeed -= 120.0f * dt;
    
    if(pushSphereSpeed >= 0.0f)
      pushSphereSpeed -= 8.0f * dt;
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 200);
  draw_sphere(&spheres[1], 1, 450);

  if(currentShot == 7){
    static char initExplosion = 1;
    if(initExplosion)
      draw_explosion(-5.0f, 70.0f, -115.0f, 35.5f, dt, 1);
    else
      draw_explosion(-5.0f, 70.0f, -115.0f, 35.5f, dt, 0);
    initExplosion = 0;
  }

}

static void scene9(void){
  static int initScene9 = 1;
  static int currentShot = 1;

  float shotsTime[] = {
    7.0f, 2.0f, 4.0f, 5.0f
  };

  static float nextShotTime;

  if(initScene9){
    glUniform1f(_isExplosion, 0.0f);
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].rotationLeftRight = -90.0f;
    spheres[0].rotationUpDown = 45.0f;
    spheres[1].rotationLeftRight = 90.0f;
    spheres[1].rotationUpDown = 45.0f;
    spheres[0].pos.x = 10.0f;
    spheres[0].pos.y = -10.0f;
    spheres[0].pos.z = 0.0f;
    spheres[1].pos.x = -10.0f;
    spheres[1].pos.y = 10.0f;
    spheres[1].pos.z = 0.0f;
    xCam = -10.0f;
    yCam = -10.0f;
    zCam = 20.0f;
    xEye = 10.0f;
    yEye = -10.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    initScene9 = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 18.0f;
  
  if(currentTime >= nextSceneTime){
    currentScene = 10;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 3){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  if(currentShot == 1){
    spheres[0].rotationUpDown -= 6.0f * dt;
    xCam += 1.1f * dt;
    zCam -= 1.1f * dt;
  } else if(currentShot == 2){
    yCam = -13.0f;
    xCam = 15.0f;
    zCam = 3.0f;

    xEye = spheres[1].pos.x;
    yEye = spheres[1].pos.y;
    zEye = spheres[1].pos.z;

  } else if(currentShot == 3){
    static float speed = 2.0f;
    if(spheres[1].rotationUpDown >= 0.0f)
      spheres[1].rotationUpDown -= 30.0f * dt;
    spheres[1].pos.x += speed * dt;
    if(speed <= 25.0f)
      speed += 7.0f * dt;
    spheres[0].rotationLeftRight += 25.0f * dt;

    xEye = spheres[1].pos.x;
    yEye = spheres[1].pos.y;
    zEye = spheres[1].pos.z;
  } else if(currentShot == 4){
    spheres[1].pos.x += 25.0f * dt;
    spheres[0].pos.x += 35.0f * dt;
    spheres[0].pos.y += spheres[1].pos.y * dt * 0.2;

    spheres[0].rotationLeftRight = 90.0f;

    xEye = spheres[1].pos.x;
    yEye = spheres[1].pos.y;
    zEye = spheres[1].pos.z;
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 200);
  draw_sphere(&spheres[1], 1, 200);
}

static void scene10(void){
  static int currentShot = 1;

  float shotsTime[] = {
    20.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].rotationLeftRight = -90.0f;
    spheres[0].rotationUpDown = 45.0f;
    spheres[1].rotationLeftRight = 90.0f;
    spheres[1].rotationUpDown = 45.0f;
    spheres[0].pos.x = -160.0f;
    spheres[0].pos.y = 0.0f;
    spheres[0].pos.z = -50.0f;
    spheres[1].pos.x = 2.0f;
    spheres[1].pos.y = 5.0f;
    spheres[1].pos.z = 0.0f;
    xCam = -180.0f;
    yCam = -5.0f;
    zCam = 50.0f;
    xEye = 10.0f;
    yEye = -10.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 18.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 11;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 1){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short pathSphere1[] = {
    -1410, 30, -30,
    -1400, 30, -30,
    -1200, 20, 0,
    -1000, 70, -70,
    -800, 40, -50,
    -500, 60, -20,
    -200, 20, 30,
    0, 100, -100,
    280, 50, -50,
    600, 60, 0,
    800, 0, -60,
    1000, -60, 0,
    1200, 0, 60,
    1400, 60, 0,
    1600, 0, -60,
    1800, -60, 0,
    2000, 0, 60,
    2200, 60, 0,
    2400, 0, -60,
    2600, -60, 0,
    2800, 0, 60,
    3000, 60, 0,
    3200, 0, -60,
    3400, -60, 0,
    3600, 0, 60,
    3800, 60, 0,
    4000, 0, -60,
    4200, -60, 0,
    4400, 0, 60,
    4800, 60, 0,
    5000, 0, -60,
    5200, -60, 0,
    5400, 0, 60,
    5600, 60, 0,
    5800, 0, -60,
    6000, -60, 0,
    6200, 0, 60,
    6210, 10, 90
  };

  const short pathSphere2[] = {
    -1460, 0, 30,
    -1450, 0, 30,
    -1200, -50, 60,
    -1000, -20, 40,
    -760, 20, 20,
    -540, 0, 40,
    -180, 30, -20,
    5, -50, 0,
    300, 40, 30,
    600, -60, 0,
    800, 0, 60,
    1000, 60, 0,
    1200, 0, -60,
    1400, -60, 0,
    1600, 0, 60,
    1800, 80, 0,
    2000, 0, -60,
    2200, -60, 0,
    2400, 0, 60,
    2600, 80, 0,
    2800, 0, -60,
    3000, -60, 0,
    3200, 0, 60,
    3400, 80, 0,
    3600, 0, -60,
    3800, -60, 0,
    4000, 0, 60,
    4200, 80, 0,
    4400, 0, -60,
    4600, -60, 0,
    4800, 0, 60,
    5000, 80, 0,
    5200, 0, -60,
    5400, -60, 0,
    5600, 0, 60,
    5800, 80, 0,
    6000, 0, -60,
    6010, 10, 90
  };

  short numPoints1 = sizeof(pathSphere1) / sizeof(pathSphere1[0]) / 3;
  short numPoints2 = sizeof(pathSphere2) / sizeof(pathSphere2[0]) / 3;
  catmull_position(pathSphere1, numPoints1, t1, 40.0f, &spheres[0]);
  catmull_position(pathSphere2, numPoints2, t1, 40.0f, &spheres[1]);

  if(currentShot == 1){
    static float speed = 30.0f;
    static char turn = 0;
    xCam += speed * dt;
    xEye = xCam - 20.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    if(xEye >= spheres[0].pos.x && !turn)
      turn = 1;
    if(turn){
      if(speed >= 20.0f)
        speed -= 2.0f * dt;
      yCam += 1.0f * dt;
      xEye = (spheres[0].pos.x - 1.0f);
      zCam -= 7.0f * dt;
    }
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 400);
  draw_sphere(&spheres[1], 1, 400);
}

static void scene11(void){
  static int initScene11 = 1;
  static int currentShot = 1;

  float shotsTime[] = {
    50.0f
  };

  static float nextShotTime;

  if(initScene11){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].rotationLeftRight = -0.0f;
    spheres[0].rotationUpDown = 0.0f;
    spheres[1].rotationLeftRight = 0.0f;
    spheres[1].rotationUpDown = 0.0f;
    spheres[0].pos.x = 0.0f;
    spheres[0].pos.y = 0.0f;
    spheres[0].pos.z = 0.0f;
    spheres[1].pos.x = 0.0f;
    spheres[1].pos.y = 0.0f;
    spheres[1].pos.z = 0.0f;
    xCam = 0.0f;
    yCam = 0.0f;
    zCam = 160.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    initScene11 = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 12.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 12;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 1){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short pathSphere1[] = {
    -110, -370, 1610,
    -60, -360, 1600,
    -30, -340, 1000,
    0, -280, 10,
    280, 0, -20,
    380, 150, -20,
    420, 300, -20,
    380, 450, -20,
    280, 520, -20,
    180, 540, -20,
    100, 510, -20,
    0, 400, 0,
    -50, 200, 70,
    -50, 100, 300,
    -60, -70, 1600,
    -110, -80, 1610
  };

  const short pathSphere2[] = {
    120, -370, 1610,
    50, -360, 1600,
    30, -340, 900,
    0, -280, 10,
    -280, 0, -20,
    -380, 150, -20,
    -420, 300, -20,
    -380, 450, -20,
    -280, 520, -20,
    -180, 540, -20,
    -100, 510, -20,
    0, 400, 0,
    50, 200, 70,
    50, 100, 300,
    60, -70, 1600,
    110, -80, 1610
  };

  short numPoints1 = sizeof(pathSphere1) / sizeof(pathSphere1[0]) / 3;
  short numPoints2 = sizeof(pathSphere2) / sizeof(pathSphere2[0]) / 3;

  catmull_position(pathSphere1, numPoints1, t1, 12.0f, &spheres[0]);
  catmull_position(pathSphere2, numPoints2, t1, 11.0f, &spheres[1]);

  xCam = 0.0f;
  yCam = 0.0f;
  zCam -= dt * 1.2;
  yEye = spheres[0].pos.y;

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 550);
  draw_sphere(&spheres[1], 1, 550);
}

static void scene12(void){
  static int currentShot = 1;
  float shotsTime[] = {
    12.0f, 4.0f, 2.5f, 2.5f, 10.0f
  };

  static float nextShotTime;

  if(sceneInit){
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].rotationLeftRight = 0.0f;
    spheres[0].rotationUpDown = 0.0f;
    spheres[1].rotationLeftRight = 0.0f;
    spheres[1].rotationUpDown = 0.0f;
    spheres[0].pos.x = 0.0f;
    spheres[0].pos.y = 0.0f;
    spheres[0].pos.z = 0.0f;
    spheres[1].pos.x = 0.0f;
    spheres[1].pos.y = 0.0f;
    spheres[1].pos.z = 0.0f;
    xCam = -10.0f;
    yCam = 10.0f;
    zCam = 0.0f;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    sceneInit = 0;
    nextShotTime = shotsTime[0];
  }


  static const float nextSceneTime = 30.0f;
  
  if(currentTime >= nextSceneTime){
    currentScene = 13;
    sceneInit = 1;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 5){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  const short pathSphere1[] = {
    0, 0, 0,
    -150, 20, 100,
    -220, 0, -50,
    -100, -10, -200,
    50, 0, -120,
    180, 10, 180,
    150, -10, 100,
    0, 20, 50,
    -20, 0, 140,
    -120, 10, 50,
    -120, 50, 40,
    80, -30, 80,
    150, 100, 120,
    0, 20, 180,
    -200, -50, 220,
    -50, -120, 280,
    120, 0, 320,
    40, 80, 380,
    -100, 20, 420,
    0, -80, 480,
    50, 0, 940,
    40, 20, 1000,
    300, 20, 1050,
    800, 50, 1100,
    1500, 0, 1150,
    1600, 0, 1150
  };

  const short pathSphere2[] = {
    50, 0, 50,
    200, -10, -100,
    100, 10, -250,
    -150, 0, -180,
    -50, 20, 0,
    -200, 0, 150,
    -100, -10, 250,
    -50, -20, -200,
    50, 0, 200,
    100, 0, -20,
    30, 10, -80,
    150, -40, 50,
    -100, 20, 100,
    -220, -80, 150,
    0, 50, 200,
    180, 100, 250,
    80, -20, 320,
    -150, 0, 380,
    -40, -100, 440,
    120, 60, 500,
    -20, 0, 980,
    -300, -20, 1050,
    -800, -50, 1100,
    -1500, 0, 1150,
    -1600, 0, 1150
  };

  short numPoints1 = sizeof(pathSphere1) / sizeof(pathSphere1[0]) / 3;
  short numPoints2 = sizeof(pathSphere2) / sizeof(pathSphere2[0]) / 3;
  catmull_position(pathSphere1, numPoints1, t1, 25.0f, &spheres[0]);
  catmull_position(pathSphere2, numPoints2, t1, 25.0f, &spheres[1]);

  if(currentShot == 1){
    static float speed = 4.0f;
    if(speed >= 2.0f)
      speed -= 1.5f * dt;
    xCam += speed * dt;
    yCam += speed * dt;
    zCam += speed * dt;
  } else if(currentShot == 2){
    static char initShot2 = 1;
    if(initShot2){
      xCam = -100.0f;
      yCam = 100.0f;
      zCam = 0.0f;
      initShot2 = 0;
    }
    static float rotAngle = 0.0f;
    rotAngle += 0.01f;

    xCam = xEye + 100.0f * sinf(rotAngle);
    zCam = zEye + 100.0f * cosf(rotAngle);
    
  } else if(currentShot == 3){
    xCam = spheres[0].pos.x + spheres[0].vD.x * 20.0f;
    yCam = spheres[0].pos.y + spheres[0].vD.y * 20.0f;
    zCam = spheres[0].pos.z + spheres[0].vD.z * 20.0f;

    xEye = spheres[0].pos.x;
    yEye = spheres[0].pos.y;
    zEye = spheres[0].pos.z;
  } else if(currentShot == 4){
    xCam = spheres[1].pos.x + spheres[1].vD.x * 20.0f;
    yCam = spheres[1].pos.y + spheres[1].vD.y * 20.0f;
    zCam = spheres[1].pos.z + spheres[1].vD.z * 20.0f;

    xEye = spheres[1].pos.x;
    yEye = spheres[1].pos.y;
    zEye = spheres[1].pos.z;
  } else if(currentShot == 5){
    static char shot8init = 1;
    if(shot8init){
      xCam = 0.0f;
      yCam = 0.0f;
      zCam = 90.0f;
      shot8init = 0;
    }
    zCam -= 40.0f * dt;
    xEye = 0.0f;
    yEye = 0.0f;
    zEye = 100.0f;
  }

  draw_init(0.0f, 0.7f);
  draw_sphere(&spheres[0], 1, 550);
  draw_sphere(&spheres[1], 1, 550);
}

static void scene13(void){
  static int initScene13 = 1;
  static int currentShot = 1;

  static float nebIntense = 0.7f;

  float shotsTime[] = {
    2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 3.0f, 3.0f, 2.0f, 2.0f, 3.4f, 7.0f
  };

  static float nextShotTime;

  if(initScene13){
    glUniform1f(_isExplosion, 0.0f);
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    spheres[0].rotationLeftRight = -90.0f;
    spheres[0].rotationUpDown = 0.0f;
    spheres[1].rotationLeftRight = 90.0f;
    spheres[1].rotationUpDown = 0.0f;
    spheres[0].pos.x = -200.0f;
    spheres[0].pos.y = 0.0f;
    spheres[0].pos.z = -500.0f;
    spheres[1].pos.x = 200.0f;
    spheres[1].pos.y = 0.0f;
    spheres[1].pos.z = -500.0f;
    xCam = 200.0f;
    yCam = 10.0f;
    zCam = -480.0f;
    xEye = 220.0f;
    yEye = 0.0f;
    zEye = -500.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    initScene13 = 0;
    nextShotTime = shotsTime[0];
  }

  static const float nextSceneTime = 30.0f;

  if(currentTime >= nextSceneTime){
    currentScene = 14;
    return;
  }

  if(t0 >= nextShotTime && currentShot <= 11){
    nextShotTime = shotsTime[currentShot];
    currentShot++;
    t0 = 0.0f;
  }

  if(currentShot == 1){
    static float speed = 30.0f;
    spheres[1].pos.x += speed * dt;
    if(speed > 0.0f)
      speed -= 20.0f * dt;
    else 
      speed = 0.0f;
  } else if(currentShot == 2){
    static float speed = 30.0f;
    xEye = -220.0f;
    xCam = -200.0f;
    spheres[0].pos.x -= speed * dt;
    if(speed > 0.0f)
      speed -= 20.0f * dt;
    else 
      speed = 0.0f;
  } else if(currentShot == 3){
    reset_trail();
    spheres[1].rotationLeftRight = -90.0f;
    xEye = 220.0f;
    xCam = 200.0f;
  } else if(currentShot == 4){
    spheres[0].rotationLeftRight = 90.0f;
    xEye = 220.0f;
    xCam = 200.0f;
    spheres[1].pos.x -= dt * 4.5f;
  } else if(currentShot == 5){
    xEye = -220.0f;
    xCam = -200.0f;
    spheres[0].pos.x += dt * 4.5f;
  } else if(currentShot == 6){
    xEye = 0.0f;
    xCam = spheres[1].pos.x + 7.0f;
    yCam = 2.0f;
    zCam = -498.0f;
    spheres[1].pos.x -= dt * 6.5f;
    spheres[0].pos.x += dt * 6.5f;
  } else if(currentShot == 7){
    xCam = spheres[0].pos.x - 7.0f;
    spheres[1].pos.x -= dt * 9.5f;
    spheres[0].pos.x += dt * 9.5f;
  } else if(currentShot == 8){
    xEye = spheres[1].pos.x;
    xCam = spheres[1].pos.x - 9.0f;
    spheres[1].pos.x -= dt * 13.5f;
    spheres[0].pos.x += dt * 13.5f;
  } else if(currentShot == 9){
    xEye = spheres[0].pos.x;
    xCam = spheres[0].pos.x + 9.0f;
    spheres[1].pos.x -= dt * 16.5f;
    spheres[0].pos.x += dt * 16.5f;
  } else if(currentShot == 10){
    static char shot11init = 1;
    if(shot11init){
      xEye = 0.0f;
      xCam = 0.0f;
      yCam = 0.0f;
      zCam = -240.0f;
      shot11init = 0;
    }
    spheres[1].pos.x -= dt * 30.5f;
    spheres[0].pos.x += dt * 30.5f;
    zCam -= 70.0f * dt;
  } else if(currentShot == 11){
    zCam += 200.0f * dt;
  }

  draw_init(0.0f, nebIntense);
  if(currentShot != 11){
    draw_sphere(&spheres[0], 1, 200);
    draw_sphere(&spheres[1], 1, 200);
  } else {
    static char initExplosion = 1;
    if(initExplosion)
      draw_explosion(0.0f, 0.0f, -500.0f, 120.0f, dt, 1);
    else
      draw_explosion(0.0f, 0.0f, -500.0f, 120.0f, dt, 0);
    initExplosion = 0;
    nebIntense = 0.5f;
  }
}

static void scene14(void){
  static char initScene14 = 1;
  static int currentShot = 1;

  static float nextShotTime;
  float shotsTime[] = {
    12.0f, 10.0f, 10.0f, 6.0f, 10.0f, 10.0f, 5.0f, 10.0f, 10.0f, 4.0f, 10.0f, 12.0f, 10.0f, 20.0f, 30.0f
  };
  float shotsTime2[] = {
    12.0f, 10.0f, 10.0f, 6.0f, 10.0f, 10.0f, 5.0f, 10.0f, 10.0f, 6.0f, 10.0f, 12.0f, 14.0f, 20.0f, 30.0f
  };
  if(initScene14){
    glUniform1f(_isExplosion, 0.0f);
    sceneTimeBegin = gl4dGetElapsedTime();
    currentTime = 0.0f;
    t0 = 0.0f;
    t1 = 0.0f;
    reset_trail();
    xCam = 0.0f;
    yCam = 00.0f;
    // zCam = 150.0f;
    xEye = 00.0f;
    yEye = 0.0f;
    zEye = 0.0f;
    xUp = 0.0f;
    yUp = 1.0f;
    zUp = 0.0f;
    initScene14 = 0;
    nextShotTime = shotsTime[0];
  }

  if(t0 >= nextShotTime && currentShot <= 13){
    if(version == 1)
      nextShotTime = shotsTime[currentShot];
    else
      nextShotTime = shotsTime2[currentShot];
    currentShot++;
    t0 = 0.0f;
  }
  
  static float speedCam = 200.0f;
  float targetSpeed = 1.0f;
  float dec = 85.0f;

  zCam += speedCam * dt;

  if (speedCam > targetSpeed) {
      speedCam -= dec * dt;
      
      if (speedCam < targetSpeed) {
          speedCam = targetSpeed;
      }
  }

  if(version == 2 && currentShot == 1){
    currentShot = 7;
    nextShotTime = shotsTime2[7];
  }

  if(currentShot == 1){
    draw_init(0.0f, 0.5f);
    if(speedCam < 150.0f)
      drawStarsChoco(0.0f);
  } else if(currentShot == 2){
    static float alpha = 0.0f;
    draw_init(0.0f, 0.5f);
    drawStarsChoco(alpha);
    alpha += 0.8f * dt;
    if(alpha >= 1.0f){
      nextShotTime = shotsTime[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 3){
    static float alpha = 1.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit1(alpha);
    alpha -= 0.8f * dt;
    if(alpha <= 0.0f){
      nextShotTime = shotsTime[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 4){
    draw_init(0.0f, 0.5f);
    drawStarsCredit1(0.0f);
  } else if(currentShot == 5){
    static float alpha = 0.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit1(alpha);
    alpha += 1.4f * dt;
    if(alpha >= 1.0f){
      nextShotTime = shotsTime[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 6){
    static float alpha = 1.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit2(alpha);
    alpha -= 1.4f * dt;
    if(alpha <= 0.0f){
      if(version == 1)
        nextShotTime = shotsTime[currentShot];
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 7){
    draw_init(0.0f, 0.5f);
    drawStarsCredit2(0.0f);
  } else if(currentShot == 8){
    static float alpha = 0.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit2(alpha);
    alpha += 1.4f * dt;
    if(alpha >= 1.0f){
      if(version == 1)
        nextShotTime = shotsTime[currentShot];
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 9){
    static float alpha = 1.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit3(alpha);
    alpha -= 1.4f * dt;
    if(alpha <= 0.0f){
      if(version == 1)
        nextShotTime = shotsTime[currentShot];
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 10){
    draw_init(0.0f, 0.5f);
    drawStarsCredit3(0.0f);
  } else if(currentShot == 11){
    static float alpha = 0.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit3(alpha);
    alpha += 1.4f * dt;
    if(alpha >= 1.0f){
      if(version == 1)
        nextShotTime = shotsTime[currentShot];
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 12){
    static float alpha = 1.0f;
    draw_init(0.0f, 0.5f);
    drawStarsCredit4(alpha);
    alpha -= 1.4f * dt;
    if(alpha <= 0.0f){
      if(version == 1)
        nextShotTime = shotsTime[currentShot];
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  } else if(currentShot == 13){
    draw_init(0.0f, 0.5f);
    drawStarsCredit4(0.0f);
  } else if(currentShot == 14){
    static float alpha = 0.0f;
    static float intensity = 0.5f;
    draw_init(alpha, intensity);
    drawStarsCredit4(alpha);
    alpha += 0.2f * dt;
    intensity -= 0.2f * dt;
    if(intensity < 0.0f)
      intensity = 0.0f;
    if(alpha >= 1.0f){
      if(version == 1){
        nextShotTime = shotsTime[currentShot];
        quit();
      }
        
      else
        nextShotTime = shotsTime2[currentShot];
      currentShot++;
      t0 = 0.0f;
    }
  }
}

int getCharWeight(char c) {
    if (c == 'W' || c == 'N' || c == 'X' || c == 'K') 
        return 5;

    if (c == '0' || c == 'O') 
        return 4;
 
    if (c == 'k' || c == 'x' || c == 'd') 
        return 3;

    if (c == 'o' || c == 'l' || c == 'c') 
        return 2;

    if (c == ':' || c == ';' || c == '.' || c == '\'' || c == ',') 
        return 1;
        
    return 2;
}

static void initStarsChoco(void){
  short stars[NUM_STARS_CHOCO * 4];
  int count = 0;
  float aspectFix = 0.5f;

  for(int i = 0; i<CHOCO_H; ++i){
    for(int j = 0; j<CHOCO_W; ++j){
      char c = choco[i * CHOCO_W + j];

      if(c == ' ' || c == '\0') continue;

      stars[count * 4 + 0] = (short)(j * aspectFix * 100.0f);
      stars[count * 4 + 1] = (short)((CHOCO_H - i) * 100.0f);
      int weight = getCharWeight(c);
      // stars[count * 4 + 2] = (short)((rand() % 100));
      int thickness = 20; 
      stars[count * 4 + 2] = (short)(-1150 + (rand() % thickness));

      stars[count * 4 + 3] = (short)weight;
      count++;
    }
  }

  _numStarsChoco = count;

  glGenVertexArrays(1, &_chocoVao);
  glGenBuffers(1, &_chocoVbo);
  glBindVertexArray(_chocoVao);
  glBindBuffer(GL_ARRAY_BUFFER, _chocoVbo);
  
  glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(short), stars, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, 4 * sizeof(short), 0);
  
}

static void drawStarsChoco(float fade){
  static char initFunc = 1;
  static float zPos;
  if(initFunc){
    zPos = zCam - 300.0f * dt;
    initFunc = 0;
  }
  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 0.0f);
  glUniform1f(_isCredit, 0.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duBindMatrix("viewMat");
  gl4duTranslatef(0.0f, 0.0f, zPos);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pIdStars, "t"), gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glBindVertexArray(_chocoVao);
  glDrawArrays(GL_POINTS, 0, _numStarsChoco);

  glDisable(GL_BLEND);
}

static void initStarsCredit1(){
  short stars[NUM_STARS_CREDIT1 * 4];
  int count = 0;
  float aspectFix = 0.5f;

  for(int i = 0; i<CREDIT1_H; ++i){
    for(int j = 0; j<CREDIT1_W; ++j){
      char c = credit1[i * CREDIT1_W + j];

      if(c == ' ' || c == '\0') continue;

      // if((rand() % 100) > 90) continue;

      stars[count * 4 + 0] = (short)(j * aspectFix * 100.0f);
      stars[count * 4 + 1] = (short)((CREDIT1_H - i) * 100.0f);
      // stars[count * 4 + 2] = (short)((rand() % 100));
      int thickness = 50; 
      stars[count * 4 + 2] = (short)(-1150 + (rand() % thickness));

      // stars[count * 4 + 3] = (short)3;
      stars[count * 4 + 3] = (short)3;
      count++;
    }
  }

  _numStarsC1 = count;

  glGenVertexArrays(1, &_credit1Vao);
  glGenBuffers(1, &_credit1Vbo);
  glBindVertexArray(_credit1Vao);
  glBindBuffer(GL_ARRAY_BUFFER, _credit1Vbo);
  
  glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(short), stars, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, 4 * sizeof(short), 0);
}

static void drawStarsCredit1(float fade){
  static char init = 1;
  static float posZ;
  if(init){
    posZ = zCam - 100.0f;
    init = 0;
  }
  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 0.0f);
  glUniform1f(_isCredit, 1.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duBindMatrix("viewMat");
  // gl4duTranslatef(0.0f, 0.0f, 1000.0f);
  gl4duTranslatef(-6.0f, 10.0f, posZ);
  // gl4duTranslatef(0.0f, 0.0f, 600.0f);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pIdStars, "t"), gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glBindVertexArray(_credit1Vao);
  glDrawArrays(GL_POINTS, 0, _numStarsC1);

  glDisable(GL_BLEND);
}

static void initStarsCredit2(){
  short stars[NUM_STARS_CREDIT2 * 4];
  int count = 0;
  float aspectFix = 0.5f;

  for(int i = 0; i<CREDIT2_H; ++i){
    for(int j = 0; j<CREDIT2_W; ++j){
      char c = credit2[i * CREDIT2_W + j];

      if(c == ' ' || c == '\0') continue;

      // if((rand() % 100) > 90) continue;

      stars[count * 4 + 0] = (short)(j * aspectFix * 100.0f);
      stars[count * 4 + 1] = (short)((CREDIT2_H - i) * 100.0f);
      // stars[count * 4 + 2] = (short)((rand() % 100));
      int thickness = 60; 
      stars[count * 4 + 2] = (short)(-1150 + (rand() % thickness));

      stars[count * 4 + 3] = (short)3;
      count++;
    }
  }

  _numStarsC2 = count;

  glGenVertexArrays(1, &_credit2Vao);
  glGenBuffers(1, &_credit2Vbo);
  glBindVertexArray(_credit2Vao);
  glBindBuffer(GL_ARRAY_BUFFER, _credit2Vbo);
  
  glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(short), stars, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, 4 * sizeof(short), 0);
}

static void drawStarsCredit2(float fade){
  static char init = 1;
  static float posZ;
  if(init){
    if(version == 1)
      posZ = zCam - 130.0f;
    else
      posZ = zCam + 130.0f;
    init = 0;
  }

  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 0.0f);
  glUniform1f(_isCredit, 1.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duBindMatrix("viewMat");
  // gl4duTranslatef(0.0f, 0.0f, 1000.0f);
  gl4duTranslatef(-16.0f, -5.0f, posZ);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pIdStars, "t"), gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glBindVertexArray(_credit2Vao);
  glDrawArrays(GL_POINTS, 0, _numStarsC2);

  glDisable(GL_BLEND);
}

static void initStarsCredit3(){
  short stars[NUM_STARS_CREDIT3 * 4];
  int count = 0;
  float aspectFix = 0.5f;

  for(int i = 0; i<CREDIT3_H; ++i){
    for(int j = 0; j<CREDIT3_W; ++j){
      char c = credit3[i * CREDIT3_W + j];

      if(c == ' ' || c == '\0') continue;

      // if((rand() % 100) > 90) continue;

      stars[count * 4 + 0] = (short)(j * aspectFix * 100.0f);
      stars[count * 4 + 1] = (short)((CREDIT3_H - i) * 100.0f);
      // stars[count * 4 + 2] = (short)((rand() % 100));
      int thickness = 40; 
      stars[count * 4 + 2] = (short)(-1150 + (rand() % thickness));

      stars[count * 4 + 3] = (short)3;
      count++;
    }
  }

  _numStarsC3 = count;

  glGenVertexArrays(1, &_credit3Vao);
  glGenBuffers(1, &_credit3Vbo);
  glBindVertexArray(_credit3Vao);
  glBindBuffer(GL_ARRAY_BUFFER, _credit3Vbo);
  
  glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(short), stars, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, 4 * sizeof(short), 0);
}

static void drawStarsCredit3(float fade){
  static char init = 1;
  static float posZ;
  if(init){
    posZ = zCam - 140.0f;
    init = 0;
  }
  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 0.0f);
  glUniform1f(_isCredit, 1.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duBindMatrix("viewMat");
  gl4duTranslatef(-40.0f, -5.0f, posZ);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pIdStars, "t"), gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glBindVertexArray(_credit3Vao);
  glDrawArrays(GL_POINTS, 0, _numStarsC3);

  glDisable(GL_BLEND);
}

static void initStarsCredit4(){
  short stars[NUM_STARS_CREDIT4 * 4];
  int count = 0;
  float aspectFix = 0.5f;

  for(int i = 0; i<CREDIT4_H; ++i){
    for(int j = 0; j<CREDIT4_W; ++j){
      char c = credit4[i * CREDIT4_W + j];

      if(c == ' ' || c == '\0') continue;

      // if((rand() % 100) > 90) continue;

      stars[count * 4 + 0] = (short)(j * aspectFix * 100.0f);
      stars[count * 4 + 1] = (short)((CREDIT4_H - i) * 100.0f);
      // stars[count * 4 + 2] = (short)((rand() % 100));
      int thickness = 50; 
      stars[count * 4 + 2] = (short)(-1150 + (rand() % thickness));

      // stars[count * 4 + 3] = (short)3;
      stars[count * 4 + 3] = (short)3;
      count++;
    }
  }

  _numStarsC4 = count;

  glGenVertexArrays(1, &_credit4Vao);
  glGenBuffers(1, &_credit4Vbo);
  glBindVertexArray(_credit4Vao);
  glBindBuffer(GL_ARRAY_BUFFER, _credit4Vbo);
  
  glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(short), stars, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_SHORT, GL_FALSE, 4 * sizeof(short), 0);
}

static void drawStarsCredit4(float fade){
  static char init = 1;
  static float posZ;
  if(init){
    posZ = zCam - 130.0f;
    init = 0;
  }
  glUseProgram(_pIdStars);
  glUniform1f(_isBack, 0.0f);
  glUniform1f(_isCredit, 1.0f);
  glUniform1f(_fade, fade);
  gl4duBindMatrix("projMat"); 
  gl4duBindMatrix("viewMat");
  gl4duTranslatef(-15.0f, 5.0f, posZ);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pIdStars, "t"), gl4dGetElapsedTime()/1000.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glBindVertexArray(_credit4Vao);
  glDrawArrays(GL_POINTS, 0, _numStarsC4);

  glDisable(GL_BLEND);
}

static void draw_clouds(float intensity){
  glUseProgram(_pIdClouds);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  gl4duBindMatrix("projMat");
  gl4duBindMatrix("viewMat");
  gl4duTranslatef(xCam, yCam, zCam);
  // gl4duScalef(100.0f, 100.0f, 100.0f);
  gl4duScalef(1000.0f, 1000.0f, 1000.0f);
  gl4duSendMatrices();

  glUniform1f(_colorIntensity, intensity);
  glUniform1f(_timeClouds, gl4dGetElapsedTime() / 1000.0f);

  gl4dgDraw(_sphere);
  glEnable(GL_DEPTH_TEST);
  
}
