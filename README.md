# Intro
HSMP exposes kernel-space memory R/W/X capabilities to user-space. At the time of this project being used, it was shipped as a signed driver. After loading it into the operating system,
you could map unsigned code into kernel-space and also execute it. Once that was done, all thats left was to unload the driver and clear traces.<br>
<br>
This repo will act as an archive, due to the project being long discontinued. Also keep in mind that this doesn't form an anti-cheat evasion in any way.
The allocated memory is freely enumerable by anyone, and there's also a couple other stuff to worry about.<br>
<br>
The HSCTL component will perform a few checks in order to determine if the driver functions properly.<br>
Keep in mind this is 1½ years old, and has not been written for production.<br>
# References
 - UC
 - definitely some other fellow github repos!
 
