void setup(){
  size(200, 200);
}

void draw(){
  float scale = 2;
  
  fill(255);
  rect(0, 0, width, height);
  /*
  
  |
B o  
  | \
  |  \ a
  |   \
 h|    o
  |   / b
  |--o
 z|  |
  |__|______
    x
    
  */
  
  //Height of coxa / femur joint
  float h = 68 * scale;
  
  //Desired co-ordinates of foot
  float x = mouseX;
  float z = height - mouseY;
  
  //a is the femur; b is the tibia, c is the distance from coxa / femur joint to end of tibia.
  float a, b, c; 
  //The angles opposite their lower case counterparts
  float A, B, C;
  //The co-ordinates in x,z space for points A, B, and C
  float Ax,Az,Bx,Bz,Cx,Cz;   
  
  //Bx,Bz is the location on X,Z plane where the leg originates 
  Bx = 0;
  Bz = h;
  
  //We know a and b by measurement (length of leg segments)
  a = 20 * scale;
  b = 57 * scale;
  //We can find c using pythagorean theorem with (h-z), x
  c = sqrt((h-z)*(h-z) + x*x);
  
  //Use low of cosines to find A, B, and C
  A = acos((b*b + c*c - a*a) / (2 * b * c));
  B = acos((a*a + c*c - b*b) / (2 * a * c));
  C = acos((a*a + b*b - c*c) / (2 * a * b));
  
  //Find X using arc cosine: cos(X) = z/a; 
  float X = acos((h-z)/c);
  
  //To find Cx,Cz we first find the angle (Temp1) between vertical and side 'a'
  float Temp1 = PI - B - X;
  Cx = sin(Temp1) * a;
  //Use pythagorean theorem to find rise of Cz over Bz.  Subtract this value from 
  // Bz to find absolute co-ordinates 
  Cz = Temp1 > PI / 2 ? (Bz - sqrt(a*a - Cx*Cx)) : (Bz + sqrt(a*a - Cx*Cx));
   
  //Now we need to find Ax,Az.
  Ax = x;
  Az = z;
  //TODO
  
  println();
  println("a=" + a + ", b=" + b + ", c=" + c);
  println("A=" + degrees(A) + ", B=" + degrees(B + X) + ", C=" + degrees(C));
  println("x=" + x + ", z=" + z);
  println("X=" + degrees(X));
  println("Ax=" + Ax + ", Az=" + Az + ", Cx=" + Cx + ", Cz=" + Cz);
  
  //Print everything to the screen
  fill(0);
  stroke(255, 0, 0, 100);
  ellipse(Bx, height - Bz, 10, 10);
  stroke(0, 255, 0, 100);
  ellipse(Cx, height - Cz, 10, 10);
  stroke(0, 0, 255, 100);
  ellipse(Ax, height - Az, 10, 10);

  stroke(0);
  line(Bx, height - Bz, Cx, height - Cz);
  line(Cx, height - Cz, Ax, height - Az);
}
