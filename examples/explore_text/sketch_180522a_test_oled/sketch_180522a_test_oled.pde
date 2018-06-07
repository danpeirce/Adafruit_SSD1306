import processing.serial.*;
Serial myPort; 

void setup() {
  size(230, 270);
  myPort = new Serial(this, "COM27", 9600);
  textSize(10);
}

void draw() {
  background(100);
  text(">" , 10, 20);
  text(key, 20, 20); // Draw "key" at coordinate (10,20)
  text("~ - Clear Screen", 20, 40); // start of Menu
  text("! - Size 1", 20, 55); // 
  text("@ - Size 2", 20, 70); // 
  text("# - Size 3", 20, 85); // 
  text("$ - Size 4", 20, 100); // 
  text("TAB or % - Landscape Mode", 20, 115); // 
  text("^ - Portrait Mode", 20, 130); 
  text("& - shift out (codes)", 20, 145);// 
  text("` - enter x & y position", 25, 160);// end Menu
}

void keyPressed() {
    if (key == '~') {
      myPort.write(0x0C); // clear screen
    } else if (key == '!') {
      myPort.write(0x11); // size 1
    } else if (key == '@') {
      myPort.write(0x12); // size 2
    } else if (key == '#') {
      myPort.write(0x13); // size 3
    } else if (key == '$') {
      myPort.write(0x14); // size 4
    } else if (key == '%') {
      myPort.write(0x09); // Landscape Mode
    } else if (key == '^') {
      myPort.write(0x0B); // Portrait Mode
    } else if (key == '&') {
      myPort.write(0x0E); // shift out
    } else if (key == CODED)
    {
          
    } else
    {
       myPort.write(key);
    }
}