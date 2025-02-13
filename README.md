# Multris
A modified version of the classic puzzle game, with randomly generated polyominoes of varying sizes.

## Table of Contents
[Showcase](#Showcase)

[How to Play](#How_to_Play)

[Building From Source](#Building_From_Source)

[Pre-Compiled Downloads](#Pre_Compiled_Downloads)

<a name="Showcase"/>

## Showcase
![Showcase](Showcase.gif)

<a name="How_to_Play"/>

## How to Play

##### DEFAULT CONTROLS
```
LEFT / RIGHT ARROW - Move. Hold to move quicker.
        DOWN ARROW - Soft Drop
          UP ARROW - Hard Drop
             SPACE - Hold current piece / Swap current piece with held piece
                 Z - Rotate CCW
                 X - Rotate CW
                 C - Mirror piece over Y-Axis
             ENTER - Select/Pause
            ESCAPE - Exit current menu.
``` 
##### GAME MODES
```
    MULTRIS - All pieces are a random size. 
  NUMERICAL - All pieces are the selected size.
              Unlock larger sizes by reaching high levels in smaller sizes.
     CUSTOM - Same as MULTRIS mode except that the maximum piece size can be selected.
MULTIPLAYER - Battle against another player in MULTRIS mode.
```           
##### GAMEPLAY
```
SPEED - Speed increases as level increases.
LEVEL - Level increases as you complete lines.
        The number of completed lines needed to advance to the next level increases with each level.
Score - You score points by completing lines.
        The more lines you complete at one time, the more points you get.
        Completing lines are also worth more points at higher levels.
```
##### OPTIONS
```
  FULLSCREEN - Enable or disable fullscreen mode
     SFX VOL - Set the volume of the game audio. Between 0-100.
   MUSIC VOL - Set the volume of the background music. Between 0-100.
   LIMIT FPS - Limit the FPS of the game to twice that of the refresh rate of the display.
               This is to lower CPU usage.
    SHOW FPS - Enable/Disable drawing the FPS to the screen.
  CENTER DOT - Enable to draw a dot in the center of the current piece while playing.
    GRAPHICS - Switch between newer graphics and older legacy graphics.
    CONTROLS - Modify the keybindings for the game.
```

<a name="Building_From_Source"/>

## Building From Source

### Requirements

###### Make sure that you have the development libraries as well as the runtime binaries installed

[SDL2](https://github.com/libsdl-org/SDL)

[SDL_Image](https://github.com/libsdl-org/SDL_image)

[SDL_Net](https://github.com/libsdl-org/SDL_net)

[SDL_Mixer](https://github.com/libsdl-org/SDL_mixer)

For all following instructions, replace `make -f Makefile_Linux` with `make -f Makefile_Windows` if you are on Windows. Also, edit `Makefile_Windows` to set the `SDL2_DIR`, `SDL2IMAGE_DIR`, `SDL2NET_DIR`, `SDL2MIXER_DIR` to the appropriate folders for your SDL2 development files.

#### Building the Latest Development Version (Unstable)

```
git clone https://github.com/RustyReich/Multris
cd Multris
make -f Makefile_Linux
```

#### Building a Specific Release Version (Stable)

###### For versions 1.0.8+

```
git clone https://github.com/RustyReich/Multris --branch v1.x.x
cd Multris
make -f Makefile_Linux
```

###### For versions 1.0.4 - 1.0.7

```
git clone https://github.com/RustyReich/Multris --branch v1.x.x
cd Multris
mkdir unix_obj
mkdir unix_dep
mkdir build
wget https://raw.githubusercontent.com/RustyReich/Multris/main/Makefile_Linux
make -f Makefile_Linux
cp -R ./src/AUDIO ./build/AUDIO
cp -R ./src/SPRITES ./build/SPRITES
```


###### Build instructions are not available for versions before 1.0.4

###### The executable will be in `/Multris/build`

<a name="Pre_Compiled_Downloads"/>

## Pre-Compiled Downloads
Since version 1.0.7, Pre-Compiled builds are only available for purchase on [Steam](https://store.steampowered.com/app/1768350/Multris/)

Older versions (1.0.0 - 1.0.6) are available for free on [Itch.io](https://rustymonster.itch.io/multris)
