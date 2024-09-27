// This is only a knob for the prototypes

pot_height=5;
pot_diameter=6;
pot_cut_depth=1.5;
stem_diameter=8;
stem_height=7;

head_height=8;
head_diameter=14;
head_fn=8;

cross_length = pot_diameter/1.75;
cross_thickness = 0.5;
$fn=100;
module pot() {
    difference() {
        cylinder(pot_height, pot_diameter/2.1, pot_diameter/1.9);
        translate([-cross_thickness/2, -cross_length/2, -0.1]) cube([cross_thickness, cross_length,pot_height/2+2*0.1]);
        rotate([0,0,90])translate([-cross_thickness/2, -cross_length/2, -0.1]) cube([cross_thickness, cross_length,pot_height/2+2*0.1]);
    }
}

module stem() {
    difference() {
        cylinder(stem_height, stem_diameter/2,stem_diameter/2);
        rotate([0,0,45]) translate([0,0,stem_height-pot_height+0.1]) pot();
    }
}

module design_head() {
    translate([0, -0.5, -0.1]) cube([head_diameter/2, 1, 0.5]);
}

module head() {
    difference() { 
    union() {
        cylinder(head_height, head_diameter/3, head_diameter/2, $fn=head_fn);
        rotate([0,0,20]) cylinder(head_height, head_diameter/2.8, head_diameter/2, $fn=head_fn);
        rotate([0,0,30]) cylinder(head_height, head_diameter/2.8, head_diameter/2.2, $fn=head_fn);
    }
    design_head();
}
}

module full() {
    translate([0,0,head_height]) stem();
    head();
}

full();
