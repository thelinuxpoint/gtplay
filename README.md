# Gt-Play
[![C/C++ CI](https://github.com/thelinuxpoint/gtplay/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/thelinuxpoint/gtplay/actions/workflows/c-cpp.yml)  <img src="https://img.shields.io/badge/version-v0.1.0-brightgreen"/>  [![GitHub license](https://badgen.net/github/license/Naereen/Strapdown.js)](https://github.com/thelinuxpoint/gtplay/blob/master/LICENSE)


Raw mp3 decoder designed with ALSA ( advanced linux sound architecture )

## Dependencies
```
sudo apt-get install libasound2-dev
```

## Downloading
```
git clone https://github.com/thelinuxpoint/gtplay.git
```
## Building
go to the gtplay directory created as a result from the above step
```
cd gtplay
```
now buid the project using the following commands
```
make
make clean
```

## Playing
```
./gtplay your_song_path.mp3
```
<div align="center"><img src="https://raw.githubusercontent.com/thelinuxpoint/gtplay/main/icons/gtplay.png" /></div>
