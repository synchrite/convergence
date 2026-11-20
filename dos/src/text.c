/* All the words: control descriptions, repair jobs, the handbook, test pattern notes. */
#include "g.h"

const char *grpname[NGRP] = { "GEOMETRY", "PICTURE", "PURITY + STATIC", "CONVERGENCE STRIPS" };
const char *patname[NPAT] = { "F1 CROSSHATCH", "F2 DOTS", "F3 RED FIELD", "F4 WHITE FIELD", "F5 PLUGE" };
const char *pathelp[NPAT] = {
    "Crosshatch: geometry, focus and convergence. Fringes on white lines mean misconvergence.",
    "Dots: convergence. Each dot should be one white dot, not three coloured ones.",
    "Red field: purity. The red must be the same red everywhere, with no other tint.",
    "White field: purity and colour patches. Magnetised spots show as coloured clouds.",
    "PLUGE: black level. The left bar must vanish into the black, the next must just show."
};

#define GEOPAT "Use the crosshatch (F1)."
const Ctl ctl[NCTL] = {
 { "H-SIZE", G_GEO, -20, 20, 0, "Width coil or pot",
   { "Sets the width of the picture. On older sets it is a slug-tuned coil in",
     "series with the horizontal yoke, on later ones a pot or a menu setting.",
     "Adjust until the grid just fills the screen with equal margins left and",
     "right. " GEOPAT " Set centring first, size second." },
   "The grid is too wide or too narrow. Adjust H-SIZE." },
 { "V-SIZE", G_GEO, -20, 20, 0, "Height pot",
   { "Sets the height of the picture by changing the vertical sweep amplitude.",
     "Adjust until the top and bottom lines sit just inside the bezel and the",
     "circle is round, not an egg. " GEOPAT,
     "If the circle is round but too small, both sizes are low." },
   "The grid is too tall or too short and the circle is not round. Adjust V-SIZE." },
 { "H-CENTRE", G_GEO, -20, 20, 0, "Horizontal position",
   { "Moves the picture left and right. It shifts the DC current through the",
     "yoke, or the video timing relative to sync. Centre the grid so the",
     "margins at the left and right edges are equal. " GEOPAT, "" },
   "The picture sits to one side. Adjust H-CENTRE." },
 { "V-CENTRE", G_GEO, -20, 20, 0, "Vertical position",
   { "Moves the picture up and down. Centre the grid so the top and bottom",
     "margins are equal. Always centre before you set the size, or you will",
     "set the size against the wrong edge. " GEOPAT, "" },
   "The picture sits too high or too low. Adjust V-CENTRE." },
 { "TILT", G_GEO, -20, 20, 0, "Yoke rotation",
   { "The whole picture is rotated. The deflection yoke has turned on the neck",
     "of the tube. A technician loosens the yoke clamp and turns the yoke a",
     "fraction; newer monitors have a rotation coil and a menu setting.",
     "Level the horizontal lines against the bezel. " GEOPAT },
   "Horizontal lines slope: the picture is rotated. Adjust TILT." },
 { "PINCUSHION", G_GEO, -20, 20, 0, "Side pincushion (E-W)",
   { "The sides bow inward or outward. A flat-ish screen is further from the",
     "gun at the corners, so the beam travels further there. The east-west",
     "correction circuit bends the horizontal scan to compensate.",
     "Adjust until the outer vertical lines are straight. " GEOPAT },
   "The left and right edges are curved. Adjust PINCUSHION." },
 { "TRAPEZOID", G_GEO, -20, 20, 0, "Keystone",
   { "The picture is wider at the top than the bottom, or the reverse.",
     "Adjust until the left and right edges are parallel. If they are parallel",
     "but both curved, that is pincushion, not trapezoid. " GEOPAT, "" },
   "Top and bottom have different widths. Adjust TRAPEZOID." },
 { "FOCUS", G_PIC, -20, 20, 0, "Focus pot on the flyback",
   { "Sets the voltage on the focus electrode of the gun, a few kilovolts,",
     "from a pot on the flyback transformer. Adjust for the thinnest, sharpest",
     "lines. On a real set, judge focus half way between centre and corner, at",
     "normal brightness: too much brightness makes the spot bloom." },
   "The lines are thick and soft. Adjust FOCUS until they are thinnest." },
 { "BRIGHTNESS", G_PIC, -12, 12, 0, "Black level (screen, G2)",
   { "Sets the black level. Use the PLUGE pattern (F5). The first bar on the",
     "left is blacker than black: it must disappear into the background. The",
     "second bar is just above black: it must stay barely visible. Too high",
     "and blacks turn grey; too low and shadow detail is lost." },
   "Black is not black, or shadows are crushed. Use PLUGE (F5) and adjust BRIGHTNESS." },
 { "PURITY ROT", G_MAG, 0, 23, 1, "Purity ring pair: direction",
   { "The purity magnet is a pair of two-pole rings at the back of the neck.",
     "It moves all three beams together so each lands only on its own colour",
     "of phosphor. Turning both rings together changes the direction of the",
     "shift. Use the red field (F3): the tint must leave the screen." },
   "The red field has a tinted side. Turn PURITY ROT until the tint is weakest, then set SPREAD." },
 { "PURITY SPREAD", G_MAG, 0, 10, 0, "Purity ring pair: strength",
   { "Opening the two tabs of the purity ring apart makes its field stronger;",
     "tabs together cancel. Degauss first: a magnetised shadow mask looks",
     "like a purity fault but no ring will cure it. Then set rotation and",
     "spread for a pure, even red over the whole screen (F3)." },
   "The red field is still tinted. Change PURITY SPREAD." },
 { "4-POLE ROT", G_MAG, 0, 23, 1, "Four-pole rings: direction",
   { "Static convergence, step 1. The four-pole magnet moves the red and",
     "blue beams in OPPOSITE directions and leaves green alone. Use it to",
     "bring red and blue onto each other at the centre of the screen.",
     "Watch the centre of the crosshatch (F1) or the dots (F2)." },
   "Red and blue are split on opposite sides of green at the centre. Use the 4-POLE rings." },
 { "4-POLE SPREAD", G_MAG, 0, 10, 0, "Four-pole rings: strength",
   { "The angle between the two tabs sets how far red and blue move apart or",
     "together. Work on the centre of the screen only: static convergence",
     "cannot fix the corners, and chasing a corner will ruin the centre.", "" },
   "Red and blue do not overlap at the centre. Change 4-POLE SPREAD." },
 { "6-POLE ROT", G_MAG, 0, 23, 1, "Six-pole rings: direction",
   { "Static convergence, step 2. The six-pole magnet moves red and blue",
     "TOGETHER in the same direction, again leaving green alone. Once red and",
     "blue overlap (they look magenta), use it to lay that magenta line onto",
     "green. The result is white at the centre." },
   "Red and blue overlap as magenta but sit beside green. Use the 6-POLE rings." },
 { "6-POLE SPREAD", G_MAG, 0, 10, 0, "Six-pole rings: strength",
   { "Sets how far the red-blue pair moves. The rings interact a little, so",
     "go round again: purity, four-pole, six-pole, and repeat. When the",
     "centre is white, seal the rings with paint so they cannot turn.", "" },
   "Magenta and green lines do not overlap at the centre. Change 6-POLE SPREAD." },
#define STRIP(n, pos) { n, G_STRIP, -3, 3, 0, "Convergence strip, " pos, \
   { "Dynamic convergence. When the centre is white but an edge or corner", \
     "still shows red and blue fringes, small magnetic strips are slid under", \
     "the yoke, against the glass, at the clock position nearest the error.", \
     "Left/Right adds strips of either polarity here. Fewer is better." }, \
   "Fringes remain near the " pos " edge. Add or remove strips there." }
 STRIP("STRIP 12:00", "12 o'clock"), STRIP("STRIP 1:30", "1:30"), STRIP("STRIP 3:00", "3 o'clock"), STRIP("STRIP 4:30", "4:30"),
 STRIP("STRIP 6:00", "6 o'clock"), STRIP("STRIP 7:30", "7:30"), STRIP("STRIP 9:00", "9 o'clock"), STRIP("STRIP 10:30", "10:30")
};

