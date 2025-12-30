# block-shooter-portable

A portable version of my main game **Block Shooter**, built using  
[Raylib](https://github.com/raysan5/raylib).

This project is developed *alongside* the main game as a flexible version that
can be forked, modified, and ported to different platforms over time.

- The original game’s repository is **not yet public**
- The main version of the game is made in **Unity 2020 LTS**
- This repository is intended to assist with future native ports, including:

  - Nintendo DS  
  - Nintendo 64 *(not confirmed)*  
  - Wii U *(if developer Unity tools are not obtained)*  
  - Nintendo Switch  

Raylib is used here for **desktop development and testing only**.  
Console ports will not use Raylib directly unless I find good ports for it.

---

## Purpose

The goal of this repository is to:

- Keep gameplay logic portable and engine-independent
- Allow rapid testing outside of Unity
- Prepare the codebase for future console ports
- Use a simple and transparent command-line build system

---

## Building the Game

This project uses a **Makefile** and is built entirely from the command line.

### Supported Platforms
- Windows (MSYS2 + MinGW64) — **Recommended**
- Linux (Debian / Ubuntu and similar)

### Not Supported
- macOS (not supported or tested)

---

## Windows Build (MSYS2 + MinGW64) — Recommended

This is the primary development and testing environment.

### 1. Install MSYS2
Download MSYS2 from:  
https://www.msys2.org

After installation, open **MSYS2 MinGW64**  
(do not use MSYS or UCRT).

### 2. Update packages
```bash
pacman -Syu
```
### 3. Install dependencies
```
pacman -S mingw-w64-x86_64-gcc 
pacman mingw-w64-x86_64-make 
pacman mingw-w64-x86_64-raylib
```
### 4. Make
Run the ``make`` command in the project root to create a ``./build/main.exe`` you have to copy the dll's from the built_libraries folder yourself.
```
make
```
