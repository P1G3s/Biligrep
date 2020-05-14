# Biligrep
A command-line-tool to help ONLY grep dynamics and Download videos from Bilibili (~~alpha~~ 'dont know if it works' version)

### What purpose this tool serves
- Nerf Dopamine
- To prevent time from being wasted because of the evil recommendation
- Improve mental health by not showing soft porn thumbnail and click bait (partially, they could be in the title as well :( )
- This tool is meant to show your own dynamics only (Searching may be implemented in the future or not), no fancy tricks

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
- Then you should be good to go (See Handling section below)

### Handling
- Press 'j' to move the cursor to next video
- Press 'k' to move the cursor to last video
- Press 'l' to browse your history dynamics
- Press 'h' to browse your newer dynamics 
- Press 'd' to download the video
- Press 'p\<CR>' to play the video online in mpv
