import processing.serial.*;
import controlP5.*;

ControlP5 cp5;

Serial myPort;  // Create object from Serial class
float inByte = 0;

void setup() 
{
  size(600, 400); //make our canvas 200 x 200 pixels big
  String portName = Serial.list()[4]; //change the 0 to a 1 or 2 etc. to match your port
  println(portName);
  myPort = new Serial(this, portName, 57600);
  myPort.bufferUntil('\n');

  cp5 = new ControlP5(this);

  // Brightness Slider
  cp5.addSlider("Brightness", 0, 255, 255, 260, 120, 14, 100);
  cp5.addTextfield("Brightness Text", 220, 270, 100, 20);

  // Color Temperature Slider
  cp5.addSlider("Temperature", 0, 200, 100, 110, 120, 14, 100);
  cp5.addTextfield("Temperature Text", 70, 270, 100, 20);
  
  // Mode Buttons
  cp5.addButton("DayLight").setPosition(370,160).setSize(150,19);
  cp5.addButton("NightLight").setPosition(370,180).setSize(150,19);
  cp5.addButton("PartyLight").setPosition(370,200).setSize(150, 19);
}

void draw() {
  background(0);
}

public void Temperature(int theValue) {
  Textfield txt = ((Textfield)cp5.getController("Temperature Text"));
  txt.setValue(""+theValue);
  
  int[] temp = colorTemperatureToRGB(theValue);
  String t = temp[0] + "," + temp[1] + "," + temp[2];
  myPort.write("t" + t + "k");
  
  //println(temp);
}

public void Brightness(int theValue) {
  Textfield txt = ((Textfield)cp5.getController("Brightness Text"));
  txt.setValue(""+theValue);
  String d = str(theValue);
  myPort.write("b" + d + "k");
}

public void DayLight() {
  myPort.write("m" + "200,168,0" + "k");
}

public void NightLight() {
  myPort.write("m" + "0,0,255" + "k");
}

public void PartyLight() {
  myPort.write("pk");
}

int[] colorTemperatureToRGB(int kelvin) {
  int temp = kelvin;

  int red, green, blue;

  if (temp <= 66) {
    red = 220;
    green = temp + 10;
    green = (int)(99.4708025861 * Math.log(green) - 161.1195681661);

    if (temp <= 10) {
      blue = 10;
    } else {
      blue = temp;
      blue = (int)(138.5177312231 * Math.log(blue) - 305.0447927307);
    }
  } else {
    red = temp - 60;
    red = (int)(329.698727446 * Math.pow(red, -0.1332047592));

    green = temp - 50;
    green = (int)(288.1221695283 * Math.pow(green, -0.0755148492 ));

    blue = 255;
  }

  int[] temp2 = new int[3];

  int r = clamp(red, 0, 255);
  int g = clamp(green, 0, 255);
  int b = clamp(blue, 0, 255);

  temp2[0] = r;
  temp2[1] = g;
  temp2[2] = b;

  return temp2;
}

int clamp(int x, int min, int max) {
  if (x < min) {
    return min;
  }
  if (x > max) {
    return max;
  }

  return x;
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
    // convert to an int and map to the screen height:
    inByte = float(inString);
    println(inString);
    inByte = map(inByte, 0, 1023, 0, height);
  }
}
