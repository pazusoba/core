# Automation
This is the ultimate goal for pazusoba, actually solving and doing the puzzle all by the AI. Currently, I am using `scrcpy` to mirror and control my phone (Android only). This way, I can simply capture a screenshot and get the board directly.

## Orbs
There are 5 colours, recovery, jammer, bomb, poison and poison+. The main goal is to at least detect 5 colours (R, B, G, L, D) and recovery (H). 
Now, there are different orb styles in the game so there should be a range for every colour and simple matching won't work for all styles. 
However, for starters, it is fine just to detect the most tradition style. 
Recoevery, jammer, bomb and poison are fixed and they will not change. Therefore, they can be matched with a sample image.

### Codes
- Fire/Red (R)
- Water/Blue (B)
- Wood/Green (G)
- Light (L)
- Dark (D)
- Recovery/Heal (H)

## iOS
It is easily to control Android devices from desktop but for iOS, it is a bit more challenging. It is indeed possible to do it but I haven't found a great solution yet. However, it is still easy to mirror iOS devices so maybe I can show the path somehow but I still need to do the puzzle by myself.

### Tools
- https://er.run/ but it is paid for iOS
- `Dell Mobile Connect`, all in one solution but it requires a `dell` computer
- Jailbreak your device but not recommended
