// cylinders to hold the screens on the EMMG midi workshop device v2
// units are mm

$fn=50;

height = 11.24;
external_diameter = 2.6;
internal_diameter = 2.3/2;
screw_hole_diameter = 1.25;
margins = 0.1;
board_height = 1.54*2; // we want the nut as well so we can crunch it


module main_body() {
    cylinder(height, external_diameter, external_diameter, $fn=6);
}
module peg() {
    translate([0,0,height])
    cylinder(board_height, internal_diameter, internal_diameter);
}

module hole() {
    translate([0,0,-margins]) cylinder(height-3*margins, screw_hole_diameter, screw_hole_diameter);
}

difference() {
    union() {
        main_body();
        peg();
    }
    hole();
}
