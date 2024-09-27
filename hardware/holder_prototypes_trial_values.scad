spherulation = 0.8;

width=97;
width_spheruled = width + 2*spherulation;
height=120.54;
margins = 5.5;
cut_in_margins = 3;
lip_depth=1;
board_depth=1.54;
cut_margin=0.1;
depth_holder = 5*board_depth;

module board(margins) {
    color("red") translate([2*margins-2.5,0,2.5*board_depth-spherulation]) cube([width,height, board_depth]);
}


module prismatic_cut(margins) {
    translate([0,height+margins-0.1,0]) rotate([90,0,0]) linear_extrude(height+margins+10) {
        polygon(points=[
        [0,0], 
        [width_spheruled+spherulation+1,0],
        [width_spheruled+spherulation-1,3*board_depth+2*spherulation],
        [3, 3*board_depth+2*spherulation],
        [0,0]]);
    }
}

module holder(margins) {
    difference() {
     cube([width_spheruled+3*margins, height+2*margins, depth_holder]);
       translate([margins,0,2*board_depth-spherulation+cut_margin]) prismatic_cut(margins);
        // for solder joints
        translate([margins+1+5, -0.1, 1*board_depth-1.5*spherulation]) cube([width_spheruled-10,height+1*margins, 2*spherulation+board_depth*2]);
     
    }
}


//board();
module rounded(margins) {
    minkowski() {
      holder(margins);
      sphere(spherulation, $fn=25);
    }
}


module test() {
    for (margins=[2:0.5:5]) {
           rotate([90,0,0]) translate([0,margins*30,0]) {
               difference() { 
                   rounded(margins);
                   translate([50,0,0]) linear_extrude(1.2) text(str(margins), size=3);
                   translate([-10,3,-10]) cube([1000,1000,100]);
               }
           }
        }
}

rounded(margins);
//test();
//prismatic_cut();