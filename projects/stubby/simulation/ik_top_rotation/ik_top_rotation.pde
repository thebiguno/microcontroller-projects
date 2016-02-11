float[] x = new float[6];
float[] y = new float[6];


void setup(){
  size(500, 500);
}

void draw(){
  fill(255);
  rect(0, 0, width, height);

  float scale = 2;
  
  //Center
  float cx = 0, cy = 0;
  
  //Body radius
  float br = 45;
  
  //Draw the body
  float cos60 = cos(radians(60));
  float sin60 = sin(radians(60));
  
  //Body co-ordinates (+45)
  float xb = 45, yb = 0, xb1, yb1;
  //Leg Coxa co-ordinates (+48)
  float xlc = 93, ylc = 0, xlc1, ylc1;
  //Leg Foot co-ordinates (+35)
  float xlf = 128, ylf = 0, xlf1, ylf1;
  
  for (int i = 0; i < 6; i++){
    //Rotational transformation matrix
    xb1 = xb * cos60 - yb * sin60;
    yb1 = xb * sin60 + yb * cos60;

    //Draw the body
    stroke(0);
    line(width/2 + xb, height/2 + yb * -1, width/2 + xb1, height/2 + yb1 * -1);
    
    //Calculate the required leg rotation
    stroke(200);
    line(width/2 + xb, height/2 + yb * -1, width/2 + xlc, height/2 + ylc * -1);
    line(width/2 + xlf, height/2 + ylf * -1, width/2 + xlc, height/2 + ylc * -1);
    ellipse(width/2 + xb, height/2 + yb * -1, 5, 5);
    ellipse(width/2 + xlc, height/2 + ylc * -1, 5, 5);
    ellipse(width/2 + xlf, height/2 + ylf * -1, 5, 5);
  
    xb = xb1;
    yb = yb1;
  
    xlc1 = xlc * cos60 - ylc * sin60;
    ylc1 = xlc * sin60 + ylc * cos60;
    xlc = xlc1;
    ylc = ylc1;
    
    xlf1 = xlf * cos60 - ylf * sin60;
    ylf1 = xlf * sin60 + ylf * cos60;
    xlf = xlf1;
    ylf = ylf1;
  }
  
  //Simulations for FrontLeft leg
  moveLeg(110, 10, 0);
  moveLeg(80, 110, 60);
  moveLeg(-67, 79, 120);
  moveLeg(-67, -79, -120);
  
  
  //Simulations for MiddleRight leg
}

void moveLeg(float x, float y, float mountingAngle){
  stroke(255, 0, 0);
  ellipse(width/2 + x, height/2 + y * -1, 5, 5);
  
  //Rotate and translate the leg to get to 'leg' co-ordinate system, with 0,0 at coxa/femur joint
  float x1 = x * cos(radians(mountingAngle * -1)) - y * sin(radians(mountingAngle * -1)) - 45;
  float y1 = x * sin(radians(mountingAngle * -1)) + y * cos(radians(mountingAngle * -1));
  stroke(0, 255, 0);
  ellipse(width/2 + x1 + 45, height/2 + y1 * -1, 3, 3);
  
//  float angle = atan2(y1, x1);
  float l = sqrt(x1*x1 + y1*y1);
  println("l: " + l);
  
  //Translate and Rotate back for drawing
  x1 += 45;
  float x2 = x1 * cos(radians(mountingAngle)) - y1 * sin(radians(mountingAngle));
  float y2 = x1 * sin(radians(mountingAngle)) + y1 * cos(radians(mountingAngle));
  
  stroke(0, 0, 255);
  ellipse(width/2 + x2, height/2 + y2 * -1, 1, 1);
}
