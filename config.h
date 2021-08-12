#include <X11/XF86keysym.h>
#include "tatami.c"
/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  = 2;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const int focusonwheel       = 0;
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char *fonts[]          = { "Cascadia Code:size=15" };
static char dmenufont[]       = "Cascadia Code:size=15";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#1E1E3F";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#be3ede";
static char selbgcolor[]            = "#5E45A5";
static char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
    [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5" };

static const Rule rules[] = {

    /* class      	     instance    title    tags mask  switchtotag    isfloating   CenterThisWindow?     monitor */
	{ "st",              NULL,       NULL,    1,         1,             0,     	     1,	    	           -1 },
	{ "discord",         NULL,       NULL,    0,         1,             0,     	     1,	    	           -1 },
	{ "firefox",         NULL,       NULL,    1 << 1,    1,             0,     	     0,	    	           -1 },


};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "T",      tile },    /* first entry is default */
    { "F",      NULL },    /* no layout function means floating behavior */
    { "M",      monocle },
    { "|+|",    tatami },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

//Terminal commands 
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *screenshot[] = {"screenshot", NULL};
static const char *pavu[] = {"pavucontrol", NULL };
static const char *firefox[] = {"firefox", NULL };

//Media keys
static const char *inc_vol[]			= { "volume", "-i", NULL };
static const char *dec_vol[]			= { "volume", "-d", NULL };
static const char *mute_vol[]			= { "volume", "-m", NULL };
/* static const char *inc_brightness[]		= { "brightness",   "-i", NULL }; */
/* static const char *dec_brightness[]		= { "brightness",   "-d", NULL }; */
static const char *print_screen_sel[]   = { "print_screen", "-s", NULL };

//menu commands
static const char *power_menu[]   	        = { "power_menu", NULL };
static const char *unicode_menu[]		= { "unicode_menu", NULL };
static const char *words_menu[]			= { "words_menu", NULL };
static const char *mount_menu[]			= { "mount_menu", NULL };
static const char *umount_menu[]		= { "umount_menu", NULL };
static const char *pass_menu[]			= { "pass_menu", NULL };

/* mpc Commands */
static const char *mpc_toggle[]			= { "mpc", "toggle", NULL };
static const char *mpc_stop[]			= { "mpc", "stop", NULL };
static const char *mpc_next[]			= { "mpc", "next", NULL };
static const char *mpc_prev[]			= { "mpc", "prev", NULL };


/*
 * Xresources preferences to load at startup
 */

ResourcePref resources[] = {
    { "fonts",              ARR_STRING,  &fonts },
    { "dmenufont",          STRING,		&dmenufont },
    { "normbgcolor",        STRING,		&normbgcolor },
    { "normbordercolor",    STRING,		&normbordercolor },
    { "normfgcolor",        STRING,		&normfgcolor },
    { "selbgcolor",         STRING,		&selbgcolor },
    { "selbordercolor",     STRING,		&selbordercolor },
    { "selfgcolor",         STRING,		&selfgcolor },
    { "borderpx",          	INTEGER,	&borderpx },
    { "snap",          		INTEGER,	&snap },
    { "showbar",          	INTEGER,	&showbar },
    { "topbar",          	INTEGER,	&topbar },
    { "nmaster",          	INTEGER,	&nmaster },
    { "resizehints",       	INTEGER,	&resizehints },
    { "mfact",      	 	FLOAT,		&mfact },
};

static Key keys[] = {
    /* modifier                     key        function        argument */
    
    { MODKEY,                       XK_y,      setlayout,      {.v = &layouts[3]} },
    //Rofi App Launch
    { MODKEY,                       XK_space,      spawn,          {.v = dmenucmd } },
    //Terminal Launch
	{ MODKEY,						XK_Return, spawn,          {.v = termcmd } },
    //Toggle top dwm bar
	{ MODKEY,                       XK_b,      togglebar,      {0} },
    //Focus cycle
    { Mod1Mask,                     XK_Tab,    focusstack,              {.i = +1 } },
    { Mod1Mask|ShiftMask,           XK_Tab,    focusstack,              {.i = -1 } },
    //Screenshot area
    { Mod1Mask|ShiftMask,           XK_s,      spawn,            {.v = screenshot } },
    //Cycle workspaces
	{ MODKEY,                       XK_Tab,       shiftview,     { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_Tab,       shiftview,     { .i = -1 } },
    //Change number of windows in master
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = +1 } }, 
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } }, 
    //Change window size
    { MODKEY,                       XK_Left,   setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_Right,  setmfact,       {.f = +0.05} },
    //Open pavucontrol
    { MODKEY,                       XK_v,      spawn, {.v = pavu } },
    /* Added by vanitygaps patch 
    //{ MODKEY|Mod1Mask,              XK_h,      incrgaps,       {.i = +1 } },
    //{ MODKEY|Mod1Mask,              XK_l,      incrgaps,       {.i = -1 } },
    //{ MODKEY|Mod1Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
    //{ MODKEY|Mod1Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
    //{ MODKEY|Mod1Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
    //{ MODKEY|Mod1Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
    //{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },
    //{ MODKEY|Mod1Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
    //{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
    //{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
    //{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
    //{ MODKEY|ControlAMask,           XK_o,      incrivgaps,     {.i = -1 } },
    //{ MODKEY|Mod1Mask,              XK_y,      incrohgaps,     {.i = +1 } },
    //{ MODKEY|Mod1Mask,              XK_o,      incrohgaps,     {.i = -1 } },
    //{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
    //{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
    End of shortcuts added by vanitygaps patch */

    { Mod1Mask,				        XK_Return, zoom,           {0} },
    { Mod3Mask, XK_w, spawn, {.v = firefox} },
    //Kill App
    { MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
    //Tabbed
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
    //   { MODKEY,                       XK_space,  setlayout,      {0} },
    //   { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
    { MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
    //Kill DWM
	{ MODKEY|ShiftMask,				XK_c,	   quit,		   {0} },

	/* Menu Bindings */
    { MODKEY|ShiftMask,             XK_p,                       spawn,  {.v = pass_menu} },
	{ MODKEY|ShiftMask,             XK_u,                       spawn,  {.v = umount_menu} },
	{ MODKEY|ShiftMask,             XK_m,                       spawn,  {.v = mount_menu} },
	{ MODKEY,                       XK_x,                       spawn,  {.v = power_menu} },
    { MODKEY,                       XK_u,                       spawn,  {.v = unicode_menu} },
    { MODKEY,                       XK_d,                       spawn,  {.v = words_menu} },
   
	/* mpc Bindings */
	{ Mod1Mask,						XK_p,						spawn,	{.v = mpc_prev} },
	{ Mod1Mask,						XK_n,						spawn,	{.v = mpc_next} },
	{ Mod1Mask,						XK_space,					spawn,	{.v = mpc_toggle} },
	{ Mod1Mask,						XK_s,						spawn,	{.v = mpc_stop} },

	/* XF86 Bindings */
	{ 0,                            XF86XK_AudioMute,           spawn,  {.v = mute_vol } },
    { 0,                            XF86XK_AudioLowerVolume,    spawn,  {.v = dec_vol } },
    { 0,                            XF86XK_AudioRaiseVolume,    spawn,  {.v = inc_vol } },
	/* { 0,                         XF86XK_MonBrightnessDown,   spawn,  {.v = dec_brightness } }, */	
    /* { 0,                         XF86XK_MonBrightnessUp,     spawn,  {.v = inc_brightness } }, */
	{ 0,                            XK_Print,                   spawn,  {.v = print_screen_sel } },

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
