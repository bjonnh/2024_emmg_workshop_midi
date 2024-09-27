// This is only a knob for the prototypes

pot_height=6;
pot_diameter=6.25;
pot_cut_depth=1.5;
stem_diameter=8;
stem_height=8;

head_height=8;
head_diameter=14;
head_fn=8;
$fn=100;
module pot() {
    difference() {
        cylinder(pot_height, pot_diameter/2, pot_diameter/2);
        translate([pot_diameter/2-pot_cut_depth, -pot_diameter/2, -0.1]) cube([pot_diameter, pot_diameter,pot_height+2*0.1]);
    }
}

module stem() {
    difference() {
        cylinder(stem_height, stem_diameter/2,stem_diameter/2);
        translate([0,0,stem_height-pot_height+0.1]) pot();
    }
}

module design_head() {
    translate([0, -0.5, -0.1]) cube([head_diameter/2, 1, 0.5]);
}

module head() {
    difference() { 
    union() {
        cylinder(head_height, head_diameter/2, head_diameter/2, $fn=head_fn);
        rotate([0,0,20]) cylinder(head_height, head_diameter/2, head_diameter/2, $fn=head_fn);
    }
    design_head();
}
}

module full() {
    translate([0,0,head_height]) stem();
    head();
}

full();