const Job jobs[NJOB] = {
 { "PORTABLE 12 INCH", { "\"The picture is squeezed over to one side.\"", "A simple one to start. Centre it, then set the width.", "" }, F_HS | F_HC, 60, PAT_HATCH },
 { "OFFICE 14 INCH", { "\"Everything leans, and the picture is short and low.\"", "The yoke has slipped round on the neck.", "" }, F_VS | F_VC | F_TILT, 90, PAT_HATCH },
 { "DRAUGHTSMAN'S 17 INCH", { "\"Straight lines bow at the sides. My drawings look wrong.\"", "Pincushion and keystone. Get the edges straight and parallel.", "" }, F_PIN | F_TRAP | F_HS, 110, PAT_HATCH },
 { "GAMER'S 15 INCH", { "\"Text is fuzzy and dark scenes are grey.\"", "Focus on the crosshatch, black level on the PLUGE pattern (F5).", "" }, F_FOCUS | F_BRIGHT, 90, PAT_HATCH },
 { "THE HI-FI SPEAKER", { "\"There is a purple cloud in one corner since I moved my speakers.\"", "The shadow mask is magnetised. Check a field pattern (F3, F4).", "Press D to degauss." }, F_MAGNET, 60, PAT_WHITE },
 { "MOVED HOUSE", { "\"The red is not red on one side, even after degaussing.\"", "Degauss anyway, then set the purity rings on the red field (F3).", "" }, F_MAGNET | F_PUR, 150, PAT_RED },
 { "COLOUR GHOSTS", { "\"White letters have a red shadow one side and blue the other.\"", "Static convergence: the four-pole rings.", "" }, F_P4, 150, PAT_HATCH },
 { "MORE GHOSTS", { "\"Now there are three of everything.\"", "Four-pole first to make magenta, then six-pole onto green.", "" }, F_P4 | F_P6, 240, PAT_DOTS },
 { "CORNER FRINGES", { "\"The middle is perfect but the corners have fringes.\"", "Dynamic convergence: place strips under the yoke.", "" }, F_STRIP2, 240, PAT_HATCH },
 { "THE WORKS", { "\"It fell off the van.\"", "Full alignment, in the right order: degauss, geometry, focus,", "black level, purity, static convergence, then the strips." },
   F_HS | F_VC | F_TILT | F_PIN | F_FOCUS | F_BRIGHT | F_MAGNET | F_PUR | F_P4 | F_P6 | F_STRIP2, 600, PAT_HATCH }
};

