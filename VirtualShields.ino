/*
    Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.

    The MIT License(MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files(the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions :

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

/*
    About the Graphics Shield:
    
    Graphics includes, but goes beyond Text to add truetype font (drawAt),
  images, buttons, rectangles, lines, colors and more.
    
*/
int buttonId, rectangleId;

void screenEvent(ShieldEvent* shieldEvent) 
{
  if (screen.isPressed(rectangleId)) 
  {
    screen.change(buttonId, ARGB(255,0,0));
    screen.change(rectangleId, ARGB(255,0,0));
  }
  
  if (screen.isReleased(rectangleId)) 
  {
    screen.change(buttonId, ARGB(0,255,0));
    screen.change(rectangleId, ARGB(0,255,0));
    reason = "VirtualShields";
    manualOverride(reason);
    reason = "";
  }
}

// Callback for startup, reconnection, and when the pushing 'Refresh' button
void refresh(ShieldEvent* shieldEvent)
{
  // clear the screen to RED
  screen.clear(ARGB(215,48,34));
  
  screen.drawAt(0,0, "This is the IoT Gatekeeper Virtual Shields interface");
  screen.drawImage(0,30, "https://blog.arduino.cc/wp-content/uploads/2015/04/windowsarduino.jpg", "", 300, 100);
  //buttonId = screen.addButton(0, 175, "Button here");
  screen.drawAt(0,600, "Manual Override Switch");
  rectangleId = screen.fillRectangle(0, 640, 100, 50, GREEN);

  if (digitalRead(9) == 0)
  {
    screen.printAt(15, "Gate open.");
  }
  else if (digitalRead(9) == 1)
  {
    screen.printAt(15, "Gate closed.");
  }
}
