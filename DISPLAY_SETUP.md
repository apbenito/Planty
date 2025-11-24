# GxEPD2 Custom Display Setup (212x104)

## Problem
Your 212×104 e-ink display is **not** in GxEPD2's pre-defined classes. The library has many standard sizes (2.13", 2.9", 4.2", etc.) but your exact resolution requires custom setup.

## Solution Overview

### Quick Fix (Temporary)
`main.cpp` currently uses `GxEPD2_213` (250×122) as a placeholder. This will compile but may not display correctly since the controller initialization and timing might differ.

### Proper Fix (What You Need)

To get your 212×104 display working correctly, you need to:

1. **Identify your display's controller chip**
   - Look at your display module's datasheet or silkscreen
   - Common controllers: SSD1680, SSD1681, SSD1608, UC8179, etc.
   - Example: Waveshare 2.13" B/W uses SSD1680

2. **Find a GxEPD2 class using the SAME controller**
   - Once you know the controller, find a GxEPD2 class that uses it
   - Example: If your display uses SSD1680, look for other SSD1680 displays in GxEPD2
   - Available controllers in library:
     - `src/epd/` — B/W displays (GxEPD2_213, GxEPD2_290, etc.)
     - `src/epd3c/` — 3-color displays (GxEPD2_213c, GxEPD2_290_C90c, etc.)

3. **Update `main.cpp`**
   - Replace `GxEPD2_213` with your display's correct class
   - Example: `GxEPD2_BW<GxEPD2_290, 104> display(GxEPD2_290(...));`

4. **Adjust the buffer height**
   - The second parameter (104) in `GxEPD2_BW<GxEPD2_213, 104>` is your height
   - Keep it as 104 for your display

## Step-by-Step

### Step 1: Find Your Display's Controller
Check your display module:
- Look for a chip labeled SSD1680, SSD1681, UC8179, etc.
- Check the Waveshare/Good Display product page
- Look at your purchase receipt or datasheet

### Step 2: Locate the Matching GxEPD2 Class
Navigate to `.pio/libdeps/esp32dev/GxEPD2/src/epd/` and look for files with your controller name.

For example, if you have SSD1680:
- `GxEPD2_213.h` / `GxEPD2_213.cpp` → SSD1680 controller
- `GxEPD2_270.h` / `GxEPD2_270.cpp` → SSD1680 controller (also uses SSD1680)

### Step 3: Update main.cpp Line 25
Change this:
```cpp
GxEPD2_BW<GxEPD2_213, 104> display(GxEPD2_213(EPD_SS, EPD_DC, EPD_RST, EPD_BUSY));
```

To (example with SSD1680):
```cpp
GxEPD2_BW<GxEPD2_290, 104> display(GxEPD2_290(EPD_SS, EPD_DC, EPD_RST, EPD_BUSY));
```

Replace `GxEPD2_290` with whichever class matches your controller.

## Advanced: Custom Display Class

If no pre-defined class matches your controller, use the template in `include/GxEPD2_Custom212x104.h`:

1. Copy initialization code from a similar display's `.cpp` file
2. Adapt width/height to 212×104
3. Include it in main.cpp:
   ```cpp
   #include "GxEPD2_Custom212x104.h"
   GxEPD2_BW<GxEPD2_Custom212x104, 104> display(GxEPD2_Custom212x104(EPD_SS, EPD_DC, EPD_RST, EPD_BUSY));
   ```

## Need Help?

Tell me:
- **Your display's exact model** (e.g., "Waveshare 2.13" B/W module")
- **Your display's controller chip** (silkscreen or datasheet)
- **Datasheet link** (if you have it)

Then I can give you the exact line to change!