const char *handbook[] = {
 "SAFETY FIRST\n\n"
 "A CRT monitor is the most dangerous thing most people ever open.\n\n"
 " * The anode runs at 15,000 to 30,000 volts. The tube is a capacitor and\n"
 "   holds that charge for days after the set is unplugged.\n"
 " * The chassis of many sets is connected to the mains. Service work needs\n"
 "   an isolation transformer.\n"
 " * The neck of the tube is thin glass under vacuum. A knock can break it.\n"
 " * The flyback area produces X-rays if the high voltage is set too high.\n\n"
 "Everything in this program that you do to a REAL monitor is done from the\n"
 "outside: the front controls, the on-screen menu, the degauss button, and\n"
 "the test patterns on the screen.\n\n"
 "The rings, the yoke and the strips are inside the case. The Service Bench\n"
 "lets you learn what they do on a simulated set. Leave the real ones to a\n"
 "trained technician, or get proper training before you open a case.",

 "THE ORDER OF ADJUSTMENT\n\n"
 "Each adjustment disturbs the ones after it, so the order matters.\n\n"
 "  1. Warm up for 20 minutes. A cold tube drifts.\n"
 "  2. Degauss. A magnetised mask fakes purity and convergence faults.\n"
 "  3. Geometry: centre, then size, then tilt, pincushion, trapezoid.\n"
 "  4. Focus, at normal brightness.\n"
 "  5. Black level (brightness) with a PLUGE pattern, then contrast.\n"
 "  6. Purity, on a red field.\n"
 "  7. Static convergence at the centre: four-pole, then six-pole.\n"
 "  8. Repeat 6 and 7. The rings interact.\n"
 "  9. Dynamic convergence at the edges: yoke tilt, wedges, strips.\n\n"
 "The Service Bench scores the result, not the order, but working in this\n"
 "order is faster, and on the final job it is the only way to make par.",

 "PURITY AND CONVERGENCE\n\n"
 "A colour tube has three guns in a row: blue, green, red. Behind the glass\n"
 "is a mask of slots, and behind each slot three phosphor stripes.\n\n"
 "PURITY means each beam lands only on its own colour. All three beams are\n"
 "moved together by the two-pole purity rings until a red field is red.\n\n"
 "CONVERGENCE means the three beams pass through the same slot.\n"
 "  Four-pole rings move red and blue in opposite directions.\n"
 "  Six-pole rings move red and blue together.\n"
 "  Green is the reference. Nothing moves green.\n"
 "Turning a ring pair together changes direction. Spreading its two tabs\n"
 "changes strength.\n\n"
 "The rings act the same everywhere on the screen, so they set the centre.\n"
 "What is left at the edges is DYNAMIC error. It is cured by tilting the\n"
 "yoke, pushing rubber wedges between yoke and glass, and finally by\n"
 "sticking small magnetic strips to the glass under the yoke.",

 "USING THE TEST PATTERNS ON YOUR OWN MONITOR\n\n"
 "Choose TEST PATTERNS from the main menu. Use the resolution you work in,\n"
 "because a multisync monitor stores its geometry separately for each mode.\n\n"
 "  Crosshatch   size, centring, tilt, pincushion, convergence\n"
 "  Dots         convergence (look for coloured edges on the dots)\n"
 "  Red, green, blue, white fields   purity, and the need to degauss\n"
 "  Colour bars  a quick check of all three guns\n"
 "  PLUGE        black level (the brightness control)\n"
 "  Focus        sharpness at centre and corners, moire\n"
 "  Geometry     circles and edge frame for size and linearity\n"
 "  Regulation   a flashing block: the picture must not breathe in and out\n\n"
 "R, G and B switch each gun off and on, so you can look at one colour, or\n"
 "at red and blue alone as magenta, the way a technician does.\n"
 "F1 shows what to look for. Press it again to clear the screen.",

 "ABOUT\n\n"
 "CHARGE! CONVERGENCE, DOS edition.\n\n"
 "This game descends from CHARGE!, written by C. Anthony Ray of Urbana,\n"
 "Illinois, and published in BYTE magazine in December 1982 as the second\n"
 "place winner of the BYTE Game Contest. In his game you steered an electron\n"
 "past charged ions to land it on a target. Here there are three beams, the\n"
 "ions have become magnets, and the target is a white dot.\n\n"
 "Runs on a 486 or better with a VESA SVGA card (640x480, 256 colours).\n"
 "Sound: AdLib or Sound Blaster. No coprocessor is needed.\n\n"
 "Switches:  /BANK     use bank switching, not the linear frame buffer\n"
 "           /NOSOUND  no sound at all\n"
 "           /NOSB     no Sound Blaster    /NOADLIB  no FM music\n\n"
 "The simulated monitor is a teaching model. Real sets differ in where the\n"
 "controls are and how they interact. Use the service manual for yours."
};
const int handbook_pages = sizeof handbook / sizeof handbook[0];

