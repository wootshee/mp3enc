# mp3enc
Simple LAME-based mp3 encoder. Converts all .wav files in a directory specified on command line argument to MP3s.
Utilizes all CPU cores to speedup encoding.

## Building from sources

```
git clone git@github.com:wootshee/mp3enc.git
cd mp3enc
```
### Linux
```
./configure
make
sudo make install
```
### Windows
Please use Visual Studio solution `vstudio/mp3enc.sln`.

Alternatively, use MSBuild:

```
msbuild /p:Configuration=Debug vstudio/mp3enc.sln
msbuild /p:Configuration=Release vstudio/mp3enc.sln
```

Windows binaries are built in `output/<ConfigurationName>` folder.

## Usage
```
mp3enc <directory_path>
