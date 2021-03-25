# Biligrep 
A command-line-tool that helps grep dynamics and ~~Download~~ play videos from Bilibili

### What purpose this tool serves
- Nerf Dopamine

### What it can not do
- Choose video quality
- Only video dynamics will be fetched, it currently disards all shared dynamics, pictures, articles, videos, etc.
- ~~Indicate the format it downloads (it seems to downlaod .flv at the moment)~~

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

### Handling (vim-like)
- Press 'Enter' to play the video on mpv
- Press 'j' to move the cursor to next dynamic(video)
- Press 'k' to move the cursor to last dynamic(video)
- Press 'l' to browse your history dynamics
- Press 'h' to browse your newer dynamics 
- ~~Press 'd' to download the video~~ (no longer available, play it on mpv instead)