const char *tp_name[] = { "CROSSHATCH", "DOTS", "RED FIELD", "GREEN FIELD", "BLUE FIELD", "WHITE FIELD", "BLACK",
                          "COLOUR BARS", "PLUGE AND GREY STEPS", "FOCUS", "GEOMETRY", "REGULATION" };
const int tp_count = 12;
const char *tp_help[][6] = {
 { "Geometry: the outer lines should sit just inside the bezel, straight and",
   "parallel to it. Use the monitor's size, position, pincushion, trapezoid and",
   "rotation controls. Squares must be square: measure one with a ruler.",
   "Convergence: every line should be white. Red or blue edges mean the beams",
   "are apart there. Press G to switch green off and look at magenta alone.", "" },
 { "Each dot should be a single white dot. A dot with a red edge on one side",
   "and blue on the other is a convergence error at that place. Errors at the",
   "centre are static convergence; errors only at the edges are dynamic.",
   "Dots also show focus: they should be small and round, not smeared.", "", "" },
 { "Purity. The red should be the same everywhere. Patches of another tint mean",
   "the red beam is striking other phosphors: degauss the monitor (many have a",
   "button or menu item; otherwise switch off for 20 minutes and on again).",
   "Keep loudspeakers and magnets away. If it stays, it needs a technician.", "", "" },
 { "Green purity. Green is the reference gun, so a tint here usually means",
   "magnetisation. Degauss. Also look for dark spots: dirt or mask damage.", "", "", "", "" },
 { "Blue purity. Blue is the weakest phosphor, so this field looks dim. It",
   "should still be even. Compare the four corners with the centre.", "", "", "", "" },
 { "White uniformity. Any coloured cloud is a purity or magnetisation fault.",
   "A slow change in brightness from centre to edge is normal for a CRT.",
   "A yellow or blue cast over the whole screen is the grey-scale (white",
   "balance) setting, not purity.", "", "" },
 { "Black. With the room dark the screen should be black, with no glow and no",
   "diagonal retrace lines. If you see them, the brightness or the screen (G2)",
   "setting is too high.", "", "", "" },
 { "All three guns at once. From the left: white, yellow, cyan, green, magenta,",
   "red, blue, black. Press R, G, B to switch guns off: with only blue on,",
   "alternate bars must be equally bright. The ramp below must run smoothly",
   "from black to white with no colour tint anywhere along it.", "", "" },
 { "Black level. Turn contrast up, then set BRIGHTNESS so the first bar on the",
   "left vanishes into the background and the second bar is only just visible.",
   "Then set CONTRAST so the brightest grey step is bright but does not bloom",
   "or blur, and all the steps can still be told apart.", "", "" },
 { "Sharpness. The fine lines and letters should be crisp at the centre and",
   "acceptable in the corners. If your monitor has a focus menu, set it here at",
   "your normal brightness. Wavy coloured bands over the fine lines are moire:",
   "they mean the focus is good. A moire control can soften them.", "", "" },
 { "Size and linearity. The frame marks the edge of the picture: set size and",
   "position so it is just visible all round. The big circle must be round and",
   "the four small ones the same size as each other; if not, linearity is off.",
   "The diagonals must cross at the centre mark.", "", "" },
 { "High voltage regulation. The block flashes between black and white. Watch",
   "the frame: it should not move. If the picture grows when the block is",
   "white (breathing), the high voltage supply is sagging under load.",
   "Reduce contrast; if it is severe the monitor needs service.", "", "" }
};
