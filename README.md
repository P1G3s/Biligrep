# Biligrep
A command-line-tool to help ONLY grep dynamics and Download videos from Bilibili

### What purpose this tool serves
- Nerf Dopamine

### What it cant do
- Choose video quality
- Only original video dynamics will be shown, it currently disards all shared dynamics, pictures, article, etc.
- Indicate the format it downloads (it seems to downlaod .flv at the moment)

### Requirement
- libncurses5-dev
- libmpv-dev

### Usage
- Install those libs above
- Edit the header_temp.h: go to the bilibili get your 'SESSDATA' from Cookies, and paste it in the field "SESSDATA="
- Change the file name of 'header_temp.h' to 'header.h'
- ```
  cd ./Biligrep
  make
  ./biligrep
  ```
- Then you should be good to go

### Handling
- Press 'j' to move the cursor to next dynamic(video)
- Press 'k' to move the cursor to last dynamic(video)
- Press 'l' to browse your history dynamics
- Press 'h' to browse your newer dynamics 
- Press 'd' to download the video
- Press '<CR>' to play the video online in mpv-
